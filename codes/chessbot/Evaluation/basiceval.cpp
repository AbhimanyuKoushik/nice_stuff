#include "basiceval.hpp"
#include "../position.hpp"
#include "../movedef.hpp"
#include "../attacks.hpp"
#include "../types.hpp"
#include <iostream>
#include <climits>
#include <algorithm>
#include <chrono>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <future>

//----------------------------------------------------------------------
// Define all extern variables from header
//----------------------------------------------------------------------

// Distance to corner table - removed static to match extern declaration
const int distanceToCorner[64] = {
    0, 1, 2, 3, 3, 2, 1, 0,
    1, 2, 3, 4, 4, 3, 2, 1,
    2, 3, 4, 5, 5, 4, 3, 2,
    3, 4, 5, 6, 6, 5, 4, 3,
    3, 4, 5, 6, 6, 5, 4, 3,
    2, 3, 4, 5, 5, 4, 3, 2,
    1, 2, 3, 4, 4, 3, 2, 1,
    0, 1, 2, 3, 3, 2, 1, 0
};

// Global search statistics
std::atomic<int> positions{0};

// Thread-local storage
thread_local int thread_positions = 0;
thread_local Position thread_temp_position;

//----------------------------------------------------------------------
// Implementation of functions
//----------------------------------------------------------------------

int ForceKingToCorner(const Position& pos) {
    // Early exit optimization - cache piece counts
    static thread_local int cached_total = -1;
    static thread_local U64 cached_hash = 0;
    
    U64 current_hash = pos.bitboards[wQ] ^ pos.bitboards[bQ] ^ 
                      pos.bitboards[wR] ^ pos.bitboards[bR] ^
                      pos.bitboards[wB] ^ pos.bitboards[bB] ^
                      pos.bitboards[wN] ^ pos.bitboards[bN];
    
    if (cached_hash != current_hash) {
        cached_total = count_bits(pos.bitboards[wQ]) + count_bits(pos.bitboards[bQ]) +
                      count_bits(pos.bitboards[wR]) + count_bits(pos.bitboards[bR]) +
                      count_bits(pos.bitboards[wB]) + count_bits(pos.bitboards[bB]) +
                      count_bits(pos.bitboards[wN]) + count_bits(pos.bitboards[bN]);
        cached_hash = current_hash;
    }
    
    if (cached_total > 4) return 0;
    
    int whiteKingSquare = get_ls1b_index(pos.bitboards[wK]);
    int blackKingSquare = get_ls1b_index(pos.bitboards[bK]);
    
    if ((whiteKingSquare | blackKingSquare) == -1) return 0;
    
    // Optimized material calculation using bit operations
    int whiteMaterial = count_bits(pos.bitboards[wQ]) * 900 + 
                       count_bits(pos.bitboards[wR]) * 500 +
                       count_bits(pos.bitboards[wB]) * 330 + 
                       count_bits(pos.bitboards[wN]) * 320 +
                       count_bits(pos.bitboards[wP]) * 100;
                       
    int blackMaterial = count_bits(pos.bitboards[bQ]) * 900 + 
                       count_bits(pos.bitboards[bR]) * 500 +
                       count_bits(pos.bitboards[bB]) * 330 + 
                       count_bits(pos.bitboards[bN]) * 320 +
                       count_bits(pos.bitboards[bP]) * 100;
    
    // Fast Manhattan distance calculation
    int kingDistance = abs((whiteKingSquare >> 3) - (blackKingSquare >> 3)) + 
                      abs((whiteKingSquare & 7) - (blackKingSquare & 7));
    
    int score = 0;
    if (whiteMaterial > blackMaterial) {
        score += distanceToCorner[blackKingSquare] * 10;
        score += (14 - kingDistance) * 5;
        
        int blackRank = blackKingSquare >> 3;
        int blackFile = blackKingSquare & 7;
        
        if ((blackRank & 6) == 0 || (blackFile & 6) == 0) score += 20;
        if (((blackRank & 6) == 0) & ((blackFile & 6) == 0)) score += 50;
        
    } else if (blackMaterial > whiteMaterial) {
        score -= distanceToCorner[whiteKingSquare] * 10;
        score -= (14 - kingDistance) * 5;
        
        int whiteRank = whiteKingSquare >> 3;
        int whiteFile = whiteKingSquare & 7;
        
        if ((whiteRank & 6) == 0 || (whiteFile & 6) == 0) score -= 20;
        if (((whiteRank & 6) == 0) & ((whiteFile & 6) == 0)) score -= 50;
    }
    
    return (pos.SideToMove == White ? score : -score);
}

