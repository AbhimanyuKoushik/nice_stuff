#include "position.hpp"
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <unordered_map>
#include "game.hpp"
#include "attacks.hpp"
#include "bitboard.hpp"
#include "uci.hpp"
#include "Evaluation/basiceval.hpp"
#include "movedef.hpp"

//----------------------------------------------------------------------
// Insufficient material draw test (CORRECTED)
//----------------------------------------------------------------------

bool Game::isDrawByInsufficientMaterial(const Position &position) {
    int wMinor = 0, bMinor = 0;
    int wBish = 0, bBish = 0;

    // Count pieces more safely
    wMinor += count_bits(position.bitboards[wN]);
    bMinor += count_bits(position.bitboards[bN]);
    wBish += count_bits(position.bitboards[wB]);
    bBish += count_bits(position.bitboards[bB]);

    // Check for material pieces (pawns, rooks, queens)
    if (position.bitboards[wP] || position.bitboards[bP] ||
        position.bitboards[wR] || position.bitboards[bR] ||
        position.bitboards[wQ] || position.bitboards[bQ]) {
        return false;
    }

    // King vs King
    if (wMinor == 0 && bMinor == 0 && wBish == 0 && bBish == 0) return true;
    
    // King + Knight vs King
    if ((wMinor == 1 && wBish == 0 && bMinor == 0 && bBish == 0) ||
        (bMinor == 1 && bBish == 0 && wMinor == 0 && wBish == 0)) return true;
    
    // King + Bishop vs King
    if (wMinor == 0 && bMinor == 0) {
        if ((wBish == 1 && bBish == 0) || (bBish == 1 && wBish == 0)) return true;
        
        // Both sides have one bishop - check if same color squares
        if (wBish == 1 && bBish == 1) {
            int wSq = get_ls1b_index(position.bitboards[wB]);
            int bSq = get_ls1b_index(position.bitboards[bB]);
            bool wCol = ((wSq / 8 + wSq % 8) & 1) == 0;
            bool bCol = ((bSq / 8 + bSq % 8) & 1) == 0;
            return (wCol == bCol);
        }
    }
    
    return false;
}

//----------------------------------------------------------------------
// Game end test (CORRECTED)
//----------------------------------------------------------------------

bool Game::isGameEnded(const Position &position) {
    // 1) Insufficient material
    if (isDrawByInsufficientMaterial(position)) return true;
    
    // 2) No legal moves available
    if (position.move_list.empty()) return true;
    
    return false;
}

//----------------------------------------------------------------------
// Determine winner: -1 draw, 0 white wins, 1 black wins (CORRECTED)
//----------------------------------------------------------------------

int Game::getWinner(const Position &position) {
    // If there are still moves available, game is not ended
    if (!position.move_list.empty()) return -2; // Game not ended
    
    // Check for insufficient material draw
    if (isDrawByInsufficientMaterial(position)) return -1;
    
    Color us = position.SideToMove;
    Color them = (us == White) ? Black : White;
    
    // Find king square
    uint8_t kPiece = (us == White) ? wK : bK;
    U64 king_bb = position.bitboards[kPiece];
    
    if (king_bb == 0) {
        // This shouldn't happen in a valid position, but handle it
        return -1; // Draw by default
    }
    
    int king_sq = get_ls1b_index(king_bb);
    
    // If king is in check, it's checkmate; otherwise it's stalemate
    if (isSquareAttacked(king_sq, position, them)) {
        return (them == White) ? 0 : 1; // Opponent wins
    } else {
        return -1; // Stalemate = draw
    }
}

//----------------------------------------------------------------------
// Main game loop (CORRECTED)
//----------------------------------------------------------------------

void Game::Startplaying(Color YourColor, bool bot_vs_bot) {
    
    BotColor = (YourColor == White) ? Black : White;
    GameEnded = false;
    Winner = -2; // undefined

    // Threefold repetition tracking
    std::unordered_map<std::string, int> rep_count;
    
    // Generate initial moves
    currposition.generate_moves();
    
    // Track initial position
    std::string fen = currposition.get_fen();
    rep_count[fen] = 1;

    std::cout << "Game started! " << (bot_vs_bot ? "Bot vs Bot" : "Human vs Bot") << std::endl;
    
    while (!GameEnded) {
        currposition.print();
        
        // Check for game end conditions first
        if (isGameEnded(currposition)) {
            GameEnded = true;
            Winner = getWinner(currposition);
            break;
        }
        
        Move mv = 0;
        
        // Get move (bot or human)
        if (bot_vs_bot || currposition.SideToMove == BotColor) {
            std::cout << "Bot is thinking..." << std::endl;
            mv = findbestmove(currposition);
            if (mv != 0) {
                std::cout << "Bot plays: " 
                         << square_to_coordinates[get_move_source(mv)]
                         << square_to_coordinates[get_move_target(mv)] << std::endl;
                add_move(mv, moves_played);
            }
        }
        else {
            std::string move_str;
            std::cout << "Enter your move (e.g., e2e4): ";
            std::cin >> move_str;
            mv = parse_move(move_str, currposition);
            
            if (mv == 0) {
                std::cout << "Invalid move! Try again." << std::endl;
                continue;
            }
            add_move(mv, moves_played);
        }
        
        // If no valid move found, end game
        if (mv == 0) {
            GameEnded = true;
            Winner = getWinner(currposition);
            break;
        }
        
        // Make the move
        currposition = makemove(mv, currposition);
        
        // Check for threefold repetition
        fen = currposition.get_fen();
        rep_count[fen]++;
        
        if (rep_count[fen] >= 3) {
            std::cout << "Draw by threefold repetition!" << std::endl;
            GameEnded = true;
            Winner = -1;
            break;
        }
        
        // Generate moves for next turn
        currposition.generate_moves();
    }
    
    // Print final result
    currposition.print();
    
    if (Winner == -1) {
        std::cout << "Game ended in a draw." << std::endl;
    } else if (Winner == 0) {
        std::cout << "White wins!" << std::endl;
    } else if (Winner == 1) {
        std::cout << "Black wins!" << std::endl;
    } else {
        std::cout << "Game ended in a draw." << std::endl;
    }
}
