// movegen.cpp
#include "movegen.hpp"
#include "board.hpp"
#include "attacks.hpp"
#include "movedef.hpp"

void generate_pawn_moves(int Side, const Board& board, Moves& move_list){

    int source_square, target_square;
    U64 bitboard, attacks;

    if(Side == White){
        bitboard = board.bitboards[wP];
        while(bitboard){
            source_square = get_ls1b_index(bitboard);
            target_square = source_square - 8; //Pawn moves only 1 step front given no capture

            // The target_square should be empty and it should exist
            if(!(target_square < a8) && !get_bit(board.occupancies[Both], target_square)){
                // Pawn promotion
                if(source_square >= a7 && source_square <= h7){
                    std::cout << "Pawn promotion: " << square_to_coordinates[source_square] 
                                                    << square_to_coordinates[target_square] << "q\n";
                    move_list.add_move(encode_move(source_square, target_square, wP, wQ, 0, 0, 0, 0));
                    std::cout << "Pawn promotion: " << square_to_coordinates[source_square] 
                                                    << square_to_coordinates[target_square] << "r\n";
                    move_list.add_move(encode_move(source_square, target_square, wP, wR, 0, 0, 0, 0));
                    std::cout << "Pawn promotion: " << square_to_coordinates[source_square] 
                                                    << square_to_coordinates[target_square] << "b\n";
                    move_list.add_move(encode_move(source_square, target_square, wP, wB, 0, 0, 0, 0));
                    std::cout << "Pawn promotion: " << square_to_coordinates[source_square] 
                                                    << square_to_coordinates[target_square] << "n\n";
                    move_list.add_move(encode_move(source_square, target_square, wP, wN, 0, 0, 0, 0));
                }

                else{
                    std::cout << "Pawn push: " << square_to_coordinates[source_square]
                                                << square_to_coordinates[target_square] <<std::endl;
                    move_list.add_move(encode_move(source_square, target_square, wP, wP, 0, 0, 0, 0));

                    // Double push initially
                    if((source_square >= a2) && (source_square <= h2) && !get_bit(board.occupancies[Both], target_square - 8)){
                        target_square = target_square - 8;
                        std::cout << "Double Pawn push: " << square_to_coordinates[source_square]
                                                << square_to_coordinates[target_square] <<std::endl;
                        move_list.add_move(encode_move(source_square, target_square, wP, wP, 0, 1, 0, 0));
                    }
                }
            }
            // Captures
            attacks = PawnAttacks[White][source_square] & board.occupancies[Black];

            while(attacks){
                target_square = get_ls1b_index(attacks);

                if(source_square >= a7 && source_square <= h7){
                    std::cout << "Pawn capture promotion: " << square_to_coordinates[source_square] 
                                                    << square_to_coordinates[target_square] << "q\n";
                    move_list.add_move(encode_move(source_square, target_square, wP, wQ, 1, 0, 0, 0));
                    std::cout << "Pawn capture promotion: " << square_to_coordinates[source_square] 
                                                    << square_to_coordinates[target_square] << "r\n";
                    move_list.add_move(encode_move(source_square, target_square, wP, wR, 1, 0, 0, 0));
                    std::cout << "Pawn capture promotion: " << square_to_coordinates[source_square] 
                                                    << square_to_coordinates[target_square] << "b\n";
                    move_list.add_move(encode_move(source_square, target_square, wP, wB, 1, 0, 0, 0));
                    std::cout << "Pawn capture promotion: " << square_to_coordinates[source_square] 
                                                    << square_to_coordinates[target_square] << "n\n";
                    move_list.add_move(encode_move(source_square, target_square, wP, wN, 1, 0, 0, 0));
                }
                else std::cout << "Pawn Capture: " << square_to_coordinates[source_square]
                                                << square_to_coordinates[target_square] <<std::endl;
                move_list.add_move(encode_move(source_square, target_square, wP, wP, 1, 0, 0, 0));

                pop_bit(attacks, target_square);
            }

            if(board.enpassant != no_sq){
                // lookup pawn attacks and bitwise AND with enpassant square (bit)
                U64 enpassant_attacks = PawnAttacks[White][source_square] & (1ULL << board.enpassant);

                // Make sure enpassant capture is available
                if(enpassant_attacks){
                    int target_enpassant = get_ls1b_index(enpassant_attacks);

                    std::cout << "Pawn Enpassant Capture: " << square_to_coordinates[source_square]
                                                << square_to_coordinates[target_enpassant] <<std::endl;
                    move_list.add_move(encode_move(source_square, target_square, wP, wN, 1, 0, 1, 0));
                }
            }

            pop_bit(bitboard, source_square);
        }
    }

    else{
        bitboard = board.bitboards[bP];
        while(bitboard){
            source_square = get_ls1b_index(bitboard);
            target_square = source_square + 8; //Pawn moves only 1 step front given no capture

            // The target_square should be empty and it should exist
            if(!(target_square > h1) && !get_bit(board.occupancies[Both], target_square)){
                // Pawn promotion
                if(source_square >= a2 && source_square <= h2){
                    std::cout << "Pawn promotion: " << square_to_coordinates[source_square] 
                                                    << square_to_coordinates[target_square] << "q\n";
                    move_list.add_move(encode_move(source_square, target_square, bP, bQ, 0, 0, 0, 0));
                    std::cout << "Pawn promotion: " << square_to_coordinates[source_square] 
                                                    << square_to_coordinates[target_square] << "r\n";
                    move_list.add_move(encode_move(source_square, target_square, bP, bR, 0, 0, 0, 0));
                    std::cout << "Pawn promotion: " << square_to_coordinates[source_square] 
                                                    << square_to_coordinates[target_square] << "b\n";
                    move_list.add_move(encode_move(source_square, target_square, bP, bB, 0, 0, 0, 0));
                    std::cout << "Pawn promotion: " << square_to_coordinates[source_square] 
                                                    << square_to_coordinates[target_square] << "n\n";
                    move_list.add_move(encode_move(source_square, target_square, bP, bN, 0, 0, 0, 0));
                }

                else{
                    std::cout << "Pawn push: " << square_to_coordinates[source_square]
                                                << square_to_coordinates[target_square] <<std::endl;
                    move_list.add_move(encode_move(source_square, target_square, bP, bP, 0, 0, 0, 0));

                    // Double push initially
                    if((source_square >= a7) && (source_square <= h7) && !get_bit(board.occupancies[Both], target_square + 8)){
                        target_square = target_square + 8;
                        std::cout << "Double Pawn push: " << square_to_coordinates[source_square]
                                                << square_to_coordinates[target_square] <<std::endl;
                        move_list.add_move(encode_move(source_square, target_square, bP, bP, 0, 1, 0, 0));
                    }
                }
            }
            // Captures
            attacks = PawnAttacks[Black][source_square] & board.occupancies[White];

            while(attacks){
                target_square = get_ls1b_index(attacks);

                if(source_square >= a2 && source_square <= h2){
                    std::cout << "Pawn capture promotion: " << square_to_coordinates[source_square] 
                                                    << square_to_coordinates[target_square] << "q\n";
                    move_list.add_move(encode_move(source_square, target_square, bP, bQ, 1, 0, 0, 0));
                    std::cout << "Pawn capture promotion: " << square_to_coordinates[source_square] 
                                                    << square_to_coordinates[target_square] << "r\n";
                    move_list.add_move(encode_move(source_square, target_square, bP, bR, 1, 0, 0, 0));
                    std::cout << "Pawn capture promotion: " << square_to_coordinates[source_square] 
                                                    << square_to_coordinates[target_square] << "b\n";
                    move_list.add_move(encode_move(source_square, target_square, bP, bB, 1, 0, 0, 0));
                    std::cout << "Pawn capture promotion: " << square_to_coordinates[source_square] 
                                                    << square_to_coordinates[target_square] << "n\n";
                    move_list.add_move(encode_move(source_square, target_square, bP, bN, 1, 0, 0, 0));
                }
                else std::cout << "Pawn Capture: " << square_to_coordinates[source_square]
                                                << square_to_coordinates[target_square] <<std::endl;
                move_list.add_move(encode_move(source_square, target_square, bP, bP, 1, 0, 0, 0));

                pop_bit(attacks, target_square);
            }

            if(board.enpassant != no_sq){
                // lookup pawn attacks and bitwise AND with enpassant square (bit)
                U64 enpassant_attacks = PawnAttacks[Black][source_square] & (1ULL << board.enpassant);

                // Make sure enpassant capture is available
                if(enpassant_attacks){
                    int target_enpassant = get_ls1b_index(enpassant_attacks);

                    std::cout << "Pawn Enpassant Capture: " << square_to_coordinates[source_square]
                                                << square_to_coordinates[target_enpassant] <<std::endl;
                    move_list.add_move(encode_move(source_square, target_square, bP, bP, 1, 0, 1, 0));
                }
            }

            pop_bit(bitboard, source_square);
        }

    }
}