int Evaluate(const Position& pos) {
    int score = 0;
    
    // Game-over shortcuts
    if (!pos.bitboards[wK]) return INT_MIN + 1000; // white mated
    if (!pos.bitboards[bK]) return INT_MAX - 1000; // black mated

    int numQueens  = count_bits(pos.bitboards[wQ]) + count_bits(pos.bitboards[bQ]);
    int numRooks   = count_bits(pos.bitboards[wR]) + count_bits(pos.bitboards[bR]);
    int numBishops = count_bits(pos.bitboards[wB]) + count_bits(pos.bitboards[bB]);
    int numKnights = count_bits(pos.bitboards[wN]) + count_bits(pos.bitboards[bN]);
    int endgameWeightSum = numQueens * queenEndgameWeight + numRooks * rookEndgameWeight + numBishops * bishopEndgameWeight + numKnights * knightEndgameWeight;

    int endgameT = 1 - std::min(1, (int)(endgameWeightSum / (float)endgameStartWeight));
    // Evaluate all pieces
    for (int p = wP; p <= bK; ++p) {
        U64 bb = pos.bitboards[p];
        while (bb) {
            int sq = get_ls1b_index(bb);
            score += material_score[p];
            
            // Add positional bonuses
            switch (p) {
                case wP: score += PAWN_TABLE[sq]; break;
                case wN: score += KNIGHT_TABLE[sq]; break;
                case wB: score += BISHOP_TABLE[sq]; break;
                case wR: score += ROOK_TABLE[sq]; break;
                case wK: score += ((int)(KING_TABLE_START[sq] * (1 - endgameT)) + (int)(KING_TABLE_END[sq] * endgameT)); break;
                case wQ: score += QUEEN_TABLE[sq]; break;
                case bP: score -= PAWN_TABLE[MirrorScore[sq]]; break;
                case bN: score -= KNIGHT_TABLE[MirrorScore[sq]]; break;
                case bB: score -= BISHOP_TABLE[MirrorScore[sq]]; break;
                case bR: score -= ROOK_TABLE[MirrorScore[sq]]; break;
                case bK: score -= ((int)(KING_TABLE_START[MirrorScore[sq]] * (1 - endgameT)) + (int)(KING_TABLE_END[MirrorScore[sq]] * endgameT)); break;
                case bQ: score -= QUEEN_TABLE[MirrorScore[sq]]; break;
                default: break;
            }
            
            pop_bit(bb, sq);
        }
    }

    if (9 * numQueens + 5 * numRooks + 3 * numBishops + 2 * numKnights <= 15) {
        score += ForceKingToCorner(pos);
    }

    // Return relative to side-to-move
    return (pos.SideToMove == White ? score : -score);
}

// Removed default parameter from implementation (it's in header)
int Quiescence(Position pos, int alpha, int beta, int depth){
    // Stand Pat
    int best_value =  Evaluate(pos);
    if(depth > 8) {return best_value;} //Max_depth

    if( best_value >= beta )
        return best_value;
    if( best_value > alpha )
        alpha = best_value;

    pos.generate_moves();
    if (pos.move_list.empty()) {
        Color us   = pos.SideToMove;
        Color them = (us == White ? Black : White);
        int kingsq = get_ls1b_index(pos.bitboards[ us==White ? wK : bK ]);
        // checkmate = large negative, stalemate = 0
        return isSquareAttacked(kingsq, pos, them)
            ? -200000 - 10 * (depth)   // deeper mate is slightly better
            : 0;
    }

    for(Move move : pos.move_list){
        if(!get_move_capture_flag(move)) {continue;}
            int score = -Quiescence(makemove(move, pos), -beta, -alpha, depth + 1 );
            if( score >= beta )
                return score;
            if( score > best_value )
                best_value = score;
            if( score > alpha )
                alpha = score;
    }
    positions++;
    return best_value;
}

