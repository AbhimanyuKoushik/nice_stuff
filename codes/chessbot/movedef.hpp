#pragma once

#include <iostream>
#include <cstdint>
#include <vector>

typedef int Move;

/*
        Binary Move bits                                            hexadecimal constants

0000 0000 0000 0000 0011 1111 source square    -> first 6 bits      0x3f
0000 0000 0000 1111 1100 0000 target square    -> next 6 bits       0xfc0
0000 0000 1111 0000 0000 0000 piece            -> next 4 bits       0xf000
0000 1111 0000 0000 0000 0000 promoted piece   -> next 4 bits       0xf0000
0001 0000 0000 0000 0000 0000 Capture flag     -> next bit          0x100000
0010 0000 0000 0000 0000 0000 double push flag -> next bit          0x200000
0100 0000 0000 0000 0000 0000 enpassant flag   -> next bit          0x400000
1000 0000 0000 0000 0000 0000 Castling flag    -> next bit          0x800000

*/

#define encode_move(source, target, piece, promoted, capture, doublepush, enpassant, castling) \
((source) | (target << 6) | (piece << 12) | (promoted << 16) | (capture << 20) | (doublepush << 21) | (enpassant << 22) | (castling << 23)) 

#define get_move_source(move)    (move & 0x3f)
#define get_move_target(move)    ((move & 0xfc0) >> 6)
#define get_move_piece(move)     ((move & 0xf000) >> 12)
#define get_move_promoted(move)  ((move & 0xf0000) >> 16)
#define get_move_capture(move)   ((move & 0x100000) >> 20)
#define get_move_double(move)    ((move & 0x200000) >> 21)
#define get_move_enpassant(move) ((move & 0x400000) >> 22)
#define get_move_castling(move)  ((move & 0x800000) >> 23)

struct Moves {
    std::vector<Move> moves;

    void print_move(Move mv) const;
    void print_move_list() const;
    void add_move(Move mv);
};