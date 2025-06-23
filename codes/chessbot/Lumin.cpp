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

// Helper to format a bitboard as 64-bit binary string
std::string bitboard_to_string(U64 b) {
    return std::bitset<64>(b).to_string();
}

int main() {
    // 1. Initialize attack tables
    init_all();

    // 2. Test position: standard Kiwipete FEN
    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    Board board = parsefen(fen);

    // 3. Print board arrangement
    std::cout << "Parsed Board:\n";
    board.print();

    // 4. Show occupancy bitboard
    U64 occ = board.occupancies[Both];
    std::cout << "Occupancy (both colors):\n";
    std::cout << bitboard_to_string(occ) << "\n\n";

    // 5. Test non-sliding attacks
    std::cout << "Knight attacks from g1 (g1 = square g1):\n";
    std::cout << bitboard_to_string(knight_attacks(g1)) << "\n\n";

    std::cout << "Pawn attacks for white from e4 (e4 = square e4):\n";
    std::cout << bitboard_to_string(pawn_attacks(White, e4)) << "\n\n";

    std::cout << "King attacks from d5 (d5 = square d5):\n";
    std::cout << bitboard_to_string(king_attacks(d5)) << "\n\n";

    // 6. Test sliding attacks
    std::cout << "Bishop attacks from d4:\n";
    std::cout << bitboard_to_string(bishop_attacks(d4, occ)) << "\n\n";

    std::cout << "Rook attacks from d4:\n";
    std::cout << bitboard_to_string(rook_attacks(d4, occ)) << "\n\n";

    std::cout << "Queen attacks from d4:\n";
    std::cout << bitboard_to_string(queen_attacks(d4, occ)) << "\n\n";

    // 7. Test isSquareAttacked
    std::cout << "Is e4 attacked by Black? ";
    std::cout << (isSquareAttacked(e4, board, Black) ? "Yes" : "No") << "\n";

    return 0;
}
