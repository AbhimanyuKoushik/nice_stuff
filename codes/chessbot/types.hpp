#ifndef TYPES_HPP
#define TYPES_HPP

#include <cstdint>
#include <string>
#include <climits>
#include <unordered_map>

// Engine name
constexpr char NAME[] = "Lumin 1.0";
 
typedef uint64_t U64;

enum Color {
    White = 0,
    Black = 1,
    Both = 2
};

enum Piece {
    wP = 0, wN, wB, wR, wQ, wK,
    bP, bN, bB, bR, bQ, bK, Em, PieceCount
};

static const char *square_to_coordinates[64] = {
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
};

// Squares
enum Square : uint8_t {
    a8 = 0, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1,
    no_sq = 64
};

// ASCII and Unicode piece representations
static const char* ascii_pieces[PieceCount] = {
    "wP","wN","wB","wR","wQ","wK",
    "bP","bN","bB","bR","bQ","bK", "Em"
};

static const char* unicode_pieces[PieceCount] = {
    "♟︎","♞","♝","♜","♛","♚",
    "♙","♘","♗","♖","♕","♔", " "
};
// Map from string to piece code
static std::unordered_map<std::string,uint8_t> char_pieces = {
    {"wP", 0}, {"wN", 1}, {"wB", 2}, {"wR", 3}, {"wQ", 4}, {"wK", 5},
    {"bP", 6}, {"bN", 7}, {"bB", 8}, {"bR", 9}, {"bQ", 10}, {"bK", 11}, {"Em", 12}
};

// Starting position array
static const uint8_t startpos[64] = {
    bR,bN,bB,bQ,bK,bB,bN,bR,
    bP,bP,bP,bP,bP,bP,bP,bP,
    Em,Em,Em,Em,Em,Em,Em,Em,
    Em,Em,Em,Em,Em,Em,Em,Em,
    Em,Em,Em,Em,Em,Em,Em,Em,
    Em,Em,Em,Em,Em,Em,Em,Em,
    wP,wP,wP,wP,wP,wP,wP,wP,
    wR,wN,wB,wQ,wK,wB,wN,wR
};

// Castling rights bits
enum CastlingRights : uint8_t {
    wk = 1, wq = 2, bk = 4, bq = 8
};

// File exclusion masks
static constexpr U64 not_a_file  = 0xFEFEFEFEFEFEFEFEULL;
static constexpr U64 not_h_file  = 0x7F7F7F7F7F7F7F7FULL;
static constexpr U64 not_hg_file = 0x3F3F3F3F3F3F3F3FULL;
static constexpr U64 not_ab_file = 0xFCFCFCFCFCFCFCFCULL;

// Relevant occupancy bit counts
static constexpr int RookRelevantBits[64] = {
    12,11,11,11,11,11,11,12,
    11,10,10,10,10,10,10,11,
    11,10,10,10,10,10,10,11,
    11,10,10,10,10,10,10,11,
    11,10,10,10,10,10,10,11,
    11,10,10,10,10,10,10,11,
    11,10,10,10,10,10,10,11,
    12,11,11,11,11,11,11,12
};
static constexpr int BishopRelevantBits[64] = {
     6,5,5,5,5,5,5,6,
     5,5,5,5,5,5,5,5,
     5,5,7,7,7,7,5,5,
     5,5,7,9,9,7,5,5,
     5,5,7,9,9,7,5,5,
     5,5,7,7,7,7,5,5,
     5,5,5,5,5,5,5,5,
     6,5,5,5,5,5,5,6
};

