#ifndef MOVEGEN_HPP
#define MOVEGEN_HPP


#pragma once
#include "types.hpp"    // for U64/Color, if needed
#include "movedef.hpp"
struct Board;

// Function declarations only - NO inline keywords
void generate_pawn_moves(int Side, const Board& board, Moves& move_list);
void generate_king_moves(int Side, const Board& board, Moves& move_list);
void generate_knight_moves(int Side, const Board& board, Moves& move_list);
void generate_bishop_moves(int Side, const Board& board, Moves& move_list);
void generate_rook_moves(int Side, const Board& board, Moves& move_list);
void generate_queen_moves(int Side, const Board& board, Moves& move_list);

#endif // MOVEGEN_HPP
 