void generate_king_moves(int Side, const Board& board, Moves& move_list){

    int source_square, target_square;
    U64 bitboard, attacks;
    uint8_t piece = (Side == White) ? wK : bK;
    bitboard = board.bitboards[piece];

    // Loop over source squares of piece bitboard
    while(bitboard){
        source_square = get_ls1b_index(bitboard);

        // Dont capture piece from your side
        attacks = KingAttacks[source_square] & ((Side == White) ? ~board.occupancies[White] : ~board.occupancies[Black]);
        while(attacks){
            target_square = get_ls1b_index(attacks);

            if(isSquareAttacked(target_square, board, (Side == White) ? Black : White)){
                pop_bit(attacks, target_square);
                continue;
            }

            // Quiet Move
            if(!get_bit(((Side == White) ? board.occupancies[Black] : board.occupancies[White]), target_square)){
                std::cout << "King Move: " << square_to_coordinates[source_square] << square_to_coordinates[target_square] << std::endl;
                move_list.add_move(encode_move(source_square, target_square, (Side == White) ? wK : bK, (Side == White) ? wK : bK, 0, 0, 0, 0));
            }


            // Capture Move
            else{
                std::cout << "King Capture: " << square_to_coordinates[source_square] << square_to_coordinates[target_square] << std::endl;
                move_list.add_move(encode_move(source_square, target_square, (Side == White) ? wK : bK, (Side == White) ? wK : bK, 1, 0, 0, 0));
            }

            pop_bit(attacks, target_square);
        }

        pop_bit(bitboard, source_square);
    }

    if(Side == White){
        // Check castling

        // Castling Kingside
        if(board.castling & wk){
            // Make sure the squares between king and rook are empty
            if(!get_bit(board.occupancies[Both], f1) && !get_bit(board.occupancies[Both], g1)){
                // Make sure the King and the squares in which the king travels are not attacked
                if(!isSquareAttacked(e1, board, Black) && !isSquareAttacked(f1, board, Black) && !isSquareAttacked(g1, board, Black)){
                    std::cout << "Castling Move: King side - e1g1" << std::endl;
                    move_list.add_move(encode_move(e1, g1, wK, wK, 0, 0, 0, 1));
                }
            }
        }

        // Castling Queenside
        if(board.castling & wq){
            if(!get_bit(board.occupancies[Both], b1) && !get_bit(board.occupancies[Both], c1) && !get_bit(board.occupancies[Both], d1)){
                if(!isSquareAttacked(e1, board, Black) && !isSquareAttacked(d1, board, Black) && !isSquareAttacked(c1, board, Black)){
                    std::cout << "Castling Move: Queen side - e1c1" << std::endl;
                    move_list.add_move(encode_move(e1, c1, wK, wK, 0, 0, 0, 1));
                }
            }
        }
    }
    else{
        if(board.castling & bk){
            // Make sure the squares between king and rook are empty
            if(!get_bit(board.occupancies[Both], f8) && !get_bit(board.occupancies[Both], g8)){
                // Make sure the King and the squares in which the king travels are not attacked
                if(!isSquareAttacked(e8, board, White) && !isSquareAttacked(f8, board, White) && !isSquareAttacked(g8, board, White)){
                    std::cout << "Castling Move: King side - e8g8" << std::endl;
                    move_list.add_move(encode_move(e8, g8, bK, bK, 0, 0, 0, 1));
                }
            }
        }

        // Castling Queenside
        if(board.castling & bq){
            if(!get_bit(board.occupancies[Both], b8) && !get_bit(board.occupancies[Both], c8) && !get_bit(board.occupancies[Both], d8)){
                if(!isSquareAttacked(e8, board, White) && !isSquareAttacked(d8, board, White) && !isSquareAttacked(c8, board, White)){
                    std::cout << "Castling Move: Queen side - e8c8" << std::endl;
                    move_list.add_move(encode_move(e8, c8, bK, bK, 0, 0, 0, 1));
                }
            }
        }
    }
}

