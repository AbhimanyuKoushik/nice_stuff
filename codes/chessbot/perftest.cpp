#include <iostream>
#include <chrono>
#include <thread>
#include <future>
#include <vector>
#include <atomic>
#include <iomanip>
#include <algorithm>

#include "perftest.hpp"
#include "position.hpp"
#include "movegen.hpp"
#include "movedef.hpp"

// Threshold for when to use multithreading
constexpr int THREADING_DEPTH_THRESHOLD = 4;
constexpr size_t MIN_MOVES_FOR_THREADING = 8;

// Internal optimized perft function with threading control
uint64_t perft_count_internal(int depth, Position position, int thread_depth = 0) {
    if (depth == 0) {
        return 1;
    }
    
    position.generate_moves();
    
    // Use single-threaded for shallow depths, few moves, or deep thread recursion
    if (depth < THREADING_DEPTH_THRESHOLD || 
        position.move_list.size() < MIN_MOVES_FOR_THREADING ||
        thread_depth > 2) {
        
        uint64_t nodes = 0;
        for (const Move& move : position.move_list) {
            Position newpos = makemove(move, position);
            if (newpos.SideToMove != position.SideToMove) {
                nodes += perft_count_internal(depth - 1, newpos, thread_depth + 1);
            }
        }
        return nodes;
    }
    
    // Multithreaded version
    const size_t num_moves = position.move_list.size();
    const unsigned int num_threads = std::min(
        std::thread::hardware_concurrency(),
        static_cast<unsigned int>(num_moves)
    );
    
    std::atomic<uint64_t> total_nodes{0};
    std::vector<std::future<void>> futures;
    futures.reserve(num_threads);
    
    // Calculate work distribution
    const size_t chunk_size = (num_moves + num_threads - 1) / num_threads;
    
    for (unsigned int t = 0; t < num_threads; ++t) {
        size_t start = t * chunk_size;
        size_t end = std::min(start + chunk_size, num_moves);
        
        if (start >= num_moves) break;
        
        futures.emplace_back(std::async(std::launch::async, [&, start, end, thread_depth]() {
            uint64_t local_nodes = 0;
            for (size_t i = start; i < end; ++i) {
                Position newpos = makemove(position.move_list[i], position);
                if (newpos.SideToMove != position.SideToMove) {
                    local_nodes += perft_count_internal(depth - 1, newpos, thread_depth + 1);
                }
            }
            total_nodes += local_nodes;
        }));
    }
    
    // Wait for all threads to complete
    for (auto& future : futures) {
        future.wait();
    }
    
    return total_nodes.load();
}

// Main perft_count function (maintains original signature)
uint64_t perft_count(int depth, Position position) {
    return perft_count_internal(depth, position, 0);
}

// Optimized perft_divide function (maintains original signature)
void perft_divide(int depth, Position position) {
    if (depth <= 0) return;
    
    position.generate_moves();
    
    std::cout << "\nPerft divide at depth " << depth << ":\n";
    std::cout << "----------------------------------------\n";
    
    const size_t num_moves = position.move_list.size();
    
    // For very few moves or shallow depth, use single-threaded
    if (num_moves < MIN_MOVES_FOR_THREADING || depth <= 2) {
        uint64_t total_nodes = 0;
        for (const Move& move : position.move_list) {
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
        return;
    }
    
    // Multithreaded version
    std::atomic<uint64_t> total_nodes{0};
    std::vector<std::pair<Move, uint64_t>> results(num_moves);
    std::vector<std::future<void>> futures;
    futures.reserve(num_moves);
    
    // Process each move in parallel
    for (size_t i = 0; i < num_moves; ++i) {
        futures.emplace_back(std::async(std::launch::async, [&, i]() {
            Move move = position.move_list[i];
            Position newpos = makemove(move, position);
            
            if (newpos.SideToMove != position.SideToMove) {
                uint64_t move_nodes = (depth == 1) ? 1 : perft_count(depth - 1, newpos);
                total_nodes += move_nodes;
                results[i] = {move, move_nodes};
            } else {
                results[i] = {move, 0}; // Invalid move
            }
        }));
    }
    
    // Wait for all tasks to complete
    for (auto& future : futures) {
        future.wait();
    }
    
    // Output results in original order
    for (const auto& [move, move_nodes] : results) {
        if (move_nodes > 0) {
            std::cout << square_to_coordinates[get_move_source(move)]
                     << square_to_coordinates[get_move_target(move)]
                     << ": " << move_nodes << std::endl;
        }
    }
    
    std::cout << "----------------------------------------\n";
    std::cout << "Total nodes: " << total_nodes.load() << std::endl;
}

// Enhanced perftcheck function with better performance reporting
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
        // 2) Time the perft_count with higher precision
        auto t0 = std::chrono::high_resolution_clock::now();
        uint64_t nodes = perft_count(depth, position);
        auto t1 = std::chrono::high_resolution_clock::now();

        auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
        double ms = microseconds / 1000.0;
        
        // Calculate nodes per second
        double nps = (microseconds > 0) ? (nodes * 1000000.0 / microseconds) : 0;
        
        std::cout << "Perft(" << depth << ") = " << nodes;
        if (nodes == expected) {
            std::cout << "  ✓ PASS";
        } else {
            std::cout << "  ✗ FAIL (expected " << expected << ")";
        }
        std::cout << "  [" << std::fixed << std::setprecision(2) << ms << " ms, " 
                  << std::scientific << std::setprecision(2) << nps << " nps]\n";
    }

    // 3) Show a per-move breakdown at depth 2
    std::cout << "\nPerft-divide at depth 5:\n";
    auto t0 = std::chrono::high_resolution_clock::now();
    perft_divide(5, position);
    auto t1 = std::chrono::high_resolution_clock::now();
    
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << "Perft-divide completed in " << ms << " ms\n";
}
