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


// Initialize all attack tables
void init_all() {
    init_sliders();      // Magic bitboards for sliding pieces
    init_nonsliders();   // Lookup tables for pawns, knights, kings
}

int main() {
    std::cout << "=== " << NAME << " perft regression suite ===\n\n";

    // 1) Initialize all of your attack‐tables / bitboards
    init_all();
    std::cout << "Attack tables initialized.\n";
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

/*int main(){
    std::cout << "Initializing chess engine...\n" << std::endl;

    // 1. Initialize all attack tables
    init_all();
    std::cout << "Attack tables initialized successfully.\n" << std::endl;

    // 2. Initialize starting position
    Position position = parsefen("rnbqkbnr/pppppppp/8/8/8/3P4/PPP1PPPP/RNBQKBNR w KQkq - 0 1");

    // 3. Display the initial board
    std::cout << "Starting position with d2d3 move:" << std::endl;
    position.print();

    // 4. Generate and display moves
    position.generate_moves();
    std::cout << "Legal moves from starting position: " << position.move_list.moves.size() << std::endl;
    Position newpos;
    for(Move move : position.move_list.moves){
        newpos = makemove(move, position);
        newpos.print();
    }
}*/