void generate_knight_moves(int Side, const Board& board, Moves& move_list){
    int source_square, target_square;
    U64 bitboard, attacks;
    uint8_t piece = (Side == White) ? wN : bN;
    bitboard = board.bitboards[piece];

    // Loop over source squares of piece bitboard
    while(bitboard){
        source_square = get_ls1b_index(bitboard);

        // Dont capture piece from your side
        attacks = KnightAttacks[source_square] & ((Side == White) ? ~board.occupancies[White] : ~board.occupancies[Black]);
        while(attacks){
            target_square = get_ls1b_index(attacks);

            // Quiet Move
            if(!get_bit(((Side == White) ? board.occupancies[Black] : board.occupancies[White]), target_square)){
                std::cout << "Knight Move: " << square_to_coordinates[source_square] << square_to_coordinates[target_square] << std::endl;
                move_list.add_move(encode_move(source_square, target_square, ((Side == White) ? wN : bN), ((Side == White) ? wN : bN), 0, 0, 0, 0));
            }


            // Capture Move
            else{
                std::cout << "Knight Capture: " << square_to_coordinates[source_square] << square_to_coordinates[target_square] << std::endl;
                move_list.add_move(encode_move(source_square, target_square, ((Side == White) ? wN : bN), ((Side == White) ? wN : bN), 1, 0, 0, 0));
            }

            pop_bit(attacks, target_square);
        }

        pop_bit(bitboard, source_square);
    }
}

