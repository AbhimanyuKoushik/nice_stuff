// movegen.cpp
#include "movegen.hpp"
#include "position.hpp"
#include "attacks.hpp"
#include "movedef.hpp"

void generate_pawn_moves(int Side, const Position& position, Moves& move_list){

    int source_square, target_square;
    U64 bitboard, attacks;

    if(Side == White){
        bitboard = position.bitboards[wP];
        while(bitboard){
            source_square = get_ls1b_index(bitboard);
            target_square = source_square - 8; //Pawn moves only 1 step front given no capture

            // The target_square should be empty and it should exist
            if(!(target_square < a8) && !get_bit(position.occupancies[Both], target_square)){
                // Pawn promotion
                if(source_square >= a7 && source_square <= h7){
                    move_list.add_move(encode_move(source_square, target_square, wP, wQ, 0, 0, 0, 0));
                    move_list.add_move(encode_move(source_square, target_square, wP, wR, 0, 0, 0, 0));
                    move_list.add_move(encode_move(source_square, target_square, wP, wB, 0, 0, 0, 0));
                    move_list.add_move(encode_move(source_square, target_square, wP, wN, 0, 0, 0, 0));
                }

                else{
                    move_list.add_move(encode_move(source_square, target_square, wP, wP, 0, 0, 0, 0));

                    // Double push initially
                    if((source_square >= a2) && (source_square <= h2) && !get_bit(position.occupancies[Both], target_square - 8)){
                        move_list.add_move(encode_move(source_square, target_square - 8, wP, wP, 0, 1, 0, 0));
                    }
                }
            }
            // Captures
            attacks = PawnAttacks[White][source_square] & position.occupancies[Black];

            while(attacks){
                target_square = get_ls1b_index(attacks);

                if(source_square >= a7 && source_square <= h7){
                    move_list.add_move(encode_move(source_square, target_square, wP, wQ, 1, 0, 0, 0));
                    move_list.add_move(encode_move(source_square, target_square, wP, wR, 1, 0, 0, 0));
                    move_list.add_move(encode_move(source_square, target_square, wP, wB, 1, 0, 0, 0));
                    move_list.add_move(encode_move(source_square, target_square, wP, wN, 1, 0, 0, 0));
                }
                
                else {
                    move_list.add_move(encode_move(source_square, target_square, wP, wP, 1, 0, 0, 0));
                }

                pop_bit(attacks, target_square);
            }

            if(position.enpassant != no_sq){
                // lookup pawn attacks and bitwise AND with enpassant square (bit)
                U64 enpassant_attacks = PawnAttacks[White][source_square] & (1ULL << position.enpassant);

                // Make sure enpassant capture is available
                if(enpassant_attacks){
                    int target_enpassant = get_ls1b_index(enpassant_attacks);
                    move_list.add_move(encode_move(source_square, target_enpassant, wP, wP, 1, 0, 1, 0));
                }
            }

            pop_bit(bitboard, source_square);
        }
    }

    else{
        bitboard = position.bitboards[bP];
        while(bitboard){
            source_square = get_ls1b_index(bitboard);
            target_square = source_square + 8; //Pawn moves only 1 step front given no capture

            // The target_square should be empty and it should exist
            if(!(target_square > h1) && !get_bit(position.occupancies[Both], target_square)){
                // Pawn promotion
                if(source_square >= a2 && source_square <= h2){
                    move_list.add_move(encode_move(source_square, target_square, bP, bQ, 0, 0, 0, 0));
                    move_list.add_move(encode_move(source_square, target_square, bP, bR, 0, 0, 0, 0));
                    move_list.add_move(encode_move(source_square, target_square, bP, bB, 0, 0, 0, 0));
                    move_list.add_move(encode_move(source_square, target_square, bP, bN, 0, 0, 0, 0));
                }

                else{
                    move_list.add_move(encode_move(source_square, target_square, bP, bP, 0, 0, 0, 0));

                    // Double push initially
                    if((source_square >= a7) && (source_square <= h7) && !get_bit(position.occupancies[Both], target_square + 8)){
                        move_list.add_move(encode_move(source_square, target_square + 8, bP, bP, 0, 1, 0, 0));
                    }
                }
            }
            // Captures
            attacks = PawnAttacks[Black][source_square] & position.occupancies[White];

            while(attacks){
                target_square = get_ls1b_index(attacks);

                if(source_square >= a2 && source_square <= h2){
                    move_list.add_move(encode_move(source_square, target_square, bP, bQ, 1, 0, 0, 0));
                    move_list.add_move(encode_move(source_square, target_square, bP, bR, 1, 0, 0, 0));
                    move_list.add_move(encode_move(source_square, target_square, bP, bB, 1, 0, 0, 0));
                    move_list.add_move(encode_move(source_square, target_square, bP, bN, 1, 0, 0, 0));
                }
                else move_list.add_move(encode_move(source_square, target_square, bP, bP, 1, 0, 0, 0));

                pop_bit(attacks, target_square);
            }

            if(position.enpassant != no_sq){
                // lookup pawn attacks and bitwise AND with enpassant square (bit)
                U64 enpassant_attacks = PawnAttacks[Black][source_square] & (1ULL << position.enpassant);

                // Make sure enpassant capture is available
                if(enpassant_attacks){
                    int target_enpassant = get_ls1b_index(enpassant_attacks);
                    move_list.add_move(encode_move(source_square, target_enpassant, bP, bP, 1, 0, 1, 0));
                }
            }

            pop_bit(bitboard, source_square);
        }

    }
}

