#ifndef PESTOEVAL_HPP
#define PESTOEVAL_HPP

#include "../position.hpp"
#include "../movedef.hpp"
#include "../types.hpp"
#include "../bitboard.hpp"
#include "../movegen.hpp"
#include "../attacks.hpp"
#include <iostream>
#include <climits>
#include <vector>
#include <algorithm>
#include <chrono>

// Piece type definitions for internal evaluation
#define PAWN   0
#define KNIGHT 1
#define BISHOP 2
#define ROOK   3
#define QUEEN  4
#define KING   5

// Color definitions for internal evaluation
#define WHITE  0
#define BLACK  1

// Piece encoding: 2*piece_type + color
#define WHITE_PAWN      (2*PAWN   + WHITE)
#define BLACK_PAWN      (2*PAWN   + BLACK)
#define WHITE_KNIGHT    (2*KNIGHT + WHITE)
#define BLACK_KNIGHT    (2*KNIGHT + BLACK)
#define WHITE_BISHOP    (2*BISHOP + WHITE)
#define BLACK_BISHOP    (2*BISHOP + BLACK)
#define WHITE_ROOK      (2*ROOK   + WHITE)
#define BLACK_ROOK      (2*ROOK   + BLACK)
#define WHITE_QUEEN     (2*QUEEN  + WHITE)
#define BLACK_QUEEN     (2*QUEEN  + BLACK)
#define WHITE_KING      (2*KING   + WHITE)
#define BLACK_KING      (2*KING   + BLACK)
#define EMPTY           (BLACK_KING  +  1)

// Utility macros
#define PCOLOR(p) ((p)&1)
#define FLIP(sq) ((sq)^56)
#define OTHER(side) ((side)^1)

// Material values
extern int mg_value[6];
extern int eg_value[6];

// Piece-square tables (declarations)
extern int mg_pawn_table[64];
extern int eg_pawn_table[64];
extern int mg_knight_table[64];
extern int eg_knight_table[64];
extern int mg_bishop_table[64];
extern int eg_bishop_table[64];
extern int mg_rook_table[64];
extern int eg_rook_table[64];
extern int mg_queen_table[64];
extern int eg_queen_table[64];
extern int mg_king_table[64];
extern int eg_king_table[64];

// Table pointers
extern int* mg_pesto_table[6];
extern int* eg_pesto_table[6];

// Game phase and combined tables
extern int gamephaseInc[12];
extern int mg_table[12][64];
extern int eg_table[12][64];

// Function declarations
void init_tables();
int Evaluate(const Position& pos);
int negamax(Position pos, int depth, int alpha, int beta);
Move Search_Position(const Position& pos, int depth);
Move findbestmove(Position position);

#endif // PESTOEVAL_HPP