// Magic numbers
static constexpr U64 RookMagics[64] = {
    0x8a80104000800020ULL,0x140002000100040ULL,0x2801880a0017001ULL,0x100081001000420ULL,
    0x200020010080420ULL,0x3001c0002010008ULL,0x8480008002000100ULL,0x2080088004402900ULL,
    0x800098204000ULL,0x2024401000200040ULL,0x100802000801000ULL,0x120800800801000ULL,
    0x208808088000400ULL,0x2802200800400ULL,0x2200800100020080ULL,0x801000060821100ULL,
    0x80044006422000ULL,0x100808020004000ULL,0x12108a0010204200ULL,0x140848010000802ULL,
    0x481828014002800ULL,0x8094004002004100ULL,0x4010040010010802ULL,0x20008806104ULL,
    0x100400080208000ULL,0x2040002120081000ULL,0x21200680100081ULL,0x20100080080080ULL,
    0x2000a00200410ULL,0x20080800400ULL,0x80088400100102ULL,0x80004600042881ULL,
    0x4040008040800020ULL,0x440003000200801ULL,0x4200011004500ULL,0x188020010100100ULL,
    0x14800401802800ULL,0x2080040080800200ULL,0x124080204001001ULL,0x200046502000484ULL,
    0x480400080088020ULL,0x1000422010034000ULL,0x30200100110040ULL,0x100021010009ULL,
    0x2002080100110004ULL,0x202008004008002ULL,0x20020004010100ULL,0x2048440040820001ULL,
    0x101002200408200ULL,0x40802000401080ULL,0x4008142004410100ULL,0x2060820c0120200ULL,
    0x1001004080100ULL,0x20c020080040080ULL,0x2935610830022400ULL,0x44440041009200ULL,
    0x280001040802101ULL,0x2100190040002085ULL,0x80c0084100102001ULL,0x4024081001000421ULL,
    0x20030a0244872ULL,0x12001008414402ULL,0x2006104900a0804ULL,0x1004081002402ULL
};
static constexpr U64 BishopMagics[64] = {
    0x40040844404084ULL,0x2004208a004208ULL,0x10190041080202ULL,0x108060845042010ULL,
    0x581104180800210ULL,0x2112080446200010ULL,0x1080820820060210ULL,0x3c0808410220200ULL,
    0x4050404440404ULL,0x21001420088ULL,0x24d0080801082102ULL,0x1020a0a020400ULL,
    0x40308200402ULL,0x4011002100800ULL,0x401484104104005ULL,0x801010402020200ULL,
    0x400210c3880100ULL,0x404022024108200ULL,0x810018200204102ULL,0x4002801a02003ULL,
    0x85040820080400ULL,0x810102c808880400ULL,0xe900410884800ULL,0x8002020480840102ULL,
    0x220200865090201ULL,0x2010100a02021202ULL,0x152048408022401ULL,0x20080002081110ULL,
    0x4001001021004000ULL,0x800040400a011002ULL,0xe4004081011002ULL,0x1c004001012080ULL,
    0x8004200962a00220ULL,0x8422100208500202ULL,0x2000402200300c08ULL,0x8646020080080080ULL,
    0x80020a0200100808ULL,0x2010004880111000ULL,0x623000a080011400ULL,0x42008c0340209202ULL,
    0x209188240001000ULL,0x400408a884001800ULL,0x110400a6080400ULL,0x1840060a44020800ULL,
    0x90080104000041ULL,0x201011000808101ULL,0x1a2208080504f080ULL,0x8012020600211212ULL,
    0x500861011240000ULL,0x180806108200800ULL,0x4000020e01040044ULL,0x300000261044000aULL,
    0x802241102020002ULL,0x20906061210001ULL,0x5a84841004010310ULL,0x4010801011c04ULL,
    0xa010109502200ULL,0x4a02012000ULL,0x500201010098b028ULL,0x8040002811040900ULL,
    0x28000010020204ULL,0x6000020202d0240ULL,0x8918844842082200ULL,0x4010011029020020ULL
};

//----------------------------------------------------------------------
// Material values for each Piece enum (wP…bK, Em last)
//----------------------------------------------------------------------
const int material_score[PieceCount] = {
    /* wP */ 100,
    /* wN */ 320,
    /* wB */ 330,
    /* wR */ 500,
    /* wQ */ 900,
    /* wK */ 20000,
    /* bP */ -100,
    /* bN */ -320,
    /* bB */ -330,
    /* bR */ -500,
    /* bQ */ -900,
    /* bK */ -20000,
    /* Em */ 0
};

const int queenEndgameWeight = 45;
const int rookEndgameWeight = 20;
const int bishopEndgameWeight = 10;
const int knightEndgameWeight = 10;

const int endgameStartWeight = 2 * rookEndgameWeight + 2 * bishopEndgameWeight + 2 * knightEndgameWeight + queenEndgameWeight;

//----------------------------------------------------------------------
// Piece-square tables (white's perspective; black will be flipped)
//----------------------------------------------------------------------
const int PAWN_TABLE[64] = {
    0,   0,   0,   0,   0,   0,   0,   0,
    50,  50,  50,  50,  50,  50,  50,  50,
    10,  10,  20,  30,  30,  20,  10,  10,
    5,   5,  10,  25,  25,  10,   5,   5,
    0,   0,   0,  20,  20,   0,   0,   0,
    5,  -5, -10,   0,   0, -10,  -5,   5,
    5,  10,  10, -20, -20,  10,  10,   5,
    0,   0,   0,   0,   0,   0,   0,   0
};

const int KNIGHT_TABLE[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50,
};

const int BISHOP_TABLE[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20,
};

const int ROOK_TABLE[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    0,  0,  0,  5,  5,  0,  0,  0
};

const int KING_TABLE_END[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20,
    -5,   0,   5,   5,   5,   5,   0,  -5,
    -10, -5,   20,  30,  30,  20,  -5, -10,
    -15, -10,  35,  45,  45,  35, -10, -15,
    -20, -15,  30,  40,  40,  30, -15, -20,
    -25, -20,  20,  25,  25,  20, -20, -25,
    -30, -25,   0,   0,   0,   0, -25, -30,
    -50, -30, -30, -30, -30, -30, -30, -50
};

const int KING_TABLE_START[64] = {
    -80, -70, -70, -70, -70, -70, -70, -80, 
    -60, -60, -60, -60, -60, -60, -60, -60, 
    -40, -50, -50, -60, -60, -50, -50, -40, 
    -30, -40, -40, -50, -50, -40, -40, -30, 
    -20, -30, -30, -40, -40, -30, -30, -20, 
    -10, -20, -20, -20, -20, -20, -20, -10, 
    20,  20,  -5,  -5,  -5,  -5,  20,  20, 
    20,  30,  10,   0,   0,  10,  30,  20
};

const int QUEEN_TABLE[64] =  {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
    -5,   0,  5,  5,  5,  5,  0, -5,
    0,    0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};

// Mirror positional score tables for opposite side
const int MirrorScore[64] = {
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8
};

#endif // TYPES_HPP