#ifndef BOARD_HPP
#define BOARD_HPP

#include <iostream>
#include <cstring>
#include <sstream>
#include "types.hpp"
#include "bitboard.hpp"
#include "nonmagic.hpp"
#include "movegen.hpp"

struct Board {
    uint8_t arrangement[64];
    uint8_t castling;    // 4 bits: white K/Q, black K/Q

    Color   SideToMove;
    bool    FiftyMove;
    U64 bitboards[12] = {0ULL};
    U64 occupancies[3] = {0ULL};
    uint8_t enpassant = no_sq;

    uint8_t HalfMovesMade;

    Board() { init(); }

    void init();
    void update_arrangement();
    void update_bitboards();
    void compute_occupancies();
    void print() const;
    void makeMove(int from, int to);
    void emptyBoard();
    void generate_moves();
};

Board parsefen(const std::string &fen);

#endif // BOARD_HPP
