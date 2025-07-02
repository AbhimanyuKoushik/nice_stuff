#ifndef BASICEVAL_HPP
#define BASICEVAL_HPP

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

// Evaluation function
int Evaluate(const Position& pos);

// Helper functions
int Quiescence(Position pos, int alpha, int beta);
int negamax(Position pos, int depth, int alpha, int beta);
Move Search_Position(Position position, int depth);
Move findbestmove(Position position);

#endif
