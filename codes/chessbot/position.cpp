#include "position.hpp"
#include "movegen.hpp"    // to get the implementations of generate_* 
#include "attacks.hpp"    // if you use them in Board:: methods
#include "nonmagic.hpp"
#include "magic.hpp"
#include "types.hpp"
#include "movedef.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

//–– parseFEN ––
Position parsefen(const std::string &fen) {
    Position position;
    position.emptyBoard();

    // 1) Split FEN into its 6 fields
    std::istringstream iss(fen);
    std::string boardPart, stmPart, castlePart, epPart;
    int halfmoveClock, fullmoveNumber;
    iss >> boardPart >> stmPart >> castlePart >> epPart
        >> halfmoveClock >> fullmoveNumber;
    // 2) Piece placement: start at a8, move left‑to‑right, top‑to‑bottom
    int sq = a8;
    for (char c : boardPart) {
        if (std::isdigit(c)) {
            sq += c - '0';             // skip empty squares
        }
        else if (c == '/') {
            continue; // Move to start of next rank
        }
        else {
            Piece p;
            switch (c) {
                case 'P': p = wP; break;
                case 'N': p = wN; break;
                case 'B': p = wB; break;
                case 'R': p = wR; break;
                case 'Q': p = wQ; break;
                case 'K': p = wK; break;
                case 'p': p = bP; break;
                case 'n': p = bN; break;
                case 'b': p = bB; break;
                case 'r': p = bR; break;
                case 'q': p = bQ; break;
                case 'k': p = bK; break;
                default:  p = Em;  break;
            }
            set_bit(position.bitboards[p], sq);
            ++sq;
        }
    }
    
    // 3) Side to move
    position.SideToMove = (stmPart == "w" ? White : Black);

    // 4) Castling rights
    position.castling = 0;
    if (castlePart.find('K') != std::string::npos) position.castling |= wk;
    if (castlePart.find('Q') != std::string::npos) position.castling |= wq;
    if (castlePart.find('k') != std::string::npos) position.castling |= bk;
    if (castlePart.find('q') != std::string::npos) position.castling |= bq;
    
    // 5) En passant target
    if (epPart != "-") {
        int file = epPart[0] - 'a';
        int rank = '9' - epPart[1] - 1;
        position.enpassant = rank * 8 + file;
    } else {
        position.enpassant = no_sq;
    }


    // 6) Rebuild occupancy bitboards
    position.compute_occupancies();
    
    if(halfmoveClock == 100) position.FiftyMove = true;

    return position;
}

std::string Position::get_fen() const {
    std::ostringstream oss;

    // 1) Piece placement
    for (int rank = 7; rank >= 0; --rank) {
        int empty = 0;
        for (int file = 0; file < 8; ++file) {
            int sq = rank*8 + file;
            // find which piece occupies this square
            int ptype = Em;
            for (int p = wP; p <= bK; ++p) {
                if (bitboards[p] & (1ULL << sq)) {
                    ptype = p;
                    break;
                }
            }
            if (ptype == Em) {
                ++empty;
            } else {
                if (empty) { oss << empty; empty = 0; }
                static const char sym[12] = {
                    'P','N','B','R','Q','K',
                    'p','n','b','r','q','k'
                };
                oss << sym[ptype];
            }
        }
        if (empty) oss << empty;
        if (rank)   oss << '/';
    }

    // 2) Side to move
    oss << ' ' << (SideToMove==White?'w':'b');

    // 3) Castling
    oss << ' ';
    bool any = false;
    if (castling & wk) { oss << 'K'; any = true; }
    if (castling & wq) { oss << 'Q'; any = true; }
    if (castling & bk) { oss << 'k'; any = true; }
    if (castling & bq) { oss << 'q'; any = true; }
    if (!any)         oss << '-';

    // 4) En passant
    oss << ' ';
    if (enpassant != no_sq) {
        int f = enpassant % 8;
        int r = enpassant / 8;
        oss << char('a'+f) << char('1'+r);
    } else {
        oss << '-';
    }

    // 5) Halfmove clock (if you track it, use that; else default to 0)
    oss << " 0";

    // 6) Fullmove number (if you track it, use that; else default to 1)
    oss << " 1";

    return oss.str();
}

