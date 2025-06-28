#include <iostream>
#include <chrono>

#include "perftest.hpp"
#include "position.hpp"
#include "movegen.hpp"


// Perft function declarations
uint64_t perft_count(int depth, Position position) {
    if (depth == 0) {
        return 1;
    }
    
    position.generate_moves();
    uint64_t nodes = 0;
    
    for (Move move : position.move_list.moves) {
        Position newpos = makemove(move, position);
        
        // Only count if the move was legal
        if (newpos.SideToMove != position.SideToMove) {
            nodes += perft_count(depth - 1, newpos);
        }
    }
    
    return nodes;
}

void perft_divide(int depth, Position position) {
    if (depth <= 0) return;
    
    position.generate_moves();
    uint64_t total_nodes = 0;
    
    std::cout << "\nPerft divide at depth " << depth << ":\n";
    std::cout << "----------------------------------------\n";
    
    for (Move move : position.move_list.moves) {
        Position newpos = makemove(move, position);
        
        if (newpos.SideToMove != position.SideToMove) {
            uint64_t move_nodes = (depth == 1) ? 1 : perft_count(depth - 1, newpos);
            total_nodes += move_nodes;
            
            std::cout << square_to_coordinates[get_move_source(move)]
                     << square_to_coordinates[get_move_target(move)]
                     << ": " << move_nodes << std::endl;
        }
    }
    
    std::cout << "----------------------------------------\n";
    std::cout << "Total nodes: " << total_nodes << std::endl;
}

void perftcheck(const std::string& fen,
                const std::vector<std::pair<int, uint64_t>>& expected_results)
{
    // 1) Set up the position from FEN (or use the default ctor for "startpos")
    Position position;
    if (fen != "startpos") {
        position = parsefen(fen);
    }

    std::cout << "\n=== PERFT TESTS for `" << fen << "` ===\n";
    for (auto [depth, expected] : expected_results) {
        // 2) Time the perft_count
        auto t0 = std::chrono::high_resolution_clock::now();
        uint64_t nodes = perft_count(depth, position);
        auto t1 = std::chrono::high_resolution_clock::now();

        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
        std::cout << "Perft(" << depth << ") = " << nodes;
        if (nodes == expected) {
            std::cout << "  ✓ PASS";
        } else {
            std::cout << "  ✗ FAIL (expected " << expected << ")";
        }
        std::cout << "  [" << ms << " ms]\n";
    }

    // 3) Show a per-move breakdown at depth 2
    std::cout << "\nPerft-divide at depth 5:\n";
    //perft_divide(5, position);
}