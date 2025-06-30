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
    uint8_t castling;    // 4 bits: white K/Q, black K/Q

    Color   SideToMove;
    bool    FiftyMove;
    U64 bitboards[12] = {0ULL};
    U64 occupancies[3] = {0ULL};
    uint8_t enpassant = no_sq;

    Moves move_list;

    Position() { init(); }

    void init();
    void compute_occupancies();
    void print() const;
    void emptyBoard();
    void generate_moves();
    std::string get_fen() const;
    bool operator==(const Position& o) const {
        return
            SideToMove == o.SideToMove &&
            castling    == o.castling    &&
            enpassant   == o.enpassant   &&

            // compare bitboards array
            std::equal(std::begin(bitboards),
                       std::end(bitboards),
                       std::begin(o.bitboards)) &&

            // compare occupancies array
            std::equal(std::begin(occupancies),
                       std::end(occupancies),
                       std::begin(o.occupancies));

    }

    // Inequality operator can simply be the negation
    bool operator!=(const Position& o) const {
        return !(*this == o);
    }
};

Position parsefen(const std::string &fen);
Position makemove(Move move, Position position);
void FilterLegalMoves(Position& position);

#endif // BOARD_HPP