void generate_bishop_moves(int Side, const Board& board, Moves& move_list){
    int source_square, target_square;
    U64 bitboard, attacks;
    uint8_t piece = (Side == White) ? wB : bB;
    bitboard = board.bitboards[piece];

    // Loop over source squares of piece bitboard
    while(bitboard){
        source_square = get_ls1b_index(bitboard);

        // Dont capture piece from your side
        attacks = bishop_attacks(source_square, board.occupancies[Both]) & ((Side == White) ? ~board.occupancies[White] : ~board.occupancies[Black]);
        while(attacks){
            target_square = get_ls1b_index(attacks);

            // Quiet Move
            if(!get_bit(((Side == White) ? board.occupancies[Black] : board.occupancies[White]), target_square)){
                std::cout << "Bishop Move: " << square_to_coordinates[source_square] << square_to_coordinates[target_square] << std::endl;
                move_list.add_move(encode_move(source_square, target_square, (Side == White) ? wB : bB, (Side == White) ? wB : bB, 0, 0, 0, 0));
            }


            // Capture Move
            else{
                std::cout << "Bishop Capture: " << square_to_coordinates[source_square] << square_to_coordinates[target_square] << std::endl;
                move_list.add_move(encode_move(source_square, target_square, (Side == White) ? wB : bB, (Side == White) ? wB : bB, 1, 0, 0, 0));
            }

            pop_bit(attacks, target_square);
        }

        pop_bit(bitboard, source_square);
    }
}

void generate_rook_moves(int Side, const Board& board, Moves& move_list){
    int source_square, target_square;
    U64 bitboard, attacks;
    uint8_t piece = (Side == White) ? wR : bR;
    bitboard = board.bitboards[piece];

    // Loop over source squares of piece bitboard
    while(bitboard){
        source_square = get_ls1b_index(bitboard);

        // Dont capture piece from your side
        attacks = rook_attacks(source_square, board.occupancies[Both]) & ((Side == White) ? ~board.occupancies[White] : ~board.occupancies[Black]);
        while(attacks){
            target_square = get_ls1b_index(attacks);

            // Quiet Move
            if(!get_bit(((Side == White) ? board.occupancies[Black] : board.occupancies[White]), target_square)){
                std::cout << "Rook Move: " << square_to_coordinates[source_square] << square_to_coordinates[target_square] << std::endl;
                move_list.add_move(encode_move(source_square, target_square, (Side == White) ? wR : bR, (Side == White) ? wR : bR, 0, 0, 0, 0));
            }


            // Capture Move
            else{
                std::cout << "Rook Capture: " << square_to_coordinates[source_square] << square_to_coordinates[target_square] << std::endl;
                move_list.add_move(encode_move(source_square, target_square, (Side == White) ? wR : bR, (Side == White) ? wR : bR, 1, 0, 0, 0));
            }

            pop_bit(attacks, target_square);
        }

        pop_bit(bitboard, source_square);
    }
}

void generate_queen_moves(int Side, const Board& board, Moves& move_list){
    int source_square, target_square;
    U64 bitboard, attacks;
    uint8_t piece = (Side == White) ? wQ : bQ;
    bitboard = board.bitboards[piece];

    // Loop over source squares of piece bitboard
    while(bitboard){
        source_square = get_ls1b_index(bitboard);

        // Dont capture piece from your side
        attacks = queen_attacks(source_square, board.occupancies[Both]) & ((Side == White) ? ~board.occupancies[White] : ~board.occupancies[Black]);
        while(attacks){
            target_square = get_ls1b_index(attacks);

            // Quiet Move
            if(!get_bit(((Side == White) ? board.occupancies[Black] : board.occupancies[White]), target_square)){
                std::cout << "Queen Move: " << square_to_coordinates[source_square] << square_to_coordinates[target_square] << std::endl;
                move_list.add_move(encode_move(source_square, target_square, (Side == White) ? wQ : bQ, (Side == White) ? wQ : bQ, 0, 0, 0, 0));
            }


            // Capture Move
            else{
                std::cout << "Queen Capture: " << square_to_coordinates[source_square] << square_to_coordinates[target_square] << std::endl;
                move_list.add_move(encode_move(source_square, target_square, (Side == White) ? wQ : bQ, (Side == White) ? wQ : bQ, 1, 0, 0, 0));
            }

            pop_bit(attacks, target_square);
        }

        pop_bit(bitboard, source_square);
    }
}