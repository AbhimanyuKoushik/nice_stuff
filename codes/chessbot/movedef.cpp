#include "movedef.hpp"
#include "types.hpp"

void Moves::print_move(Move mv) const{
        std::cout << "Move: "             << square_to_coordinates[ get_move_source(mv) ]
                                          << square_to_coordinates[ get_move_target(mv) ] << ", "
                  << "Piece: "            << unicode_pieces[ get_move_piece(mv) ]         << ", "
                  << "Promoted: "         << unicode_pieces[ get_move_promoted(mv) ]      << ", "
                  << "Capture: "          << (get_move_capture(mv)   ? "Yes" : "No ")     << ", "
                  << "Double Pawn Push: " << (get_move_double(mv)    ? "Yes" : "No ")     << ", "
                  << "En-pass: "          << (get_move_enpassant(mv) ? "Yes" : "No ")     << ", "
                  << "Castle: "           << (get_move_castling(mv)  ? "Yes" : "No ")     << "\n";
}

void Moves::print_move_list() const {
    for (Move mv : moves) {
        print_move(mv);
    }
}

void Moves::add_move(Move mv) {
    // Debug: uncomment to see when moves are added
    //std::cout << "Added move: " << moves.size() << std::endl;
    moves.push_back(mv);
}