//–– print ––
void Position::print() const {
    std::cout << "\n";
    // 1) Board diagram: ranks 8→1, files a→h
    for (int rank = 7; rank >= 0; --rank) {
        std::cout << (rank+1) << " ";
        for (int file = 0; file < 8; ++file) {
            int sq = (7 - rank)*8 + file;
            // default empty
            const char* sym = ".";
            // scan piece bitboards for something on this square
            for (int p = wP; p <= bK; ++p) {
                if (bitboards[p] & (1ULL << sq)) {
                    sym = unicode_pieces[p];
                    break;
                }
            }
            std::printf("%s ", sym);
        }
        std::printf("\n");
    }

    // 2) Files labels
    std::cout << "  a b c d e f g h\n\n";

    // 3) Side to move
    std::cout << "Side to move: "
              << (SideToMove == White ? "White" : "Black")
              << "\n";

    // 4) En passant
    std::cout << "Enpassant Square: "
              << (enpassant == no_sq
                  ? "No Square"
                  : square_to_coordinates[enpassant])
              << "\n";

    // 5) Castling rights
    std::cout << "Castling: ";
    std::cout << ((castling & wk) ? "White Kingside "  : "- ");
    std::cout << ((castling & wq) ? "White Queenside " : "- ");
    std::cout << ((castling & bk) ? "Black Kingside "  : "- ");
    std::cout << ((castling & bq) ? "Black Queenside"   : "- ");
    std::cout << "\n\n";
}

//–– generate_moves ––
void Position::generate_moves() {
    // call into your free functions:

    move_list = {}; // Add this line

    generate_knight_moves(SideToMove, *this, move_list);
    generate_bishop_moves(SideToMove, *this, move_list);
    generate_rook_moves(SideToMove, *this, move_list);
    generate_queen_moves(SideToMove, *this, move_list);
    generate_pawn_moves(SideToMove, *this, move_list);
    generate_king_moves(SideToMove, *this, move_list);
    FilterLegalMoves(*this);
    // plus sliding pieces, etc.
}

void Position::init() {
    castling   = wk | wq | bk | bq;      // All castling rights initially
    FiftyMove  = false;
    SideToMove = White;

    // Pawn ranks
    bitboards[bP] = 0x000000000000FF00ULL;  // white pawns on rank 2
    bitboards[wP] = 0x00FF000000000000ULL;  // black pawns on rank 7

    // Knights on b1/g1  and b8/g8
    bitboards[bN] = 0x0000000000000042ULL;  // bits 1 & 6
    bitboards[wN] = 0x4200000000000000ULL;  // bits 57 & 62

    // Bishops on c1/e1  and c8/e8
    bitboards[bB] = 0x0000000000000024ULL;  // bits 2 & 5
    bitboards[wB] = 0x2400000000000000ULL;  // bits 58 & 61

    // Rooks on a1/h1  and a8/h8
    bitboards[bR] = 0x0000000000000081ULL;  // bits 0 & 7
    bitboards[wR] = 0x8100000000000000ULL;  // bits 56 & 63

    // Queens on d1 and d8
    bitboards[bQ] = 0x0000000000000008ULL;  // bit 3
    bitboards[wQ] = 0x0800000000000000ULL;  // bit 59

    // Kings on e1 and e8
    bitboards[bK] = 0x0000000000000010ULL;  // bit 4
    bitboards[wK] = 0x1000000000000000ULL;  // bit 60

    // If you have a compute_occupancies() helper, call it now:
    compute_occupancies();
}

void Position::compute_occupancies() {
    occupancies[0] = occupancies[1] = occupancies[2] = 0ULL;
    for (int i = 0; i < 6; i++)
        occupancies[0] |= bitboards[i];
    for (int i = 6; i < 12; i++)
        occupancies[1] |= bitboards[i];
    occupancies[2] = occupancies[0] | occupancies[1];
}

void Position::emptyBoard() {
    for(uint8_t i = 0; i < 12; i++){
        bitboards[i] = 0ULL;
    }
    occupancies[0] = 0ULL;
    occupancies[1] = 0ULL;
    occupancies[2] = 0ULL;
}

void Position::order_moves() {
    int n = move_list.size();
    if (n <= 1) return;

    // Reserve a scratch buffer of (score, move) pairs
    std::vector<std::pair<int, Move>> scored;
    scored.reserve(n);

    // Precompute material scores locally for speed
    const int *mat = material_score;

    Color us   = SideToMove;
    Color them = (us == White ? Black : White);

    // 1) Score each move once
    for (Move mv : move_list) {
        int score = 0;

        // MVV/LVA: Most Valuable Victim, Least Valuable Aggressor
        if (get_move_capture_flag(mv)) {
            int victim   = get_move_captured(mv);
            int attacker = get_move_piece(mv);
            score += 10 * mat[victim] - mat[attacker];
        }

        // Promotion bonus
        int promo = get_move_promoted(mv);
        if (promo) {
            score += mat[promo];
        }

        scored.emplace_back(score, mv);
    }

    // 2) Sort by score descending
    std::sort(
        scored.begin(), scored.end(),
        [](auto &a, auto &b){
            return a.first > b.first;
        }
    );

    // 3) Unpack back into move_list
    for (int i = 0; i < n; ++i) {
        move_list[i] = scored[i].second;
    }
}

