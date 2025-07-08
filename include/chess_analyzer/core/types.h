#pragma once

#include <cstdint>
#include <string>

namespace chess {

// Forward declarations
class Move;

// Basic type aliases
using Bitboard = uint64_t;
using Square = int;
using Piece = int;

// Square constants
constexpr Square NO_SQUARE = -1;
constexpr Square A1 = 0, B1 = 1, C1 = 2, D1 = 3, E1 = 4, F1 = 5, G1 = 6, H1 = 7;
constexpr Square A8 = 56, B8 = 57, C8 = 58, D8 = 59, E8 = 60, F8 = 61, G8 = 62, H8 = 63;

// Colors
enum Color : uint8_t {
    WHITE = 0,
    BLACK = 1,
    NO_COLOR = 2
};

// Piece types
enum PieceType : uint8_t {
    PAWN = 0,
    KNIGHT = 1,
    BISHOP = 2,
    ROOK = 3,
    QUEEN = 4,
    KING = 5,
    NO_PIECE_TYPE = 6
};

// Combined piece representation
enum Pieces : uint8_t {
    NO_PIECE = 0,
    W_PAWN = 1, W_KNIGHT = 2, W_BISHOP = 3, W_ROOK = 4, W_QUEEN = 5, W_KING = 6,
    B_PAWN = 9, B_KNIGHT = 10, B_BISHOP = 11, B_ROOK = 12, B_QUEEN = 13, B_KING = 14
};

// Castling rights flags
enum CastlingRights : uint8_t {
    NO_CASTLING = 0,
    WHITE_OO = 1,
    WHITE_OOO = 2,
    BLACK_OO = 4,
    BLACK_OOO = 8,
    ALL_CASTLING = WHITE_OO | WHITE_OOO | BLACK_OO | BLACK_OOO
};

// Move types
enum MoveType : uint8_t {
    NORMAL = 0,
    PROMOTION = 1,
    EN_PASSANT = 2,
    CASTLING = 3
};

// Promotion piece types
enum PromotionType : uint8_t {
    PROMOTE_TO_QUEEN = 0,
    PROMOTE_TO_ROOK = 1,
    PROMOTE_TO_BISHOP = 2,
    PROMOTE_TO_KNIGHT = 3
};

// Direction offsets for move generation
namespace Direction {
    constexpr int NORTH = 8;
    constexpr int SOUTH = -8;
    constexpr int EAST = 1;
    constexpr int WEST = -1;
    constexpr int NORTH_EAST = 9;
    constexpr int NORTH_WEST = 7;
    constexpr int SOUTH_EAST = -7;
    constexpr int SOUTH_WEST = -9;
}

// Utility functions
inline Color operator~(Color c) {
    return static_cast<Color>(c ^ 1);
}

inline Square makeSquare(int file, int rank) {
    return rank * 8 + file;
}

inline int fileOf(Square sq) {
    return sq & 7;
}

inline int rankOf(Square sq) {
    return sq >> 3;
}

inline std::string squareToString(Square sq) {
    if (sq == NO_SQUARE) return "-";
    return std::string{static_cast<char>('a' + fileOf(sq)), 
                      static_cast<char>('1' + rankOf(sq))};
}

inline Square stringToSquare(const std::string& str) {
    if (str.length() != 2) return NO_SQUARE;
    int file = str[0] - 'a';
    int rank = str[1] - '1';
    if (file < 0 || file > 7 || rank < 0 || rank > 7) return NO_SQUARE;
    return makeSquare(file, rank);
}

// Bitboard utilities
inline Bitboard squareBB(Square sq) {
    return 1ULL << sq;
}

inline bool moreThanOne(Bitboard b) {
    return b & (b - 1);
}

inline Square lsb(Bitboard b) {
    return __builtin_ctzll(b);
}

inline Square popLsb(Bitboard& b) {
    Square sq = lsb(b);
    b &= b - 1;
    return sq;
}

inline int popcount(Bitboard b) {
    return __builtin_popcountll(b);
}

// Piece utilities
inline PieceType typeOf(Piece p) {
    return static_cast<PieceType>((p - 1) % 6);
}

inline Color colorOf(Piece p) {
    return p < B_PAWN ? WHITE : BLACK;
}

inline Piece makePiece(Color c, PieceType pt) {
    return static_cast<Piece>((c == WHITE ? W_PAWN : B_PAWN) + pt);
}

} // namespace chess 