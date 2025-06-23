#ifndef ATTACKS_HPP
#define ATTACKS_HPP

#include "types.hpp"
#include "magic.hpp"
#include "nonmagic.hpp"
#include "board.hpp"

U64 pawn_attacks(Color side, int sq);
U64 knight_attacks(int sq);
U64 king_attacks(int sq);
U64 rook_attacks(int sq, U64 full_occ);
U64 bishop_attacks(int sq, U64 full_occ);
U64 queen_attacks(int sq, U64 occ);

bool isSquareAttacked(int sq, const Board& board, int Side);
void print_attacked_squares(const Board& board, int Side);

#endif // ATTACKS_HPP
