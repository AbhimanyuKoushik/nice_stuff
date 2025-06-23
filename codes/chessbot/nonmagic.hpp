#pragma once

#include <cstdint>
#include <iostream>
#include "bitboard.hpp"
#include "types.hpp"

// Non-sliding piece attack masks and initialization

// Attack tables (defined in .cpp)
extern Bitboard PawnAttacks[2][64];
extern Bitboard KnightAttacks[64];
extern Bitboard KingAttacks[64];
extern Bitboard bishop_masks[64];
extern Bitboard rook_masks[64];

inline U64 mask_pawn_attacks(int side, int square)
{
    // result attacks bitboard
    U64 attacks = 0ULL;

    // piece bitboard
    U64 bitboard = 0ULL;
    
    // set piece on board
    set_bit(bitboard, square);
    
    // white pawns
    if (side == White)
    {
        // generate pawn attacks
        if ((bitboard >> 7) & not_a_file) attacks |= (bitboard >> 7);
        if ((bitboard >> 9) & not_h_file) attacks |= (bitboard >> 9);
    }
    
    // black pawns
    else
    {
        // generate pawn attacks
        if ((bitboard << 7) & not_h_file) attacks |= (bitboard << 7);
        if ((bitboard << 9) & not_a_file) attacks |= (bitboard << 9);    
    }
    
    // return attack map
    return attacks;
}

// generate knight attacks
inline U64 mask_knight_attacks(int square)
{
    // result attacks bitboard
    U64 attacks = 0ULL;

    // piece bitboard
    U64 bitboard = 0ULL;
    
    // set piece on board
    set_bit(bitboard, square);
    
    // generate knight attacks
    if ((bitboard >> 17) & not_h_file) attacks |= (bitboard >> 17);
    if ((bitboard >> 15) & not_a_file) attacks |= (bitboard >> 15);
    if ((bitboard >> 10) & not_hg_file) attacks |= (bitboard >> 10);
    if ((bitboard >> 6) & not_ab_file) attacks |= (bitboard >> 6);
    if ((bitboard << 17) & not_a_file) attacks |= (bitboard << 17);
    if ((bitboard << 15) & not_h_file) attacks |= (bitboard << 15);
    if ((bitboard << 10) & not_ab_file) attacks |= (bitboard << 10);
    if ((bitboard << 6) & not_hg_file) attacks |= (bitboard << 6);

    // return attack map
    return attacks;
}

// generate king attacks
inline U64 mask_king_attacks(int square)
{
    // result attacks bitboard
    U64 attacks = 0ULL;

    // piece bitboard
    U64 bitboard = 0ULL;
    
    // set piece on board
    set_bit(bitboard, square);
    
    // generate king attacks
    if (bitboard >> 8) attacks |= (bitboard >> 8);
    if ((bitboard >> 9) & not_h_file) attacks |= (bitboard >> 9);
    if ((bitboard >> 7) & not_a_file) attacks |= (bitboard >> 7);
    if ((bitboard >> 1) & not_h_file) attacks |= (bitboard >> 1);
    if (bitboard << 8) attacks |= (bitboard << 8);
    if ((bitboard << 9) & not_a_file) attacks |= (bitboard << 9);
    if ((bitboard << 7) & not_h_file) attacks |= (bitboard << 7);
    if ((bitboard << 1) & not_a_file) attacks |= (bitboard << 1);
    
    // return attack map 
    return attacks;
}

inline void init_nonsliders() {
    std::cout << "Initializing Non-sliders...\n";
    for (int sq = 0; sq < 64; ++sq) {
        PawnAttacks[0][sq] = mask_pawn_attacks(0, sq);
        PawnAttacks[1][sq] = mask_pawn_attacks(1, sq);
        KnightAttacks[sq] = mask_knight_attacks(sq);
        KingAttacks[sq] = mask_king_attacks(sq);
    }
}