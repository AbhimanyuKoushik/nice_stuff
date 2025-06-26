#include "position.hpp"
#include "movegen.hpp"    // to get the implementations of generate_* 
#include "attacks.hpp"    // if you use them in Board:: methods
#include "nonmagic.hpp"
#include "magic.hpp"
#include "types.hpp"
#include "movedef.hpp"
#include <iostream>
#include <sstream>

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
            position.arrangement[sq] = static_cast<uint8_t>(p);
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
    
    position.HalfMovesMade = fullmoveNumber * 2 - (position.SideToMove == White ? 1 : 0);

    position.update_bitboards();

    return position;
}

//–– print ––
void Position::print() const {
    std::cout << std::endl;
    for (int rank = 0; rank < 8; ++rank) {
        std::cout << 8 - rank << " ";
        for (int file = 0; file < 8; ++file) {
            uint8_t sq = arrangement[rank*8 + file];
            switch (sq) {
                case wR: std::printf("♜ "); break;
                case wN: std::printf("♞ "); break;
                case wB: std::printf("♝ "); break;
                case wQ: std::printf("♛ "); break;
                case wK: std::printf("♚ "); break;
                case wP: std::printf("♟︎ "); break;
                case bR: std::printf("♖ "); break;
                case bN: std::printf("♘ "); break;
                case bB: std::printf("♗ "); break;
                case bQ: std::printf("♕ "); break;
                case bK: std::printf("♔ "); break;
                case bP: std::printf("♙ "); break;
                default: std::printf(". "); break;
            }
        }
        std::printf("\n");
    }
    std::cout << "  a b c d e f g h" << std::endl;
    std::cout << std::endl;
    std::cout << "Side to move: " << (SideToMove == Black ? "Black" : "White") << std::endl;
    std::cout << "Enpassant Square: " << (enpassant == no_sq ? "No Square" : square_to_coordinates[enpassant]) << std::endl;
    std::cout << "Castling: "
            << ((castling & wk) ? "White Kingside " : "- ")
            << ((castling & wq) ? "White Queenside " : "- ")
            << ((castling & bk) ? "Black Kingside " : "- ")
            << ((castling & bq) ? "Black Queenside " : "- ")
            << std::endl;
    std::cout << "Moves Made: " << int(HalfMovesMade/2) << std::endl;
    std::cout << std::endl;
}

//–– generate_moves ––
void Position::generate_moves() {
    // call into your free functions:
    generate_pawn_moves(SideToMove, *this, move_list);
    generate_king_moves(SideToMove, *this, move_list);
    generate_knight_moves(SideToMove, *this, move_list);
    generate_bishop_moves(SideToMove, *this, move_list);
    generate_rook_moves(SideToMove, *this, move_list);
    generate_queen_moves(SideToMove, *this, move_list);
    // plus sliding pieces, etc.
}

void Position::init() {
    castling   = wk | wq | bk | bq;      // All castling rights initially
    FiftyMove  = false;
    SideToMove = White;

    HalfMovesMade = 0;

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

    // Fill the array-based board for print() and other routines
    std::memcpy(arrangement, startpos, sizeof(arrangement));

    // If you have a compute_occupancies() helper, call it now:
    compute_occupancies();
}

void Position::update_arrangement() {
    // Clear the array-based board
    for (int sq = 0; sq < 64; ++sq) {
        arrangement[sq] = Em;
    }
    // For each piece type, set squares from its bitboard
    for (int p = wP; p <= bK; ++p) {
        U64 bb = bitboards[p];
        while (bb) {
            int sq = get_ls1b_index(bb);   // index of least‑significant 1
            arrangement[sq] = static_cast<uint8_t>(p);
            pop_bit(bb, sq);                  // pop that bit
        }
    }
    compute_occupancies();
}

void Position::update_bitboards() {
    // Clear the array-based board
    for (int i = 0; i < 12; i++) {
        bitboards[i] = 0ULL;
    }
    // For each piece type, set squares from its bitboard
    for (int sq = 0; sq < 64; sq++) {
        if(arrangement[sq] != Em) set_bit(bitboards[arrangement[sq]], sq);
    }
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

void Position::makeMove(int from, int to) {
    arrangement[to]   = arrangement[from];
    arrangement[from] = Em;
    HalfMovesMade++;
}

void Position::emptyBoard() {
    for (uint8_t rank = 0; rank < 8; ++rank) {
        for (uint8_t file = 0; file < 8; ++file) {
            arrangement[rank*8 + file] = Em;
        }
    }
    for(uint8_t i = 0; i < 12; i++){
        bitboards[i] = 0ULL;
    }
    occupancies[0] = 0ULL;
    occupancies[1] = 0ULL;
    occupancies[2] = 0ULL;
}
//–– any other member-fn definitions … –
