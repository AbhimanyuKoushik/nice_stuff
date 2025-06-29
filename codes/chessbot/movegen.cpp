#include "movegen.hpp"
#include "position.hpp"
#include "attacks.hpp"
#include "movedef.hpp"

void generate_pawn_moves(int Side,const Position& position, Moves& move_list){
    int source_square, target_square;
    U64 bitboard, attacks, ForPieceAttack;
    int PieceAttack = Em;

    if(Side == White){
        bitboard = position.bitboards[wP];
        while(bitboard){
            source_square = get_ls1b_index(bitboard);
            target_square = source_square - 8; // White pawns move up (decreasing square numbers)

            // Single pawn push - target square should be empty and valid
            if(target_square >= a8 && !get_bit(position.occupancies[Both], target_square)){
                // Pawn promotion (7th rank to 8th rank)
                if(source_square >= a7 && source_square <= h7){
                    add_move(encode_move(source_square, target_square, wP, wQ, 0, Em, 0, 0, 0), move_list);
                    add_move(encode_move(source_square, target_square, wP, wR, 0, Em, 0, 0, 0), move_list);
                    add_move(encode_move(source_square, target_square, wP, wB, 0, Em, 0, 0, 0), move_list);
                    add_move(encode_move(source_square, target_square, wP, wN, 0, Em, 0, 0, 0), move_list);
                }
                else{
                    // Regular single push
                    add_move(encode_move(source_square, target_square, wP, wP, 0, Em, 0, 0, 0), move_list);
                    
                    // Double push from starting position (2nd rank)
                    if((source_square >= a2) && (source_square <= h2) && !get_bit(position.occupancies[Both], target_square - 8)){
                        add_move(encode_move(source_square, target_square - 8, wP, wP, 0, Em, 1, 0, 0), move_list);
                    }
                }
            }

            // Pawn captures
            attacks = PawnAttacks[White][source_square] & position.occupancies[Black];
            while(attacks){
                target_square = get_ls1b_index(attacks);
                
                // Determine what piece is being captured
                PieceAttack = Em;
                if(get_bit(position.bitboards[bP], target_square)) PieceAttack = bP;
                else if(get_bit(position.bitboards[bN], target_square)) PieceAttack = bN;
                else if(get_bit(position.bitboards[bB], target_square)) PieceAttack = bB;
                else if(get_bit(position.bitboards[bR], target_square)) PieceAttack = bR;
                else if(get_bit(position.bitboards[bQ], target_square)) PieceAttack = bQ;
                else if(get_bit(position.bitboards[bK], target_square)) PieceAttack = bK;

                // Capture with promotion
                if(source_square >= a7 && source_square <= h7){
                    add_move(encode_move(source_square, target_square, wP, wQ, 1, PieceAttack, 0, 0, 0), move_list);
                    add_move(encode_move(source_square, target_square, wP, wR, 1, PieceAttack, 0, 0, 0), move_list);
                    add_move(encode_move(source_square, target_square, wP, wB, 1, PieceAttack, 0, 0, 0), move_list);
                    add_move(encode_move(source_square, target_square, wP, wN, 1, PieceAttack, 0, 0, 0), move_list);
                }
                else{
                    // Regular capture
                    add_move(encode_move(source_square, target_square, wP, wP, 1, PieceAttack, 0, 0, 0), move_list);
                }
                pop_bit(attacks, target_square);
            }

            // En passant capture
            if(position.enpassant != no_sq){
                U64 enpassant_attacks = PawnAttacks[White][source_square] & (1ULL << position.enpassant);
                if(enpassant_attacks){
                    int target_enpassant = get_ls1b_index(enpassant_attacks);
                    add_move(encode_move(source_square, target_enpassant, wP, wP, 1, bP, 0, 1, 0), move_list);
                }
            }

            pop_bit(bitboard, source_square);
        }
    }
    else{ // Black pawns
        bitboard = position.bitboards[bP];
        while(bitboard){
            source_square = get_ls1b_index(bitboard);
            target_square = source_square + 8; // Black pawns move down (increasing square numbers)

            // Single pawn push - target square should be empty and valid
            if(target_square <= h1 && !get_bit(position.occupancies[Both], target_square)){
                // Pawn promotion (2nd rank to 1st rank)
                if(source_square >= a2 && source_square <= h2){
                    add_move(encode_move(source_square, target_square, bP, bQ, 0, Em, 0, 0, 0), move_list);
                    add_move(encode_move(source_square, target_square, bP, bR, 0, Em, 0, 0, 0), move_list);
                    add_move(encode_move(source_square, target_square, bP, bB, 0, Em, 0, 0, 0), move_list);
                    add_move(encode_move(source_square, target_square, bP, bN, 0, Em, 0, 0, 0), move_list);
                }
                else{
                    // Regular single push
                    add_move(encode_move(source_square, target_square, bP, bP, 0, Em, 0, 0, 0), move_list);
                    
                    // Double push from starting position (7th rank)
                    if((source_square >= a7) && (source_square <= h7) && !get_bit(position.occupancies[Both], target_square + 8)){
                        add_move(encode_move(source_square, target_square + 8, bP, bP, 0, Em, 1, 0, 0), move_list);
                    }
                }
            }

            // Pawn captures
            attacks = PawnAttacks[Black][source_square] & position.occupancies[White];
            while(attacks){
                target_square = get_ls1b_index(attacks);
                
                // Determine what piece is being captured
                PieceAttack = Em;
                if(get_bit(position.bitboards[wP], target_square)) PieceAttack = wP;
                else if(get_bit(position.bitboards[wN], target_square)) PieceAttack = wN;
                else if(get_bit(position.bitboards[wB], target_square)) PieceAttack = wB;
                else if(get_bit(position.bitboards[wR], target_square)) PieceAttack = wR;
                else if(get_bit(position.bitboards[wQ], target_square)) PieceAttack = wQ;
                else if(get_bit(position.bitboards[wK], target_square)) PieceAttack = wK;

                // Capture with promotion
                if(source_square >= a2 && source_square <= h2){
                    add_move(encode_move(source_square, target_square, bP, bQ, 1, PieceAttack, 0, 0, 0), move_list);
                    add_move(encode_move(source_square, target_square, bP, bR, 1, PieceAttack, 0, 0, 0), move_list);
                    add_move(encode_move(source_square, target_square, bP, bB, 1, PieceAttack, 0, 0, 0), move_list);
                    add_move(encode_move(source_square, target_square, bP, bN, 1, PieceAttack, 0, 0, 0), move_list);
                }
                else{
                    // Regular capture
                    add_move(encode_move(source_square, target_square, bP, bP, 1, PieceAttack, 0, 0, 0), move_list);
                }
                pop_bit(attacks, target_square);
            }

            // En passant capture
            if(position.enpassant != no_sq){
                U64 enpassant_attacks = PawnAttacks[Black][source_square] & (1ULL << position.enpassant);
                if(enpassant_attacks){
                    int target_enpassant = get_ls1b_index(enpassant_attacks);
                    add_move(encode_move(source_square, target_enpassant, bP, bP, 1, wP, 0, 1, 0), move_list);
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
    int PieceAttack = Em;
    bitboard = position.bitboards[piece];

    // Loop over source squares of piece bitboard
    while(bitboard){
        source_square = get_ls1b_index(bitboard);

        // Dont capture piece from your side
        attacks = KingAttacks[source_square] & ((Side == White) ? ~position.occupancies[White] : ~position.occupancies[Black]);
        while(attacks){
            target_square = get_ls1b_index(attacks);
            PieceAttack = Em;
            if(Side == White){
                if(get_bit(position.bitboards[bP], target_square)) PieceAttack = bP;
                else if(get_bit(position.bitboards[bN], target_square)) PieceAttack = bN;
                else if(get_bit(position.bitboards[bB], target_square)) PieceAttack = bB;
                else if(get_bit(position.bitboards[bR], target_square)) PieceAttack = bR;
                else if(get_bit(position.bitboards[bQ], target_square)) PieceAttack = bQ;
                else if(get_bit(position.bitboards[bK], target_square)) PieceAttack = bK;
            }
            else{
                if(get_bit(position.bitboards[wP], target_square)) PieceAttack = wP;
                else if(get_bit(position.bitboards[wN], target_square)) PieceAttack = wN;
                else if(get_bit(position.bitboards[wB], target_square)) PieceAttack = wB;
                else if(get_bit(position.bitboards[wR], target_square)) PieceAttack = wR;
                else if(get_bit(position.bitboards[wQ], target_square)) PieceAttack = wQ;
                else if(get_bit(position.bitboards[wK], target_square)) PieceAttack = wK;
            }
            if(isSquareAttacked(target_square, position, (Side == White) ? Black : White)){
                pop_bit(attacks, target_square);
                continue;
            }

            // Quiet Move
            if(!get_bit(((Side == White) ? position.occupancies[Black] : position.occupancies[White]), target_square)){
                add_move(encode_move(source_square, target_square, ((Side == White) ? wK : bK), ((Side == White) ? wK : bK), 0, Em, 0, 0, 0), move_list);
            }


            // Capture Move
            else{
                add_move(encode_move(source_square, target_square, ((Side == White) ? wK : bK), ((Side == White) ? wK : bK), 1, PieceAttack, 0, 0, 0), move_list);
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
                   if(get_bit(position.bitboards[wR], h1)) add_move(encode_move(e1, g1, wK, wK, 0, Em, 0, 0, 1), move_list);
                }
            }
        }

        // Castling Queenside
        if(position.castling & wq){
            if(!get_bit(position.occupancies[Both], b1) && !get_bit(position.occupancies[Both], c1) && !get_bit(position.occupancies[Both], d1)){
                if(!isSquareAttacked(e1, position, Black) && !isSquareAttacked(d1, position, Black) && !isSquareAttacked(c1, position, Black)){
                    if(get_bit(position.bitboards[wR], a1)) add_move(encode_move(e1, c1, wK, wK, 0, Em, 0, 0, 1), move_list);
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
                    if(get_bit(position.bitboards[bR], h8)) add_move(encode_move(e8, g8, bK, bK, 0, Em, 0, 0, 1), move_list);
                }
            }
        }

        // Castling Queenside
        if(position.castling & bq){
            if(!get_bit(position.occupancies[Both], b8) && !get_bit(position.occupancies[Both], c8) && !get_bit(position.occupancies[Both], d8)){
                if(!isSquareAttacked(e8, position, White) && !isSquareAttacked(d8, position, White) && !isSquareAttacked(c8, position, White)){
                    if(get_bit(position.bitboards[bR], a8)) add_move(encode_move(e8, c8, bK, bK, 0, Em, 0, 0, 1), move_list);
                }
            }
        }
    }
}

void generate_knight_moves(int Side, const Position& position, Moves& move_list){
    int source_square, target_square;
    U64 bitboard, attacks;
    uint8_t piece = (Side == White) ? wN : bN;
    int PieceAttack = Em;
    bitboard = position.bitboards[piece];

    // Loop over source squares of piece bitboard
    while(bitboard){
        source_square = get_ls1b_index(bitboard);

        // Dont capture piece from your side
        attacks = KnightAttacks[source_square] & ((Side == White) ? ~position.occupancies[White] : ~position.occupancies[Black]);
        while(attacks){
            target_square = get_ls1b_index(attacks);
            PieceAttack = Em;
            if(Side == White){
                if(get_bit(position.bitboards[bP], target_square)) PieceAttack = bP;
                else if(get_bit(position.bitboards[bN], target_square)) PieceAttack = bN;
                else if(get_bit(position.bitboards[bB], target_square)) PieceAttack = bB;
                else if(get_bit(position.bitboards[bR], target_square)) PieceAttack = bR;
                else if(get_bit(position.bitboards[bQ], target_square)) PieceAttack = bQ;
                else if(get_bit(position.bitboards[bK], target_square)) PieceAttack = bK;
            }
            else{
                if(get_bit(position.bitboards[wP], target_square)) PieceAttack = wP;
                else if(get_bit(position.bitboards[wN], target_square)) PieceAttack = wN;
                else if(get_bit(position.bitboards[wB], target_square)) PieceAttack = wB;
                else if(get_bit(position.bitboards[wR], target_square)) PieceAttack = wR;
                else if(get_bit(position.bitboards[wQ], target_square)) PieceAttack = wQ;
                else if(get_bit(position.bitboards[wK], target_square)) PieceAttack = wK;
            }
            // Quiet Move
            if(!get_bit(((Side == White) ? position.occupancies[Black] : position.occupancies[White]), target_square)){
                add_move(encode_move(source_square, target_square, ((Side == White) ? wN : bN), ((Side == White) ? wN : bN), 0, Em, 0, 0, 0), move_list);
            }


            // Capture Move
            else{
                add_move(encode_move(source_square, target_square, ((Side == White) ? wN : bN), ((Side == White) ? wN : bN), 1, PieceAttack, 0, 0, 0), move_list);
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
    int PieceAttack = Em;
    bitboard = position.bitboards[piece];

    // Loop over source squares of piece bitboard
    while(bitboard){
        source_square = get_ls1b_index(bitboard);

        // Dont capture piece from your side
        attacks = bishop_attacks(source_square, position.occupancies[Both]) & ((Side == White) ? ~position.occupancies[White] : ~position.occupancies[Black]);
        while(attacks){
            target_square = get_ls1b_index(attacks);
            PieceAttack = Em;
            if(Side == White){
                if(get_bit(position.bitboards[bP], target_square)) PieceAttack = bP;
                else if(get_bit(position.bitboards[bN], target_square)) PieceAttack = bN;
                else if(get_bit(position.bitboards[bB], target_square)) PieceAttack = bB;
                else if(get_bit(position.bitboards[bR], target_square)) PieceAttack = bR;
                else if(get_bit(position.bitboards[bQ], target_square)) PieceAttack = bQ;
                else if(get_bit(position.bitboards[bK], target_square)) PieceAttack = bK;
            }
            else{
                if(get_bit(position.bitboards[wP], target_square)) PieceAttack = wP;
                else if(get_bit(position.bitboards[wN], target_square)) PieceAttack = wN;
                else if(get_bit(position.bitboards[wB], target_square)) PieceAttack = wB;
                else if(get_bit(position.bitboards[wR], target_square)) PieceAttack = wR;
                else if(get_bit(position.bitboards[wQ], target_square)) PieceAttack = wQ;
                else if(get_bit(position.bitboards[wK], target_square)) PieceAttack = wK;
            }

            // Quiet Move
            if(!get_bit(((Side == White) ? position.occupancies[Black] : position.occupancies[White]), target_square)){
                add_move(encode_move(source_square, target_square, (Side == White) ? wB : bB, (Side == White) ? wB : bB, 0, Em, 0, 0, 0), move_list);
            }


            // Capture Move
            else{
                add_move(encode_move(source_square, target_square, (Side == White) ? wB : bB, (Side == White) ? wB : bB, 1, PieceAttack, 0, 0, 0), move_list);
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
    int PieceAttack = Em;
    bitboard = position.bitboards[piece];

    // Loop over source squares of piece bitboard
    while(bitboard){
        source_square = get_ls1b_index(bitboard);

        // Dont capture piece from your side
        attacks = rook_attacks(source_square, position.occupancies[Both]) & ((Side == White) ? ~position.occupancies[White] : ~position.occupancies[Black]);
        while(attacks){
            target_square = get_ls1b_index(attacks);
            PieceAttack = Em;
            if(Side == White){
                if(get_bit(position.bitboards[bP], target_square)) PieceAttack = bP;
                else if(get_bit(position.bitboards[bN], target_square)) PieceAttack = bN;
                else if(get_bit(position.bitboards[bB], target_square)) PieceAttack = bB;
                else if(get_bit(position.bitboards[bR], target_square)) PieceAttack = bR;
                else if(get_bit(position.bitboards[bQ], target_square)) PieceAttack = bQ;
                else if(get_bit(position.bitboards[bK], target_square)) PieceAttack = bK;
            }
            else{
                if(get_bit(position.bitboards[wP], target_square)) PieceAttack = wP;
                else if(get_bit(position.bitboards[wN], target_square)) PieceAttack = wN;
                else if(get_bit(position.bitboards[wB], target_square)) PieceAttack = wB;
                else if(get_bit(position.bitboards[wR], target_square)) PieceAttack = wR;
                else if(get_bit(position.bitboards[wQ], target_square)) PieceAttack = wQ;
                else if(get_bit(position.bitboards[wK], target_square)) PieceAttack = wK;
            }
            // Quiet Move
            if(!get_bit(((Side == White) ? position.occupancies[Black] : position.occupancies[White]), target_square)){
                add_move(encode_move(source_square, target_square, (Side == White) ? wR : bR, (Side == White) ? wR : bR, 0, Em, 0, 0, 0), move_list);
            }


            // Capture Move
            else{
                add_move(encode_move(source_square, target_square, (Side == White) ? wR : bR, (Side == White) ? wR : bR, 1, PieceAttack, 0, 0, 0), move_list);
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
    int PieceAttack = Em;
    bitboard = position.bitboards[piece];

    // Loop over source squares of piece bitboard
    while(bitboard){
        source_square = get_ls1b_index(bitboard);

        // Dont capture piece from your side
        attacks = queen_attacks(source_square, position.occupancies[Both]) & ((Side == White) ? ~position.occupancies[White] : ~position.occupancies[Black]);
        while(attacks){
            target_square = get_ls1b_index(attacks);
            PieceAttack = Em;
            if(Side == White){
                if(get_bit(position.bitboards[bP], target_square)) PieceAttack = bP;
                else if(get_bit(position.bitboards[bN], target_square)) PieceAttack = bN;
                else if(get_bit(position.bitboards[bB], target_square)) PieceAttack = bB;
                else if(get_bit(position.bitboards[bR], target_square)) PieceAttack = bR;
                else if(get_bit(position.bitboards[bQ], target_square)) PieceAttack = bQ;
                else if(get_bit(position.bitboards[bK], target_square)) PieceAttack = bK;
            }
            else{
                if(get_bit(position.bitboards[wP], target_square)) PieceAttack = wP;
                else if(get_bit(position.bitboards[wN], target_square)) PieceAttack = wN;
                else if(get_bit(position.bitboards[wB], target_square)) PieceAttack = wB;
                else if(get_bit(position.bitboards[wR], target_square)) PieceAttack = wR;
                else if(get_bit(position.bitboards[wQ], target_square)) PieceAttack = wQ;
                else if(get_bit(position.bitboards[wK], target_square)) PieceAttack = wK;
            }

            // Quiet Move
            if(!get_bit(((Side == White) ? position.occupancies[Black] : position.occupancies[White]), target_square)){
                add_move(encode_move(source_square, target_square, (Side == White) ? wQ : bQ, (Side == White) ? wQ : bQ, 0, Em, 0, 0, 0), move_list);
            }


            // Capture Move
            else{
                add_move(encode_move(source_square, target_square, (Side == White) ? wQ : bQ, (Side == White) ? wQ : bQ, 1, PieceAttack, 0, 0, 0), move_list);
            }

            pop_bit(attacks, target_square);
        }

        pop_bit(bitboard, source_square);
    }
}