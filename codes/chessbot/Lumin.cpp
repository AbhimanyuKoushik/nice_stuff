#include <iostream>
#include <chrono>
#include <bitset>
#include <string>
#include <ctime>
#include <cstdlib>

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
#include "evaluate.hpp"

// Initialize all attack tables
void init_all() {
    init_sliders();      // Magic bitboards for sliding pieces
    init_nonsliders();   // Lookup tables for pawns, knights, kings
    std::cout << "Attack tables initialized.\n";
}

int main() {
    // Initialize random seed
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    // Initialize all attack tables, bitboards, magics, etc.
    init_all();
    
    std::cout << "Welcome to " << NAME << "!\n";
    std::cout << "Choose mode:\n";
    std::cout << "1. Bot vs Human\n";
    std::cout << "2. Bot vs Bot\n";
    std::cout << "3. Test Position\n";
    std::cout << "Enter your choice (1, 2, or 3): ";

    int mode = 0;
    std::cin >> mode;
    std::cin.ignore(); // Clear newline

    if (mode == 1) {
        // Set up initial position
        std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        Position start = parsefen(fen);

        Game game;
        game.currposition = start;

        std::cout << "Choose your side (w for White, b for Black): ";
        char side;
        std::cin >> side;
        Color userColor = (side == 'w' || side == 'W') ? White : Black;
        game.Startplaying(userColor, false);
        
    } else if (mode == 2) {
        // Set up initial position
        std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        Position start = parsefen(fen);

        Game game;
        game.currposition = start;

        std::cout << "Bot vs Bot mode selected. Press Enter to start.\n";
        std::cin.ignore();
        game.Startplaying(White, true); // Bot vs Bot, White starts
        
    } else if (mode == 3) {
        // Test position mode
        std::cout << "Testing evaluation function...\n";
        
        // Test position: Italian Game opening
        Position position = parsefen("rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1");
        
        std::cout << "\nCurrent position:\n";
        position.print();
        
        std::cout << "\nSearching for best move...\n";
        Move bestMove = findbestmove(position);
        
        if (bestMove != 0) {
            std::cout << "\nBest move found: ";
            print_move(bestMove);
            std::cout << std::endl;
            
            // Show move details
            std::cout << "Move details: " 
                      << square_to_coordinates[get_move_source(bestMove)] 
                      << square_to_coordinates[get_move_target(bestMove)] << std::endl;
            
            // Apply the move and show resulting position
            Position newPos = makemove(bestMove, position);
            std::cout << "\nPosition after move:\n";
            newPos.print();
            
        } else {
            std::cout << "No valid move found!\n";
        }
        
    } else {
        std::cout << "Invalid mode selected.\n";
        return 1;
    }

    std::cout << "Thank you for playing!\n";
    return 0;
}
