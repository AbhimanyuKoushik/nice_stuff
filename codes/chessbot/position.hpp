#ifndef POSITION_HPP
#define POSITION_HPP

#include <iostream>
#include <cstring>
#include <sstream>
#include "types.hpp"
#include "bitboard.hpp"
#include "nonmagic.hpp"
#include "movegen.hpp"
#include "movedef.hpp"

struct Position {
    uint8_t arrangement[64];
    uint8_t castling;    // 4 bits: white K/Q, black K/Q

    Color   SideToMove;
    bool    FiftyMove;
    U64 bitboards[12] = {0ULL};
    U64 occupancies[3] = {0ULL};
    uint8_t enpassant = no_sq;

    uint8_t HalfMovesMade;
    Moves move_list;

    Position() { init(); }

    void init();
    void update_arrangement();
    void update_bitboards();
    void compute_occupancies();
    void print() const;
    void makeMove(int from, int to);
    void emptyBoard();
    void generate_moves();
};

Position parsefen(const std::string &fen);

#endif // BOARD_HPP
