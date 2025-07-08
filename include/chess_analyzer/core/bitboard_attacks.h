#pragma once

#include "chess_analyzer/core/types.h"

namespace chess {

/**
 * @brief Bitboard attack generation utilities
 * 
 * This file contains optimized attack generation functions using bitboards
 * for high-performance move generation.
 */

// File and rank masks
constexpr Bitboard FILE_A = 0x0101010101010101ULL;
constexpr Bitboard FILE_B = 0x0202020202020202ULL;
constexpr Bitboard FILE_C = 0x0404040404040404ULL;
constexpr Bitboard FILE_D = 0x0808080808080808ULL;
constexpr Bitboard FILE_E = 0x1010101010101010ULL;
constexpr Bitboard FILE_F = 0x2020202020202020ULL;
constexpr Bitboard FILE_G = 0x4040404040404040ULL;
constexpr Bitboard FILE_H = 0x8080808080808080ULL;

constexpr Bitboard RANK_1 = 0x00000000000000FFULL;
constexpr Bitboard RANK_2 = 0x000000000000FF00ULL;
constexpr Bitboard RANK_3 = 0x0000000000FF0000ULL;
constexpr Bitboard RANK_4 = 0x00000000FF000000ULL;
constexpr Bitboard RANK_5 = 0x000000FF00000000ULL;
constexpr Bitboard RANK_6 = 0x0000FF0000000000ULL;
constexpr Bitboard RANK_7 = 0x00FF000000000000ULL;
constexpr Bitboard RANK_8 = 0xFF00000000000000ULL;

// Center squares
constexpr Bitboard CENTER = 0x0000001818000000ULL;  // d4, e4, d5, e5
constexpr Bitboard EXTENDED_CENTER = 0x00003C3C3C3C0000ULL;  // c3-f6 square

// Shift operations with boundary handling
template<int Direction>
constexpr Bitboard shift(Bitboard b) {
    return Direction == Direction::NORTH      ? b << 8 :
           Direction == Direction::SOUTH      ? b >> 8 :
           Direction == Direction::EAST       ? (b & ~FILE_H) << 1 :
           Direction == Direction::WEST       ? (b & ~FILE_A) >> 1 :
           Direction == Direction::NORTH_EAST ? (b & ~FILE_H) << 9 :
           Direction == Direction::NORTH_WEST ? (b & ~FILE_A) << 7 :
           Direction == Direction::SOUTH_EAST ? (b & ~FILE_H) >> 7 :
           Direction == Direction::SOUTH_WEST ? (b & ~FILE_A) >> 9 : 0;
}

// Ray generation for sliding pieces
Bitboard getRay(Square from, Square to);
Bitboard getBetween(Square from, Square to);

// Attack generation helpers
Bitboard pawnAttacksBB(Bitboard pawns, Color c);
Bitboard knightAttacksBB(Square sq);
Bitboard bishopAttacksBB(Square sq, Bitboard occupied);
Bitboard rookAttacksBB(Square sq, Bitboard occupied);
Bitboard queenAttacksBB(Square sq, Bitboard occupied);
Bitboard kingAttacksBB(Square sq);

// Magic bitboard structures (for future optimization)
struct MagicEntry {
    Bitboard mask;
    Bitboard magic;
    Bitboard* attacks;
    unsigned shift;
    
    unsigned index(Bitboard occupied) const {
        return unsigned(((occupied & mask) * magic) >> shift);
    }
};

extern MagicEntry rookMagics[64];
extern MagicEntry bishopMagics[64];

// Initialize magic bitboards (called once at startup)
void initializeMagicBitboards();

} // namespace chess 