Position makemove(Move move, Position position){
    Position original_position = position;    
    // Quiet Moves
    int source_square = get_move_source(move);
    int target_square = get_move_target(move);
    int piece = get_move_piece(move);
    int promoted = get_move_promoted(move);
    int capture = get_move_captured(move);
    int doublepush = get_move_double(move);
    int enpassant = get_move_enpassant(move);
    int castling = get_move_castling(move);

    pop_bit(position.bitboards[piece], source_square);
    set_bit(position.bitboards[piece], target_square);

    if(capture != Em){
        // Remove the captured piece from the bitboards
        //std::cout << "Side To move: " << (position.SideToMove == White ? "White" : "Black") << "\n";
        //std::cout << unicode_pieces[bb_piece] << " captured by " << unicode_pieces[piece] << " moved from "<< square_to_coordinates[source_square]
        //<< " to square " << square_to_coordinates[target_square] << "\n";
        pop_bit(position.bitboards[capture], target_square);
    }

    if(promoted){
        pop_bit(position.bitboards[position.SideToMove == White ? wP : bP], target_square);
        set_bit(position.bitboards[promoted], target_square);
    }

    if(enpassant){
        (position.SideToMove == White) ? 
            pop_bit(position.bitboards[bP], target_square + 8) : 
            pop_bit(position.bitboards[wP], target_square - 8);
    }

    position.enpassant = no_sq;
    if(doublepush){
        position.enpassant = (position.SideToMove == White) ? 
            (target_square + 8) : (target_square - 8);
    }

    if(castling){
        switch(target_square){
            case g1:
                pop_bit(position.bitboards[wR], h1);
                set_bit(position.bitboards[wR], f1);
                position.castling &= 0b1100;
                break;
            case c1:
                pop_bit(position.bitboards[wR], a1);
                set_bit(position.bitboards[wR], d1);
                position.castling &= 0b1100;
                break;
            case g8:
                pop_bit(position.bitboards[bR], h8);
                set_bit(position.bitboards[bR], f8);
                position.castling &= 0b0011;
                break;
            case c8:
                pop_bit(position.bitboards[bR], a8);
                set_bit(position.bitboards[bR], d8);
                position.castling &= 0b0011;
                break;
        }
    }

    // Update castling rights based on piece movement
    if(piece == wK){
        position.castling &= 0b1100;
    }
    if(piece == bK){
        position.castling &= 0b0011;
    }
    if((piece == wR) && (source_square == h1)){
        position.castling &= 0b1110;
    }
    if((piece == wR) && (source_square == a1)){
        position.castling &= 0b1101;
    }
    if((piece == bR) && (source_square == h8)){
        position.castling &= 0b1011;
    }
    if((piece == bR) && (source_square == a8)){
        position.castling &= 0b0111;
    }

    position.compute_occupancies();

    // 2) Figure out whose king we're protecting *before* the flip
    int us = position.SideToMove;
    int our_king_sq = get_ls1b_index(position.bitboards[
        (us == White) ? wK : bK]);
    Color them = (us == White) ? Black : White;

    // 4) Only now do we switch sides
    position.SideToMove = them;
    return position;
}

void FilterLegalMoves(Position &position) {
    // Whose turn is it?
    Color us = position.SideToMove;
    Color them = (us == White ? Black : White);

    std::vector<Move> legal;
    legal.reserve(position.move_list.size());

    // For each pseudo‑legal move:
    for (Move mv : position.move_list) {
        // 1. Apply the move to a copy of the position
        Position copy = makemove(mv, position);
        // makemove should already flip SideToMove, update bitboards & occupancies

        // 2. Locate the king of the side that just moved
        Color mover = (copy.SideToMove == us ? them : us);
        uint8_t kingPiece = (mover == White ? wK : bK);
        int kingSq = get_ls1b_index(copy.bitboards[kingPiece]);

        // 3. If that king is *not* attacked by the new side‑to‑move, it’s legal
        if (!isSquareAttacked(kingSq, copy, copy.SideToMove)) {
            legal.push_back(mv);
        }
    }

    // 4. Replace the old list with just the legal moves
    position.move_list = std::move(legal);
}
//–– any other member-fn definitions … –
