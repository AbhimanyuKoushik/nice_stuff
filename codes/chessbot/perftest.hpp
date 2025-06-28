#ifndef PERFTEST_HPP
#define PERFTEST_HPP

#include <iostream>
#include "position.hpp"

uint64_t perft_count(int depth, Position position);
void perft_divide(int depth, Position position);
void perftcheck(const std::string& fen,
                const std::vector<std::pair<int, uint64_t>>& expected_results);

#endif