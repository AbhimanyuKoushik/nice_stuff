#ifndef ATTACKS_HPP
#define ATTACKS_HPP

#include "types.hpp"
#include "magic.hpp"
#include "nonmagic.hpp"

// Piece-specific attack generation
inline U64 pawn_attacks(Color side, int sq){
    return PawnAttacks[side == White ? 0 : 1][sq];
}

inline U64 knight_attacks(int sq){
    return KnightAttacks[sq];
}

inline U64 king_attacks(int sq){
    return KingAttacks[sq];
}

inline U64 rook_attacks(int sq, U64 full_occ) {
    U64 occ = full_occ & rook_mask(sq);
    int idx = int((occ * RookMagics[sq]) >> (64 - RookRelevantBits[sq]));
    return RookAttacks[sq][idx];
}

inline U64 bishop_attacks(int sq, U64 full_occ) {
    U64 occ = full_occ & bishop_mask(sq);
    int idx = int((occ * BishopMagics[sq]) >> (64 - BishopRelevantBits[sq]));
    return BishopAttacks[sq][idx];
}

inline U64 queen_attacks(int sq, U64 occ) {
    return rook_attacks(sq, occ) | bishop_attacks(sq, occ);
}

inline bool isSquareAttacked(int sq, const Board& board, int Side) {
    // Checking using if a piece exists on the attacking square, check if same piece exists on the square it attacks
    // if there then the square is attacked
    if (Side == White && (PawnAttacks[Black][sq] & board.bitboards[bP])) return true;
    if (Side == Black && (PawnAttacks[White][sq] & board.bitboards[wP])) return true;
    if (KnightAttacks[sq] & ((Side == White) ? board.bitboards[wN] : board.bitboards[bN])) return true;
    if (KingAttacks[sq] & ((Side == White) ? board.bitboards[wK] : board.bitboards[bK])) return true;
    if (bishop_attacks(sq, board.occupancies[2]) & ((Side == White) ? board.bitboards[wB] : board.bitboards[bB])) return true;
    if (rook_attacks(sq, board.occupancies[2]) & ((Side == White) ? board.bitboards[wR] : board.bitboards[bR])) return true; 
    if (queen_attacks(sq, board.occupancies[2]) & ((Side == White) ? board.bitboards[wQ] : board.bitboards[bQ])) return true;
    
    return false;
}

#endif // ATTACKS_HPP