int negamax(Position pos, int depth, int alpha, int beta) {
    // 1) Leaf node: static evaluate
    if (depth == 0) {
        return Quiescence(pos, alpha, beta, 1);
    }

    // 2) Generate pseudo‑legal moves
    pos.generate_moves();
    pos.order_moves();
    // 3) No moves → checkmate or stalemate
    if (pos.move_list.empty()) {
        Color us   = pos.SideToMove;
        Color them = (us == White ? Black : White);
        int kingsq = get_ls1b_index(pos.bitboards[ us==White ? wK : bK ]);
        // checkmate = large negative, stalemate = 0
        return isSquareAttacked(kingsq, pos, them)
            ? -200000 - 10 * (depth)   // deeper mate is slightly better
            : 0;
    }

    // 4) Recurse
    int best = -INT_MAX;
    for (Move m : pos.move_list) {
        Position nxt = makemove(m, pos);
        int val = -negamax(nxt, depth - 1, -beta, -alpha);
        if(val >= beta){ return beta; }
        alpha = std::max(alpha, val);
    }
    return alpha;
}

Move Search_Position(Position pos, int depth) {
    positions.store(0, std::memory_order_relaxed);
    
    pos.generate_moves();
    pos.order_moves();
    if (pos.move_list.empty()) return 0;
    
    // Optimized thread count
    unsigned int num_threads = std::min(
        std::max(1u, std::thread::hardware_concurrency()),
        static_cast<unsigned int>(pos.move_list.size())
    );
    
    // Use single-threaded for small move counts
    if (pos.move_list.size() <= 4 || depth <= 2) {
        Move best_move = pos.move_list[0];
        int best_score = -INT_MAX;
        
        for (Move m : pos.move_list) {
            Position nxt = makemove(m, pos);
            int score = -negamax(nxt, depth - 1, -INT_MAX, INT_MAX);
            if (score > best_score) {
                best_score = score;
                best_move = m;
            }
        }
        return best_move;
    }
    
    ThreadedSearch searcher;
    
    // Pre-allocate move batches
    std::vector<std::vector<Move>> move_batches(num_threads);
    for (size_t i = 0; i < pos.move_list.size(); ++i) {
        move_batches[i % num_threads].push_back(pos.move_list[i]);
    }
    
    std::vector<std::future<SearchResult>> futures;
    futures.reserve(num_threads);
    
    // Launch threads with pre-computed batches
    for (unsigned int t = 0; t < num_threads; ++t) {
        if (move_batches[t].empty()) continue;
        
        futures.push_back(std::async(std::launch::async, 
            [&searcher, pos, batch = std::move(move_batches[t]), depth]() {
                SearchResult best_in_batch;
                
                for (Move move : batch) {
                    if (searcher.is_mate_found()) break;
                    
                    SearchResult result = searcher.search_move(pos, move, depth, -INT_MAX, INT_MAX);
                    
                    if (result.score > best_in_batch.score) {
                        best_in_batch = result;
                    }
                    
                    searcher.update_best_move(result);
                }
                
                return best_in_batch;
            }));
    }
    
    // Collect results
    for (auto& future : futures) {
        future.get();
    }
    
    return searcher.get_best_move();
}

