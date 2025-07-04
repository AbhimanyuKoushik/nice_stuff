#ifndef ATTACKS_HPP
#define ATTACKS_HPP

// attacks.hpp
#pragma once
#include "types.hpp"    // for U64/Color, if needed
struct Position;  

U64 pawn_attacks(Color side, int sq);
U64 knight_attacks(int sq);
U64 king_attacks(int sq);
U64 rook_attacks(int sq, U64 full_occ);
U64 bishop_attacks(int sq, U64 full_occ);
U64 queen_attacks(int sq, U64 occ);

bool isSquareAttacked(int sq, const Position& position, int Side);
void print_attacked_squares(const Position& position, int Side);

#endif // ATTACKS_HPP