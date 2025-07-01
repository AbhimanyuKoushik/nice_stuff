#include "evaluate.hpp"
#include "position.hpp"
#include "movedef.hpp"
#include "attacks.hpp"
#include "types.hpp"
#include <iostream>
#include <climits>
#include <algorithm>

//----------------------------------------------------------------------
// Evaluate: material + positional, from side-to-move's perspective
//----------------------------------------------------------------------
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

    // Return relative to side-to-move
    return (pos.SideToMove == White ? score : -score);
}

//----------------------------------------------------------------------
// Minimax + alpha-beta
//----------------------------------------------------------------------

// Simple negamax: returns a score from the POV of pos.SideToMove
int negamax(Position pos, int depth, int alpha, int beta) {
    // 1) Leaf node: static evaluate
    if (depth == 0) {
        return Evaluate(pos);
    }

    // 2) Generate pseudo‑legal moves
    pos.generate_moves();
    // 3) No moves → checkmate or stalemate
    if (pos.move_list.empty()) {
        Color us   = pos.SideToMove;
        Color them = (us == White ? Black : White);
        int kingsq = get_ls1b_index(pos.bitboards[ us==White ? wK : bK ]);
        // checkmate = large negative, stalemate = 0
        return isSquareAttacked(kingsq, pos, them)
            ? -200000 + (depth)   // deeper mate is slightly better
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

// At the root, search and pick the best‐scoring move
Move Search_Position(Position pos, int depth) {
    std::cout << "Starting search at depth " << depth << "...\n";

    // 1) Generate root moves
    pos.generate_moves();
    if (pos.move_list.empty()) return 0;

    Move best_move   = pos.move_list[0];
    int  alpha       = -INT_MAX;
    int  beta        =  INT_MAX;
    int  best_score  = -INT_MAX;

    // 2) Negamax over each branch
    for (Move m : pos.move_list) {
        Position nxt = makemove(m, pos);
        // note the negation and swapped alpha/beta
        int score = -negamax(nxt, depth - 1, -beta, -alpha);

        if (score > best_score) {
            best_score = score;
            best_move  = m;
        }
        alpha = std::max(alpha, score);
        // optional: if (alpha >= beta) break;  // root‐level cut
    }

    std::cout << "Best move: "
              << square_to_coordinates[get_move_source(best_move)]
              << square_to_coordinates[get_move_target(best_move)]
              << " (score: " << best_score << ")\n";

    return best_move;
}


Move findbestmove(Position position) {
    // You can parameterize depth; here we hardcode 5
    return Search_Position(position, 5);
}
