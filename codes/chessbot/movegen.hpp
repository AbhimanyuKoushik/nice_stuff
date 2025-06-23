#ifndef MOVEGEN_HPP
#define MOVEGEN_HPP

#include "types.hpp"
#include <iostream>

// Forward declaration
struct Board;

// Function declarations only - NO inline keywords
void generate_pawn_moves(int Side, const Board& board);
void generate_king_moves(int Side, const Board& board);

#endif // MOVEGEN_HPP
