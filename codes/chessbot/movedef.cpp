#include "movedef.hpp"
#include "types.hpp"
#include <iostream>
#include <bitset>
void print_move(Move mv){
        std::cout << "Source: "            << square_to_coordinates[ get_move_source(mv) ]                             << ", "
                  << "Target: "            << square_to_coordinates[ get_move_target(mv) ]                             << ", "
                  << "Piece: "             << unicode_pieces[ get_move_piece(mv) ]                                     << ", "
                  << "Promoted: "          << unicode_pieces[ get_move_promoted(mv) ]                                  << ", "
                  << "Captured Piece : "   << (get_move_capture_flag(mv) ? unicode_pieces[get_move_captured(mv)] : "NA") << ", "
                  << "Double Pawn Push: "  << (get_move_double(mv)    ? "Yes" : "No ")                                 << ", "
                  << "En-pass: "           << (get_move_enpassant(mv) ? "Yes" : "No ")                                 << ", "
                  << "Castle: "            << (get_move_castling(mv)  ? "Yes" : "No ")                                 << "\n";
}

void print_move_list(Moves moves){
    for (Move mv : moves) {
        print_move(mv);
    }
    std::cout << "Total Moves in List: " << moves.size() << "\n"; 
}

void add_move(Move mv, Moves &movelist) {
    movelist.push_back(mv);
}