void generate_king_moves(int Side, const Position& position, Moves& move_list){ 
    int source_square, target_square;
    U64 bitboard, attacks;
    uint8_t piece = (Side == White) ? wK : bK;
    bitboard = position.bitboards[piece];

    // Loop over source squares of piece bitboard
    while(bitboard){
        source_square = get_ls1b_index(bitboard);

        // Dont capture piece from your side
        attacks = KingAttacks[source_square] & ((Side == White) ? ~position.occupancies[White] : ~position.occupancies[Black]);
        while(attacks){
            target_square = get_ls1b_index(attacks);

            // Quiet Move
            if(!get_bit(((Side == White) ? position.occupancies[Black] : position.occupancies[White]), target_square)){
                move_list.add_move(encode_move(source_square, target_square, ((Side == White) ? wK : bK), ((Side == White) ? wK : bK), 0, 0, 0, 0));
            }


            // Capture Move
            else{
                move_list.add_move(encode_move(source_square, target_square, ((Side == White) ? wK : bK), ((Side == White) ? wK : bK), 1, 0, 0, 0));
            }

            pop_bit(attacks, target_square);
        }

        pop_bit(bitboard, source_square);
    }

    if(Side == White){
        // Check castling

        // Castling Kingside
        if(position.castling & wk){
            // Make sure the squares between king and rook are empty
            if(!get_bit(position.occupancies[Both], f1) && !get_bit(position.occupancies[Both], g1)){
                // Make sure the King and the squares in which the king travels are not attacked
                if(!isSquareAttacked(e1, position, Black) && !isSquareAttacked(f1, position, Black) && !isSquareAttacked(g1, position, Black)){
                    if(get_bit(position.bitboards[wR], h1)) move_list.add_move(encode_move(e1, g1, wK, wK, 0, 0, 0, 1));
                }
            }
        }

        // Castling Queenside
        if(position.castling & wq){
            if(!get_bit(position.occupancies[Both], b1) && !get_bit(position.occupancies[Both], c1) && !get_bit(position.occupancies[Both], d1)){
                if(!isSquareAttacked(e1, position, Black) && !isSquareAttacked(d1, position, Black) && !isSquareAttacked(c1, position, Black)){
                    if(get_bit(position.bitboards[wR], a1)) move_list.add_move(encode_move(e1, c1, wK, wK, 0, 0, 0, 1));
                }
            }
        }
    }
    else{
        if(position.castling & bk){
            // Make sure the squares between king and rook are empty
            if(!get_bit(position.occupancies[Both], f8) && !get_bit(position.occupancies[Both], g8)){
                // Make sure the King and the squares in which the king travels are not attacked
                if(!isSquareAttacked(e8, position, White) && !isSquareAttacked(f8, position, White) && !isSquareAttacked(g8, position, White)){
                    if(get_bit(position.bitboards[bR], h8)) move_list.add_move(encode_move(e8, g8, bK, bK, 0, 0, 0, 1));
                }
            }
        }

        // Castling Queenside
        if(position.castling & bq){
            if(!get_bit(position.occupancies[Both], b8) && !get_bit(position.occupancies[Both], c8) && !get_bit(position.occupancies[Both], d8)){
                if(!isSquareAttacked(e8, position, White) && !isSquareAttacked(d8, position, White) && !isSquareAttacked(c8, position, White)){
                    if(get_bit(position.bitboards[bR], a8)) move_list.add_move(encode_move(e8, c8, bK, bK, 0, 0, 0, 1));
                }
            }
        }
    }
}

