#include <iostream>
#include <bitset>
#include <string>

#include "bitboard.hpp"
#include "position.hpp"
#include "types.hpp"
#include "attacks.hpp"
#include "magic.hpp"
#include "nonmagic.hpp"
#include "movegen.hpp"
#include "movedef.hpp"

// Initialize all attack tables
void init_all() {
    init_sliders();      // Magic bitboards for sliding pieces
    init_nonsliders();   // Lookup tables for pawns, knights, kings
}

int main() {
    // 1. Initialize attack tables
    init_all();

    Position position = parsefen("rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1");
    position.print();
    position.generate_moves();
    position.move_list.print_move_list();

    return 0;
}
