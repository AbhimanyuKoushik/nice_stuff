#include <iostream>
#include <bitset>
#include <string>

#include "bitboard.hpp"
#include "board.hpp"
#include "types.hpp"
#include "attacks.hpp"
#include "magic.hpp"
#include "nonmagic.hpp"

// Initialize all attack tables
void init_all() {
    init_sliders();      // Magic bitboards for sliding pieces
    init_nonsliders();   // Lookup tables for pawns, knights, kings
}

int main() {
    // 1. Initialize attack tables
    init_all();

    // 2. Test position: standard Kiwipete FEN
    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1";
    Board board = parsefen(fen);

    // 3. Print board arrangement
    std::cout << "Parsed Board:\n";
    board.print();

    // 4. Show occupancy bitboard
    U64 occ = board.occupancies[Both];

    board.generate_moves();



    return 0;
}