void generate_knight_moves(int Side, const Position& position, Moves& move_list){
    int source_square, target_square;
    U64 bitboard, attacks;
    uint8_t piece = (Side == White) ? wN : bN;
    bitboard = position.bitboards[piece];

    // Loop over source squares of piece bitboard
    while(bitboard){
        source_square = get_ls1b_index(bitboard);

        // Dont capture piece from your side
        attacks = KnightAttacks[source_square] & ((Side == White) ? ~position.occupancies[White] : ~position.occupancies[Black]);
        while(attacks){
            target_square = get_ls1b_index(attacks);

            // Quiet Move
            if(!get_bit(((Side == White) ? position.occupancies[Black] : position.occupancies[White]), target_square)){
                move_list.add_move(encode_move(source_square, target_square, ((Side == White) ? wN : bN), ((Side == White) ? wN : bN), 0, 0, 0, 0));
            }


            // Capture Move
            else{
                move_list.add_move(encode_move(source_square, target_square, ((Side == White) ? wN : bN), ((Side == White) ? wN : bN), 1, 0, 0, 0));
            }

            pop_bit(attacks, target_square);
        }

        pop_bit(bitboard, source_square);
    }
}

void generate_bishop_moves(int Side, const Position& position, Moves& move_list){
    int source_square, target_square;
    U64 bitboard, attacks;
    uint8_t piece = (Side == White) ? wB : bB;
    bitboard = position.bitboards[piece];

    // Loop over source squares of piece bitboard
    while(bitboard){
        source_square = get_ls1b_index(bitboard);

        // Dont capture piece from your side
        attacks = bishop_attacks(source_square, position.occupancies[Both]) & ((Side == White) ? ~position.occupancies[White] : ~position.occupancies[Black]);
        while(attacks){
            target_square = get_ls1b_index(attacks);

            // Quiet Move
            if(!get_bit(((Side == White) ? position.occupancies[Black] : position.occupancies[White]), target_square)){
                move_list.add_move(encode_move(source_square, target_square, ((Side == White) ? wB : bB), ((Side == White) ? wB : bB), 0, 0, 0, 0));
            }


            // Capture Move
            else{
                move_list.add_move(encode_move(source_square, target_square, ((Side == White) ? wB : bB), ((Side == White) ? wB : bB), 1, 0, 0, 0));
            }

            pop_bit(attacks, target_square);
        }

        pop_bit(bitboard, source_square);
    }
}

void generate_rook_moves(int Side, const Position& position, Moves& move_list){
    int source_square, target_square;
    U64 bitboard, attacks;
    uint8_t piece = (Side == White) ? wR : bR;
    bitboard = position.bitboards[piece];

    // Loop over source squares of piece bitboard
    while(bitboard){
        source_square = get_ls1b_index(bitboard);

        // Dont capture piece from your side
        attacks = rook_attacks(source_square, position.occupancies[Both]) & ((Side == White) ? ~position.occupancies[White] : ~position.occupancies[Black]);
        while(attacks){
            target_square = get_ls1b_index(attacks);

            // Quiet Move
            if(!get_bit(((Side == White) ? position.occupancies[Black] : position.occupancies[White]), target_square)){
                move_list.add_move(encode_move(source_square, target_square, ((Side == White) ? wR : bR), ((Side == White) ? wR : bR), 0, 0, 0, 0));
            }


            // Capture Move
            else{
                move_list.add_move(encode_move(source_square, target_square, ((Side == White) ? wR : bR), ((Side == White) ? wR : bR), 1, 0, 0, 0));
            }

            pop_bit(attacks, target_square);
        }

        pop_bit(bitboard, source_square);
    }
}

void generate_queen_moves(int Side, const Position& position, Moves& move_list){
    int source_square, target_square;
    U64 bitboard, attacks;
    uint8_t piece = (Side == White) ? wQ : bQ;
    bitboard = position.bitboards[piece];

    // Loop over source squares of piece bitboard
    while(bitboard){
        source_square = get_ls1b_index(bitboard);

        // Dont capture piece from your side
        attacks = queen_attacks(source_square, position.occupancies[Both]) & ((Side == White) ? ~position.occupancies[White] : ~position.occupancies[Black]);
        while(attacks){
            target_square = get_ls1b_index(attacks);

            // Quiet Move
            if(!get_bit(((Side == White) ? position.occupancies[Black] : position.occupancies[White]), target_square)){
                move_list.add_move(encode_move(source_square, target_square, ((Side == White) ? wQ : bQ), ((Side == White) ? wQ : bQ), 0, 0, 0, 0));
            }


            // Capture Move
            else{
                move_list.add_move(encode_move(source_square, target_square, ((Side == White) ? wQ : bQ), ((Side == White) ? wQ : bQ), 1, 0, 0, 0));
            }

            pop_bit(attacks, target_square);
        }

        pop_bit(bitboard, source_square);
    }
}