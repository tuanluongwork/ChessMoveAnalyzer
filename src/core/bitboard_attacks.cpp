#include "chess_analyzer/core/bitboard_attacks.h"
#include <array>

namespace chess {

// Pre-calculated attack tables
namespace {
    std::array<Bitboard, 64> knightAttacks;
    std::array<Bitboard, 64> kingAttacks;
    std::array<std::array<Bitboard, 64>, 2> pawnAttacks;
    
    struct AttackTableInit {
        AttackTableInit() {
            initKnightAttacks();
            initKingAttacks();
            initPawnAttacks();
        }
        
        void initKnightAttacks() {
            for (Square sq = A1; sq <= H8; ++sq) {
                Bitboard attacks = 0;
                int rank = rankOf(sq);
                int file = fileOf(sq);
                
                // All 8 knight moves
                const int moves[8][2] = {
                    {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
                    {1, -2}, {1, 2}, {2, -1}, {2, 1}
                };
                
                for (auto& m : moves) {
                    int newRank = rank + m[0];
                    int newFile = file + m[1];
                    if (newRank >= 0 && newRank < 8 && newFile >= 0 && newFile < 8) {
                        attacks |= squareBB(makeSquare(newFile, newRank));
                    }
                }
                
                knightAttacks[sq] = attacks;
            }
        }
        
        void initKingAttacks() {
            for (Square sq = A1; sq <= H8; ++sq) {
                Bitboard attacks = 0;
                int rank = rankOf(sq);
                int file = fileOf(sq);
                
                // All 8 king moves
                for (int dr = -1; dr <= 1; ++dr) {
                    for (int df = -1; df <= 1; ++df) {
                        if (dr == 0 && df == 0) continue;
                        
                        int newRank = rank + dr;
                        int newFile = file + df;
                        if (newRank >= 0 && newRank < 8 && newFile >= 0 && newFile < 8) {
                            attacks |= squareBB(makeSquare(newFile, newRank));
                        }
                    }
                }
                
                kingAttacks[sq] = attacks;
            }
        }
        
        void initPawnAttacks() {
            for (Square sq = A1; sq <= H8; ++sq) {
                int rank = rankOf(sq);
                int file = fileOf(sq);
                
                // White pawn attacks
                Bitboard wAttacks = 0;
                if (rank < 7) {  // Can attack forward
                    if (file > 0) wAttacks |= squareBB(makeSquare(file - 1, rank + 1));
                    if (file < 7) wAttacks |= squareBB(makeSquare(file + 1, rank + 1));
                }
                pawnAttacks[WHITE][sq] = wAttacks;
                
                // Black pawn attacks
                Bitboard bAttacks = 0;
                if (rank > 0) {  // Can attack backward
                    if (file > 0) bAttacks |= squareBB(makeSquare(file - 1, rank - 1));
                    if (file < 7) bAttacks |= squareBB(makeSquare(file + 1, rank - 1));
                }
                pawnAttacks[BLACK][sq] = bAttacks;
            }
        }
    } attackInit;
}

Bitboard pawnAttacksBB(Bitboard pawns, Color c) {
    if (c == WHITE) {
        return shift<Direction::NORTH_WEST>(pawns) | shift<Direction::NORTH_EAST>(pawns);
    } else {
        return shift<Direction::SOUTH_WEST>(pawns) | shift<Direction::SOUTH_EAST>(pawns);
    }
}

Bitboard knightAttacksBB(Square sq) {
    return knightAttacks[sq];
}

Bitboard bishopAttacksBB(Square sq, Bitboard occupied) {
    Bitboard attacks = 0;
    int rank = rankOf(sq);
    int file = fileOf(sq);
    
    // Northeast
    for (int r = rank + 1, f = file + 1; r < 8 && f < 8; ++r, ++f) {
        Square s = makeSquare(f, r);
        attacks |= squareBB(s);
        if (occupied & squareBB(s)) break;
    }
    
    // Southeast
    for (int r = rank - 1, f = file + 1; r >= 0 && f < 8; --r, ++f) {
        Square s = makeSquare(f, r);
        attacks |= squareBB(s);
        if (occupied & squareBB(s)) break;
    }
    
    // Southwest
    for (int r = rank - 1, f = file - 1; r >= 0 && f >= 0; --r, --f) {
        Square s = makeSquare(f, r);
        attacks |= squareBB(s);
        if (occupied & squareBB(s)) break;
    }
    
    // Northwest
    for (int r = rank + 1, f = file - 1; r < 8 && f >= 0; ++r, --f) {
        Square s = makeSquare(f, r);
        attacks |= squareBB(s);
        if (occupied & squareBB(s)) break;
    }
    
    return attacks;
}

Bitboard rookAttacksBB(Square sq, Bitboard occupied) {
    Bitboard attacks = 0;
    int rank = rankOf(sq);
    int file = fileOf(sq);
    
    // North
    for (int r = rank + 1; r < 8; ++r) {
        Square s = makeSquare(file, r);
        attacks |= squareBB(s);
        if (occupied & squareBB(s)) break;
    }
    
    // South
    for (int r = rank - 1; r >= 0; --r) {
        Square s = makeSquare(file, r);
        attacks |= squareBB(s);
        if (occupied & squareBB(s)) break;
    }
    
    // East
    for (int f = file + 1; f < 8; ++f) {
        Square s = makeSquare(f, rank);
        attacks |= squareBB(s);
        if (occupied & squareBB(s)) break;
    }
    
    // West
    for (int f = file - 1; f >= 0; --f) {
        Square s = makeSquare(f, rank);
        attacks |= squareBB(s);
        if (occupied & squareBB(s)) break;
    }
    
    return attacks;
}

Bitboard queenAttacksBB(Square sq, Bitboard occupied) {
    return bishopAttacksBB(sq, occupied) | rookAttacksBB(sq, occupied);
}

Bitboard kingAttacksBB(Square sq) {
    return kingAttacks[sq];
}

Bitboard getRay(Square from, Square to) {
    // Generate ray between two squares
    Bitboard ray = 0;
    int rankDiff = rankOf(to) - rankOf(from);
    int fileDiff = fileOf(to) - fileOf(from);
    
    if (rankDiff == 0 && fileDiff == 0) return 0;
    
    // Normalize direction
    int rankDir = (rankDiff > 0) ? 1 : (rankDiff < 0) ? -1 : 0;
    int fileDir = (fileDiff > 0) ? 1 : (fileDiff < 0) ? -1 : 0;
    
    // Check if valid ray (straight or diagonal)
    if (rankDir != 0 && fileDir != 0 && std::abs(rankDiff) != std::abs(fileDiff)) {
        return 0;  // Not a valid ray
    }
    
    // Generate ray
    int rank = rankOf(from) + rankDir;
    int file = fileOf(from) + fileDir;
    
    while (rank >= 0 && rank < 8 && file >= 0 && file < 8) {
        Square sq = makeSquare(file, rank);
        ray |= squareBB(sq);
        
        if (sq == to) break;
        
        rank += rankDir;
        file += fileDir;
    }
    
    return ray;
}

Bitboard getBetween(Square from, Square to) {
    // Get squares between two squares (excluding endpoints)
    Bitboard ray = getRay(from, to);
    return ray & ~squareBB(to);
}

// Placeholder for magic bitboard data
MagicEntry rookMagics[64];
MagicEntry bishopMagics[64];

void initializeMagicBitboards() {
    // TODO: Implement magic bitboard initialization
    // This would involve finding magic numbers and building attack tables
    // For now, we use the classical approach in the attack functions above
}

} // namespace chess 