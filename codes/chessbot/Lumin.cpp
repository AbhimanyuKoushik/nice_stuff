#include <iostream>
#include <bitset>
#include <string>

#include "bitboard.hpp"
#include "board.hpp"
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

    Board board;
    board.generate_moves();
    board.move_list.print_move_list();

    return 0;
}
