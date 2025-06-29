#ifndef BITBOARD_HPP
#define BITBOARD_HPP 

#include <cstdint>
#include <cstdio>
#include <vector>
#include "types.hpp"

// Bitboard type
using Bitboard = U64;

// Bitboard operations
#define set_bit(bitboard, square) (bitboard = bitboard | (1ULL << square))
#define get_bit(bitboard, square) (bitboard & (1ULL << square))
#define pop_bit(bitboard, square) (get_bit(bitboard, square) ? bitboard ^= (1ULL << square) : 0)

// Print a bitboard as an 8x8 grid (1 for occupied, . for empty)
void print_bitboard(U64 bitboard);

// count bits within a bitboard (Brian Kernighan's way)
int count_bits(U64 bitboard);

// get least significant 1st bit index
int get_ls1b_index(U64 bitboard);

#endif