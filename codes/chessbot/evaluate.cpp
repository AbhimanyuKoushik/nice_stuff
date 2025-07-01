#include "evaluate.hpp"
#include "position.hpp"
#include "movedef.hpp"
#include "attacks.hpp"
#include <iostream>
#include <climits>
#include <algorithm>

//----------------------------------------------------------------------
// Material values for each Piece enum (wP…bK, Em last)
//----------------------------------------------------------------------
static const int material_score[PieceCount] = {
    /* wP */ 100,
    /* wN */ 320,
    /* wB */ 330,
    /* wR */ 500,
    /* wQ */ 900,
    /* wK */ 20000,
    /* bP */ -100,
    /* bN */ -320,
    /* bB */ -330,
    /* bR */ -500,
    /* bQ */ -900,
    /* bK */ -20000,
    /* Em */ 0
};

//----------------------------------------------------------------------
// Piece-square tables (white's perspective; black will be flipped)
//----------------------------------------------------------------------
const int PAWN_TABLE[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    90, 90, 90, 90, 90, 90, 90, 90,
    30, 30, 30, 40, 40, 30, 30, 30,
    20, 20, 20, 30, 30, 30, 20, 20,
    10, 10, 10, 20, 20, 10, 10, 10,
    5, 5, 10, 20, 20, 5, 5, 5,
    0, 0, 0, 5, 5, 0, 0, 0,
    0, 0, 0, -10, -10, 0, 0, 0
};

const int KNIGHT_TABLE[64] = {
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 10, 10, 0, 0, -5,
    -5, 5, 20, 20, 20, 20, 5, -5,
    -5, 10, 20, 30, 30, 20, 10, -5,
    -5, 10, 20, 30, 30, 20, 10, -5,
    -5, 5, 20, 10, 10, 20, 5, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, -10, 0, 0, 0, 0, -10, -5
};

const int BISHOP_TABLE[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 10, 10, 0, 0, 0,
    0, 0, 10, 20, 20, 10, 0, 0,
    0, 0, 10, 20, 20, 10, 0, 0,
    0, 10, 0, 0, 0, 0, 10, 0,
    0, 30, 0, 0, 0, 0, 30, 0,
    0, 0, -10, 0, 0, -10, 0, 0
};

const int ROOK_TABLE[64] = {
    50, 50, 50, 50, 50, 50, 50, 50,
    50, 50, 50, 50, 50, 50, 50, 50,
    0, 0, 10, 20, 20, 10, 0, 0,
    0, 0, 10, 20, 20, 10, 0, 0,
    0, 0, 10, 20, 20, 10, 0, 0,
    0, 0, 10, 20, 20, 10, 0, 0,
    0, 0, 10, 20, 20, 10, 0, 0,
    0, 0, 0, 20, 20, 0, 0, 0
};

const int KING_TABLE[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 5, 5, 5, 5, 0, 0,
    0, 5, 5, 10, 10, 5, 5, 0,
    0, 5, 10, 20, 20, 10, 5, 0,
    0, 5, 10, 20, 20, 10, 5, 0,
    0, 0, 5, 10, 10, 5, 0, 0,
    0, 5, 5, -5, -5, 0, 5, 0,
    0, 0, 5, 0, -15, 0, 10, 0
};

// Mirror positional score tables for opposite side
const int MirrorScore[64] = {
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8
};

//----------------------------------------------------------------------
// Evaluate: material + positional, from side-to-move's perspective
//----------------------------------------------------------------------
int Evaluate(const Position& pos) {
    int score = 0;
    
    // Game-over shortcuts
    if (!pos.bitboards[wK]) return INT_MIN + 1000; // white mated
    if (!pos.bitboards[bK]) return INT_MAX - 1000; // black mated

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
                case wK: score += KING_TABLE[sq]; break;
                case bP: score -= PAWN_TABLE[MirrorScore[sq]]; break;
                case bN: score -= KNIGHT_TABLE[MirrorScore[sq]]; break;
                case bB: score -= BISHOP_TABLE[MirrorScore[sq]]; break;
                case bR: score -= ROOK_TABLE[MirrorScore[sq]]; break;
                case bK: score -= KING_TABLE[MirrorScore[sq]]; break;
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
Move best_move = 0;
int ply = 0;

int negamax(Position pos, int depth, int alpha, int beta) {
    // At leaf or zero-depth: return static evaluation
    if (depth == 0) {
        return Evaluate(pos);
    }
    
    // Increase ply (for root detection)
    ++ply;
    
    // Generate all legal moves for this position
    pos.generate_moves();
    if (pos.move_list.empty()) {
        // No moves: checkmate or stalemate
        Color us = pos.SideToMove;
        Color them = (us == White ? Black : White);
        int ksq = get_ls1b_index(pos.bitboards[us == White ? wK : bK]);
        bool in_check = isSquareAttacked(ksq, pos, them);
        
        --ply;
        return in_check 
            ? (-20000 + ply) // mate score (closer mate is worse)
            : 0; // stalemate draw
    }

    best_move = pos.move_list[0];
    Move local_best = 0;
    
    // Loop over each move
    for (Move m : pos.move_list) {
        // Apply move
        Position nxt = makemove(m, pos);
        
        // Recurse, flipping sign and alpha/beta
        int score = -negamax(nxt, depth - 1, -beta, -alpha);
        
        // Alpha-beta cutoff
        if (score >= beta) {
            --ply;
            return beta; // fail-hard
        }
        
        if (score > alpha) {
            alpha = score;
            local_best = m;
        }
    }

    // On the root ply, record the best move
    if (ply == 1 && local_best != 0) {
        best_move = local_best;
    }

    --ply;
    return alpha;
}

void Search_Position(Position position, int depth) {
    best_move = 0; // Reset best move
    ply = 0; // Reset ply counter
    
    std::cout << "Starting search at depth " << depth << "..." << std::endl;
    
    int score = negamax(position, depth, INT_MIN, INT_MAX);
    
    if (best_move != 0) {
        std::cout << "Best move: " 
                  << square_to_coordinates[get_move_source(best_move)] 
                  << square_to_coordinates[get_move_target(best_move)] 
                  << " (score: " << score << ")" << std::endl;
    } else {
        std::cout << "No best move found in search!" << std::endl;
    }
}

Move findbestmove(Position position) {
    Search_Position(position, 5); // Search 4 plies deep
    return best_move;
}
