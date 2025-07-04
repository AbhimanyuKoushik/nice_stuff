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

// Use fixed-size arrays for better performance
static constexpr int MAX_MOVES = 256;
static std::pair<Move, int> previous_move_scores[MAX_MOVES];
static int previous_move_count = 0;
static std::mutex move_ordering_mutex;

// Global time control variables
static std::atomic<bool> time_up{false};
static std::chrono::high_resolution_clock::time_point search_start_time;
static std::chrono::milliseconds search_time_limit{1000};

// Helper function to check if time is up
inline bool is_time_up() {
    auto current_time = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - search_start_time);
    return elapsed >= search_time_limit;
}

// Custom comparator for move ordering
struct MoveComparator {
    bool operator()(const std::pair<Move, int>& a, const std::pair<Move, int>& b) const {
        return a.second > b.second; // Higher scores first
    }
};

void order_moves_by_previous_scores(Position& pos) {
    if (previous_move_count == 0) {
        pos.order_moves();
        return;
    }
    
    Move ordered_moves[MAX_MOVES];
    Move unscored_moves[MAX_MOVES];
    int ordered_count = 0;
    int unscored_count = 0;
    
    // First, add moves that were scored in previous iteration (in order)
    {
        std::lock_guard<std::mutex> lock(move_ordering_mutex);
        for (int i = 0; i < previous_move_count; ++i) {
            Move scored_move = previous_move_scores[i].first;
            
            for (size_t j = 0; j < pos.move_list.size(); ++j) {
                if (pos.move_list[j] == scored_move) {
                    ordered_moves[ordered_count++] = scored_move;
                    break;
                }
            }
        }
    }
    
    // Add unscored moves
    for (size_t i = 0; i < pos.move_list.size(); ++i) {
        Move move = pos.move_list[i];
        bool found = false;
        
        for (int j = 0; j < ordered_count; ++j) {
            if (ordered_moves[j] == move) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            unscored_moves[unscored_count++] = move;
        }
    }
    
    // Combine moves
    pos.move_list.clear();
    pos.move_list.reserve(ordered_count + unscored_count);
    
    for (int i = 0; i < ordered_count; ++i) {
        pos.move_list.push_back(ordered_moves[i]);
    }
    for (int i = 0; i < unscored_count; ++i) {
        pos.move_list.push_back(unscored_moves[i]);
    }
}

// Modified negamax with time checking
int negamax(const Position& pos, int depth, int alpha, int beta) {
    // Check time every few nodes to avoid overhead
    static thread_local int node_count = 0;
    if (++node_count % 1000 == 0 && is_time_up()) {
        return alpha; // Return current alpha when time is up
    }
    
    if (depth == 0) {
        return Quiescence(pos, alpha, beta, 1);
    }

    Position search_pos = pos;
    search_pos.generate_moves();
    search_pos.order_moves();
    
    if (search_pos.move_list.empty()) {
        Color us = search_pos.SideToMove;
        int kingsq = get_ls1b_index(search_pos.bitboards[us == White ? wK : bK]);
        
        return isSquareAttacked(kingsq, search_pos, us ^ 1)
            ? (-MATE_SCORE - depth)
            : 0;
    }

    int best = -INT_MAX;
    for (Move m : search_pos.move_list) {
        if (is_time_up()) break; // Stop search if time is up
        
        Position nxt = makemove(m, search_pos);
        int val = -negamax(nxt, depth - 1, -beta, -alpha);
        
        if (val >= beta) return beta;
        best = std::max(best, val);
        alpha = std::max(alpha, val);
    }
    return best;
}

Move Search_Position(Position pos, int max_depth) {
    positions.store(0, std::memory_order_relaxed);
    
    pos.generate_moves();
    
    Move best_move = pos.move_list[0];
    int best_score = -INT_MAX;
    
    // Initialize time control
    search_start_time = std::chrono::high_resolution_clock::now();
    time_up.store(false, std::memory_order_relaxed);
    
    std::cout << "Starting 1-second search..." << std::endl;
    
    // Iterative deepening with proper time control
    for (int current_depth = 1; current_depth <= max_depth; ++current_depth) {
        // Check time before starting new depth
        if (is_time_up()) {
            std::cout << "Time limit reached before depth " << current_depth << std::endl;
            break;
        }
        
        std::cout << "Searching depth " << current_depth << "..." << std::endl;
        
        // Order moves based on previous iteration scores
        order_moves_by_previous_scores(pos);
        
        Move iteration_best_move = 0;
        int iteration_best_score = -INT_MAX;
        std::pair<Move, int> current_move_scores[MAX_MOVES];
        int current_score_count = 0;
        bool depth_completed = true;
        
        // Search all moves at current depth
        for (size_t i = 0; i < pos.move_list.size(); ++i) {
            // Time check before each move
            if (is_time_up()) {
                std::cout << "Time limit reached during depth " << current_depth 
                         << " after " << i << " moves" << std::endl;
                depth_completed = false;
                break;
            }
            
            Move m = pos.move_list[i];
            Position nxt = makemove(m, pos);
            int score = -negamax(nxt, current_depth - 1, -INT_MAX, INT_MAX);
            
            // Only record score if we didn't run out of time
            if (!is_time_up()) {
                current_move_scores[current_score_count++] = {m, score};
                
                if (score > iteration_best_score) {
                    iteration_best_score = score;
                    iteration_best_move = m;
                }
                
                // Early termination for mate
                if (score >= MATE_SCORE - 1000) {
                    std::cout << "Mate found at depth " << current_depth << "!" << std::endl;
                    best_move = m;
                    best_score = score;
                    goto search_complete;
                }
            } else {
                depth_completed = false;
                break;
            }
        }
        
        // Only update best move if we completed the depth
        if (depth_completed && iteration_best_move != 0) {
            best_move = iteration_best_move;
            best_score = iteration_best_score;
            
            auto current_time = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - search_start_time);
            
            std::cout << "Depth " << current_depth << " completed in " << elapsed.count() 
                      << "ms, best: " << square_to_coordinates[get_move_source(best_move)]
                      << square_to_coordinates[get_move_target(best_move)]
                      << " (score: " << best_score << ")" << std::endl;
            
            // Store move scores for next iteration ordering
            {
                std::lock_guard<std::mutex> lock(move_ordering_mutex);
                previous_move_count = std::min(current_score_count, MAX_MOVES);
                for (int i = 0; i < previous_move_count; ++i) {
                    previous_move_scores[i] = current_move_scores[i];
                }
                std::sort(previous_move_scores, previous_move_scores + previous_move_count, MoveComparator());
            }
        } else {
            std::cout << "Depth " << current_depth << " incomplete due to time limit" << std::endl;
            break;
        }
        
        // Final time check after completing depth
        if (is_time_up()) {
            std::cout << "Time limit reached after completing depth " << current_depth << std::endl;
            break;
        }
    }
    
    search_complete:
    auto final_time = std::chrono::high_resolution_clock::now();
    auto total_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(final_time - search_start_time);
    std::cout << "Search completed in " << total_elapsed.count() << "ms, positions: " 
              << positions.load() << std::endl;
    
    return best_move;
}

// Updated findbestmove function
Move findbestmove(Position position) {
    // Set search time limit (can be adjusted)
    search_time_limit = std::chrono::milliseconds(2500); // 2.5 second
    
    // Use high max depth since time will limit the search
    Move best_move = Search_Position(position, 20);
    
    return best_move;
}


/*Move findbestmove(Position position) {
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
            cached_search_depth = 12;
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
}*/

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
