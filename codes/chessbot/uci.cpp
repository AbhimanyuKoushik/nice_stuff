#include "uci.hpp"
#include "position.hpp"
#include "movedef.hpp"
#include <iostream>

/*
    Example UCI commands to init position on chess board
    
    // init start position
    position startpos
    
    // init start position and make the moves on chess board
    position startpos moves e2e4 e7e5
    
    // init position from FEN string
    position fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 
    
    // init position from fen string and make moves on chess board
    position fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 moves e2a6 e8g8
*/


Move parse_move(const std::string& move_str, Position position) {
    // Must have at least source and target squares
    if (move_str.size() < 4)
        return 0;
    position.generate_moves();
    // Convert file ('a'..'h') and rank ('1'..'8') to 0-based square index
    int src_file = move_str[0] - 'a';
    int src_rank = move_str[1] - '0';
    int dst_file = move_str[2] - 'a';
    int dst_rank = move_str[3] - '0';

    // Compute square indices (0 = a8, 7 = h8, 8 = a7, ... , 63 = h1)
    int source_square = src_file + (8 - src_rank) * 8;
    int target_square = dst_file + (8 - dst_rank) * 8;

    // Iterate through all generated legal moves
    for (Move mv : position.move_list.moves) {
        if (source_square == get_move_source(mv) && target_square == get_move_target(mv)){
            int promoted = get_move_promoted(mv);
            // Handle promotions
            if (promoted) {
                if (move_str.size() == 5) {
                    char promo_char = std::tolower(move_str[4]);
                    if ((promoted == wQ || promoted == bQ) && promo_char == 'q')
                        return mv;
                    if ((promoted == wR || promoted == bR) && promo_char == 'r')
                        return mv;
                    if ((promoted == wB || promoted == bB) && promo_char == 'b')
                        return mv;
                    if ((promoted == wN || promoted == bN) && promo_char == 'n')
                        return mv;
                }
                // Wrong promotion piece requested, skip this move
                else{
                    return mv;
                }
            }

            // Non-promotion moves match
        }
    }

    // No matching legal move found
    return 0;
}


