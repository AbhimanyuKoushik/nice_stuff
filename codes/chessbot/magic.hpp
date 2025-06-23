#ifndef MAGIC_HPP
#define MAGIC_HPP

#include <iostream>
#include <vector>
#include <array>
#include <cstdint>
#include "types.hpp"
#include "bitboard.hpp"

// ----------- Magic Bitboard Move Generation -----------

using Attacks = std::vector<U64>;
using AttackArray = std::array<Attacks,64>;
extern AttackArray RookAttacks;
extern AttackArray BishopAttacks;

// Compute occupancy mask for rook on square `sq`
inline U64 rook_mask(int sq) {
    U64 mask = 0ULL;
    int r = sq/8, f = sq%8;
    for (int ff = f+1; ff <= 6; ++ff) mask |= (1ULL << (r*8 + ff));
    for (int ff = f-1; ff >= 1; --ff) mask |= (1ULL << (r*8 + ff));
    for (int rr = r+1; rr <= 6; ++rr) mask |= (1ULL << (rr*8 + f));
    for (int rr = r-1; rr >= 1; --rr) mask |= (1ULL << (rr*8 + f));
    return mask;
}

inline U64 bishop_mask(int sq) {
    U64 mask = 0ULL;
    int r = sq/8, f = sq%8;
    for (int d = 1; f+d <= 6 && r+d <= 6; ++d) mask |= (1ULL << ((r+d)*8 + (f+d)));
    for (int d = 1; f-d >= 1 && r+d <= 6; ++d) mask |= (1ULL << ((r+d)*8 + (f-d)));
    for (int d = 1; f+d <= 6 && r-d >= 1; ++d) mask |= (1ULL << ((r-d)*8 + (f+d)));
    for (int d = 1; f-d >= 1 && r-d >= 1; ++d) mask |= (1ULL << ((r-d)*8 + (f-d)));
    return mask;
}

// Generates sliding attacks (naive) used in precompute
inline U64 sliding_attacks_rook(int square, U64 block) {
    U64 blockers = block & rook_mask(square);

     // result attacks bitboard
    U64 attacks = 0ULL;
    
    // init ranks & files
    int r, f;
    
    // init target rank & files
    int tr = square / 8;
    int tf = square % 8;
    
    // generate rook attacks
    for (r = tr + 1; r <= 7; r++)
    {
        attacks |= (1ULL << (r * 8 + tf));
        if ((1ULL << (r * 8 + tf)) & block) break;
    }
    
    for (r = tr - 1; r >= 0; r--)
    {
        attacks |= (1ULL << (r * 8 + tf));
        if ((1ULL << (r * 8 + tf)) & block) break;
    }
    
    for (f = tf + 1; f <= 7; f++)
    {
        attacks |= (1ULL << (tr * 8 + f));
        if ((1ULL << (tr * 8 + f)) & block) break;
    }
    
    for (f = tf - 1; f >= 0; f--)
    {
        attacks |= (1ULL << (tr * 8 + f));
        if ((1ULL << (tr * 8 + f)) & block) break;
    }
    
    // return attack map
    return attacks;
}

// Naive slidingâ€bishop, but only looks at the diagonal blockers.
// `block` is the full occupancy; we mask it first.
inline U64 sliding_attacks_bishop(int square, U64 block) {
    // Only keep the diagonal blockers
    U64 blockers = block & bishop_mask(square);

    U64 attacks = 0ULL;
    int tr = square / 8, tf = square % 8;
    int r, f;

    // Northâ€‘East
    for (r = tr + 1, f = tf + 1; r < 8 && f < 8; ++r, ++f) {
        U64 sqMask = 1ULL << (r*8 + f);
        attacks |= sqMask;
        if (blockers & sqMask) break;
    }
    // Southâ€‘East
    for (r = tr - 1, f = tf + 1; r >= 0 && f < 8; --r, ++f) {
        U64 sqMask = 1ULL << (r*8 + f);
        attacks |= sqMask;
        if (blockers & sqMask) break;
    }
    // Northâ€‘West
    for (r = tr + 1, f = tf - 1; r < 8 && f >= 0; ++r, --f) {
        U64 sqMask = 1ULL << (r*8 + f);
        attacks |= sqMask;
        if (blockers & sqMask) break;
    }
    // Southâ€‘West
    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; --r, --f) {
        U64 sqMask = 1ULL << (r*8 + f);
        attacks |= sqMask;
        if (blockers & sqMask) break;
    }

    return attacks;
}

// Precompute magic tables with debug printing
inline void init_sliders() {
    std::cout << "Initializing magic sliders...\n";
    for (int sq = 0; sq < 64; ++sq) {
        U64 mask = rook_mask(sq);
        int bits = RookRelevantBits[sq];
        int entries = 1 << bits;
        RookAttacks[sq].resize(entries);
        for (int idx = 0; idx < entries; ++idx) {
            // build occupancy subset
            U64 occSubset = 0ULL;
            U64 tempMask = mask;
            for (int bit = 0; bit < bits; ++bit) {
                U64 b = tempMask & -tempMask;
                tempMask -= b;
                if (idx & (1 << bit)) occSubset |= b;
            }
            int magic_index = int((occSubset * RookMagics[sq]) >> (64 - bits));
            U64 attacks = sliding_attacks_rook(sq, occSubset);

            RookAttacks[sq][magic_index] = attacks;
        }

        mask = bishop_mask(sq);
        bits = BishopRelevantBits[sq];
        entries = 1 << bits;
        BishopAttacks[sq].resize(entries);
        for (int idx = 0; idx < entries; ++idx) {
            U64 occSubset = 0ULL;
            U64 tempMask = mask;
            for (int bit = 0; bit < bits; ++bit) {
                U64 b = tempMask & -tempMask;
                tempMask -= b;
                if (idx & (1 << bit)) occSubset |= b;
            }
            int magic_index = int((occSubset * BishopMagics[sq]) >> (64 - bits));
            U64 attacks = sliding_attacks_bishop(sq, occSubset);

            BishopAttacks[sq][magic_index] = attacks;
        }
    }
}

#endif // MAGIC_HPP
 