#include <iostream>
#include <chrono>
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
#include "perftest.hpp"
#include "uci.hpp"
#include "game.hpp"


// Initialize all attack tables
void init_all() {
    init_sliders();      // Magic bitboards for sliding pieces
    init_nonsliders();   // Lookup tables for pawns, knights, kings
    std::cout << "Attack tables initialized.\n";
}

int main() {
    std::cout << "=== " << NAME << " perft regression suite ===\n\n";

    // 1) Initialize all of your attack‐tables / bitboards
    //init_all();

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Initialize all attack tables, bitboards, magics, etc.
    init_all();

    std::cout << "Welcome to the Chess Engine!\n";
    std::cout << "Choose mode:\n";
    std::cout << "1. Bot vs Human\n";
    std::cout << "2. Bot vs Bot\n";
    std::cout << "Enter your choice (1 or 2): ";

    int mode = 0;
    std::cin >> mode;
    std::cin.ignore(); // Clear newline

    // Set up initial position (FEN can be customized)
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    Position start = parsefen(fen);

    Game game;
    game.currposition = start;

    if (mode == 1) {
        std::cout << "Choose your side (w for White, b for Black): ";
        char side;
        std::cin >> side;
        Color userColor = (side == 'w' || side == 'W') ? White : Black;
        game.Startplaying(userColor, false);
    } else if (mode == 2) {
        std::cout << "Bot vs Bot mode selected. Press Enter to start.\n";
        std::cin.ignore();
        game.Startplaying(White, true); // Bot vs Bot, White starts
    } else {
        std::cout << "Invalid mode selected.\n";
        return 1;
    }

    std::cout << "Thank you for playing!\n";
    // 3) “Kiwipete” test position
    //    r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1
    std::string pos1 = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";
    // Known perft counts for Kiwipete:
    std::vector<std::pair<int, uint64_t>> pos1_results = {
        {1,     48ULL},
        {2,   2039ULL},
        {3,  97862ULL},
        {4, 4085603ULL},
        {5, 193690690ULL}
    };
    perftcheck(pos1, pos1_results);

    std::string pos2 = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1";
    // Known perft counts for Kiwipete:
    std::vector<std::pair<int, uint64_t>> pos2_results = {
        {1,     14ULL},
        {2,    191ULL},
        {3,   2812ULL},
        {4,  43238ULL},
        {5, 674624ULL},
        {6, 11030083ULL},
    };
    perftcheck(pos2, pos2_results);

    std::string pos3 = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
    // Known perft counts for Kiwipete:
    std::vector<std::pair<int, uint64_t>> pos3_results = {
        {1,       6ULL},
        {2,     264ULL},
        {3,    9467ULL},
        {4,  422333ULL}
    };
    perftcheck(pos3, pos3_results);

    std::string pos4 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    Position position4 = parsefen(pos4);
    position4.print();
    // Known perft counts for Kiwipete:
    std::vector<std::pair<int, uint64_t>> pos4_results = {
        {1,       20ULL},
        {2,      400ULL},
        {3,     8902ULL},
        {4,   197281ULL},
        {5,  4865609ULL},
        {6, 119060324ULL}
    };
    perftcheck(pos4, pos4_results);
    

    std::cout << "\n=== ALL TESTS COMPLETE ===\n";
    return 0;
}
