#ifndef BASICEVAL_HPP
#define BASICEVAL_HPP

#include "../position.hpp"
#include "../movedef.hpp"
#include "../types.hpp"
#include "../bitboard.hpp"
#include "../movegen.hpp"
#include "../attacks.hpp"
#include <iostream>
#include <climits>
#include <vector>
#include <algorithm>
#include <atomic>
#include <thread>
#include <future>
#include <mutex>

// Forward declarations for optimization
struct SearchResult;
class ThreadedSearch;

// Material scoring constants (extern declarations)
extern const int material_score[PieceCount];
extern const int queenEndgameWeight;
extern const int rookEndgameWeight;
extern const int bishopEndgameWeight;
extern const int knightEndgameWeight;
extern const int endgameStartWeight;

// Piece-square tables (extern declarations)
extern const int PAWN_TABLE[64];
extern const int KNIGHT_TABLE[64];
extern const int BISHOP_TABLE[64];
extern const int ROOK_TABLE[64];
extern const int QUEEN_TABLE[64];
extern const int KING_TABLE_START[64];
extern const int KING_TABLE_END[64];
extern const int MirrorScore[64];

// Global search statistics
extern std::atomic<int> positions;

// Core evaluation functions
inline int Evaluate(const Position& pos);
int ForceKingToCorner(const Position& pos);

// Search functions with optimized signatures
int Quiescence(Position pos, int alpha, int beta, int depth);
int negamax(Position pos, int depth, int alpha, int beta);

// Main search interface
Move Search_Position(Position pos, int depth);
Move findbestmove(Position position);

// Search result structure for threading
struct SearchResult {
    Move move;
    int score;
    
    SearchResult();
    SearchResult(Move m, int s);
};

// Threaded search class for parallel processing
class ThreadedSearch {
private:
    std::atomic<Move> global_best_move;
    std::atomic<int> global_best_score;
    std::atomic<bool> search_stopped;
    std::atomic<bool> mate_found;
    
public:
    ThreadedSearch();
    
    SearchResult search_move(const Position& pos, Move move, int depth, int alpha, int beta);
    void update_best_move(const SearchResult& result);
    
    Move get_best_move() const;
    int get_best_score() const;
    void stop_search();
    bool is_mate_found() const;
};

// Optimization constants
static constexpr int MATE_SCORE = 200000;
static constexpr int MAX_QUIESCENCE_DEPTH = 6;

// Pre-computed lookup tables for performance
extern const int distanceToCorner[64];

// Utility functions for performance monitoring
namespace SearchStats {
    void reset_counters();
    int get_positions_searched();
    double get_search_time_ms();
}

// Template specializations for common position types (optional optimization)
template<Color SideToMove>
inline int EvaluateTemplated(const Position& pos);

// Compile-time optimizations
#ifdef __GNUC__
    #define LIKELY(x)   __builtin_expect(!!(x), 1)
    #define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
    #define LIKELY(x)   (x)
    #define UNLIKELY(x) (x)
#endif

// Memory prefetch hints for better cache performance
#ifdef __GNUC__
    #define PREFETCH(addr) __builtin_prefetch(addr, 0, 3)
#else
    #define PREFETCH(addr)
#endif

// Force inline for critical functions
#ifdef _MSC_VER
    #define FORCE_INLINE __forceinline
#elif defined(__GNUC__)
    #define FORCE_INLINE __attribute__((always_inline)) inline
#else
    #define FORCE_INLINE inline
#endif

// Hot path optimization markers
#define HOT_FUNCTION __attribute__((hot))
#define COLD_FUNCTION __attribute__((cold))

// Thread-local storage for performance-critical data
extern thread_local int thread_positions;
extern thread_local Position thread_temp_position;

#endif // BASICEVAL_HPP