Move findbestmove(Position position) {
    static int cached_num_pieces = -1;
    static int cached_search_depth = 4;
    static U64 position_hash = 0;

    // Simple position hash for caching
    U64 current_hash = 0;
    for (int piece = wP; piece <= bK; ++piece) {
        current_hash ^= position.bitboards[piece];
    }

    // Only recalculate if position changed significantly
    if (position_hash != current_hash) {
        int num_pieces = 0;
        int total_material = 0;
        
        // Count pieces and calculate total material value
        for (int piece = wN; piece <= bK; ++piece) {
            if(piece == bP) {continue;}
            int piece_count = count_bits(position.bitboards[piece]);
            num_pieces += piece_count;
            
            // Add material value (excluding kings)
            if (piece != wK && piece != bK) {
                total_material += piece_count * abs(material_score[piece]);
            }
        }
        
        // Count specific high-value pieces
        int queens = count_bits(position.bitboards[wQ]) + count_bits(position.bitboards[bQ]);
        int rooks = count_bits(position.bitboards[wR]) + count_bits(position.bitboards[bR]);
        int minors = count_bits(position.bitboards[wB]) + count_bits(position.bitboards[bB]) + 
                    count_bits(position.bitboards[wN]) + count_bits(position.bitboards[bN]);
        int pawns = count_bits(position.bitboards[wP]) + count_bits(position.bitboards[bP]);
        
        // Material-based adaptive depth
        if (total_material >= 6000) {
            // Opening/Early middlegame (high material)
            cached_search_depth = 4;
        }
        else if (total_material >= 4000) {
            // Middlegame (medium-high material)
            cached_search_depth = 5;
        }
        else if (total_material >= 2500) {
            // Late middlegame (medium material)
            cached_search_depth = 6;
        }
        else if (queens >= 1) {
            // Queen endgame - keep moderate depth due to complexity
            cached_search_depth = 6;
        }
        else if (rooks >= 1) {
            // Rook endgame - can search deeper but not too deep
            cached_search_depth = 7;
        }
        else if (minors >= 2) {
            // Multiple minor pieces - moderate depth
            cached_search_depth = 6;
        }
        else if (minors >= 1 && total_material >= 400) {
            // Single minor piece endgame - deeper search
            cached_search_depth = 8;
        }
        else if (num_pieces >= 6) {
            // Pawn endgame or very simple endgame
            cached_search_depth = 9;
        }
        else if(pawns >= 8){
            cached_search_depth = 10;
        }
        else {
            // King and pawn endgame - deepest search
            cached_search_depth = 11;
        }
        
        cached_num_pieces = num_pieces;
        position_hash = current_hash;
    }

    int search_depth = cached_search_depth;
    std::cout << "Searching at depth: " << search_depth << "\n";
    Move best_move;
    
    auto t0 = std::chrono::high_resolution_clock::now();
    
    // Use the single Search_Position function which already handles threading internally
    best_move = Search_Position(position, search_depth);
    
    auto t1 = std::chrono::high_resolution_clock::now();

    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    double ms = microseconds / 1000.0;
    
    // Only output timing info, not during search
    std::cout << "Time: " << ms << "ms, Positions: " << positions.load() << std::endl;
    
    return best_move;
}

// Removed struct and class definitions (they're in header)

// Constructor implementations
SearchResult::SearchResult() : move(0), score(-INT_MAX) {}
SearchResult::SearchResult(Move m, int s) : move(m), score(s) {}

// ThreadedSearch constructor
ThreadedSearch::ThreadedSearch() : 
    global_best_move{0}, 
    global_best_score{-INT_MAX}, 
    search_stopped{false}, 
    mate_found{false} {}

SearchResult ThreadedSearch::search_move(const Position& pos, Move move, int depth, int alpha, int beta) {
    if (search_stopped.load(std::memory_order_relaxed)) {
        return SearchResult(move, -INT_MAX);
    }
    
    Position nxt = makemove(move, pos);
    int score = -negamax(nxt, depth - 1, -beta, -alpha);
    
    // Early termination for mate
    if (score >= MATE_SCORE - 1000) {
        mate_found.store(true, std::memory_order_relaxed);
        search_stopped.store(true, std::memory_order_relaxed);
    }
    
    return SearchResult(move, score);
}

void ThreadedSearch::update_best_move(const SearchResult& result) {
    int current_best = global_best_score.load(std::memory_order_relaxed);
    while (result.score > current_best && 
           !global_best_score.compare_exchange_weak(current_best, result.score, std::memory_order_relaxed)) {
        // Retry if another thread updated the score
    }
    if (result.score > current_best) {
        global_best_move.store(result.move, std::memory_order_relaxed);
    }
}

Move ThreadedSearch::get_best_move() const { 
    return global_best_move.load(std::memory_order_relaxed); 
}

int ThreadedSearch::get_best_score() const { 
    return global_best_score.load(std::memory_order_relaxed); 
}

void ThreadedSearch::stop_search() { 
    search_stopped.store(true, std::memory_order_relaxed); 
}

bool ThreadedSearch::is_mate_found() const { 
    return mate_found.load(std::memory_order_relaxed); 
}

// SearchStats namespace implementation
namespace SearchStats {
    void reset_counters() {
        positions.store(0, std::memory_order_relaxed);
        thread_positions = 0;
    }
    
    int get_positions_searched() {
        return positions.load(std::memory_order_relaxed);
    }
    
    double get_search_time_ms() {
        // This would need to be implemented with timing logic
        return 0.0;
    }
}
