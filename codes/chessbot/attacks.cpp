// attacks.cpp
#include "attacks.hpp"
#include "board.hpp"
#include "types.hpp"
#include "magic.hpp"
#include "nonmagic.hpp"

// Piece-specific attack generation
U64 pawn_attacks(Color side, int sq){
    return PawnAttacks[side == White ? 0 : 1][sq];
}

U64 knight_attacks(int sq){
    return KnightAttacks[sq];
}

U64 king_attacks(int sq){
    return KingAttacks[sq];
}

U64 rook_attacks(int sq, U64 full_occ) {
    U64 occ = full_occ & rook_mask(sq);
    int idx = int((occ * RookMagics[sq]) >> (64 - RookRelevantBits[sq]));
    return RookAttacks[sq][idx];
}

U64 bishop_attacks(int sq, U64 full_occ) {
    U64 occ = full_occ & bishop_mask(sq);
    int idx = int((occ * BishopMagics[sq]) >> (64 - BishopRelevantBits[sq]));
    return BishopAttacks[sq][idx];
}

U64 queen_attacks(int sq, U64 occ) {
    return rook_attacks(sq, occ) | bishop_attacks(sq, occ);
}

// Returns true is any piece from Side attackes square sq on Board board
bool isSquareAttacked(int sq, const Board& board, int Side) {
    // Checking using if a piece exists on the attacking square, check if same piece exists on the square it attacks
    // if there then the square is attacked
    if (Side == White && (PawnAttacks[Black][sq] & board.bitboards[wP])) return true;
    if (Side == Black && (PawnAttacks[White][sq] & board.bitboards[bP])) return true;
    if (KnightAttacks[sq] & ((Side == White) ? board.bitboards[wN] : board.bitboards[bN])) return true;
    if (KingAttacks[sq] & ((Side == White) ? board.bitboards[wK] : board.bitboards[bK])) return true;
    if (bishop_attacks(sq, board.occupancies[2]) & ((Side == White) ? board.bitboards[wB] : board.bitboards[bB])) return true;
    if (rook_attacks(sq, board.occupancies[2]) & ((Side == White) ? board.bitboards[wR] : board.bitboards[bR])) return true;
    if (queen_attacks(sq, board.occupancies[2]) & ((Side == White) ? board.bitboards[wQ] : board.bitboards[bQ])) return true;
    
    return false;
}

void print_attacked_squares(const Board& board, int Side){
    printf("\n");
    
    // loop over board ranks
    for (int rank = 0; rank < 8; rank++)
    {
        // loop over board files
        for (int file = 0; file < 8; file++)
        {
            // init square
            int square = rank * 8 + file;
            
            // print ranks
            if (!file)
                printf("  %d ", 8 - rank);
            
            // check whether current square is attacked or not
            printf(" %d", isSquareAttacked(square, board, Side) ? 1 : 0);
        }
        
        // print new line every rank
        printf("\n");
    }
    
    // print files
    printf("\n     a b c d e f g h\n\n");
}