#include "chess_analyzer/evaluation/evaluator.h"
#include "chess_analyzer/core/bitboard_attacks.h"
#include <algorithm>
#include <cmath>
#include <memory>

namespace chess {

// Piece-square tables for positional evaluation
namespace {
    // Pawn positional values (from white's perspective)
    constexpr int pawnTable[64] = {
         0,  0,  0,  0,  0,  0,  0,  0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
         5,  5, 10, 25, 25, 10,  5,  5,
         0,  0,  0, 20, 20,  0,  0,  0,
         5, -5,-10,  0,  0,-10, -5,  5,
         5, 10, 10,-20,-20, 10, 10,  5,
         0,  0,  0,  0,  0,  0,  0,  0
    };
    
    // Knight positional values
    constexpr int knightTable[64] = {
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50
    };
    
    // Bishop positional values
    constexpr int bishopTable[64] = {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20
    };
    
    // Rook positional values
    constexpr int rookTable[64] = {
         0,  0,  0,  0,  0,  0,  0,  0,
         5, 10, 10, 10, 10, 10, 10,  5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
         0,  0,  0,  5,  5,  0,  0,  0
    };
    
    // Queen positional values
    constexpr int queenTable[64] = {
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5,  5,  5,  5,  0,-10,
         -5,  0,  5,  5,  5,  5,  0, -5,
          0,  0,  5,  5,  5,  5,  0, -5,
        -10,  5,  5,  5,  5,  5,  0,-10,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20
    };
    
    // King positional values (middlegame)
    constexpr int kingMiddlegameTable[64] = {
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -10,-20,-20,-20,-20,-20,-20,-10,
         20, 20,  0,  0,  0,  0, 20, 20,
         20, 30, 10,  0,  0, 10, 30, 20
    };
    
    // King positional values (endgame)
    constexpr int kingEndgameTable[64] = {
        -50,-40,-30,-20,-20,-30,-40,-50,
        -30,-20,-10,  0,  0,-10,-20,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-30,  0,  0,  0,  0,-30,-30,
        -50,-30,-30,-30,-30,-30,-30,-50
    };
    
    int getPieceSquareValue(PieceType piece, Square sq, Color color, bool endgame) {
        // Flip square for black pieces
        if (color == BLACK) {
            sq = sq ^ 56;  // Flip vertically
        }
        
        switch (piece) {
            case PAWN:   return pawnTable[sq];
            case KNIGHT: return knightTable[sq];
            case BISHOP: return bishopTable[sq];
            case ROOK:   return rookTable[sq];
            case QUEEN:  return queenTable[sq];
            case KING:   return endgame ? kingEndgameTable[sq] : kingMiddlegameTable[sq];
            default:     return 0;
        }
    }
}

class Evaluator::Impl {
public:
    int evaluate(const Position& pos) const {
        int score = 0;
        
        // Material balance
        score += getMaterialBalance(pos);
        
        // Piece-square tables
        score += getPieceSquareScore(pos);
        
        // Pawn structure
        score += evaluatePawnStructure(pos);
        
        // Piece mobility
        score += evaluateMobility(pos);
        
        // King safety
        score += evaluateKingSafety(pos, WHITE) - evaluateKingSafety(pos, BLACK);
        
        // Center control
        score += evaluateCenterControl(pos);
        
        // Return score from perspective of side to move
        return pos.getSideToMove() == WHITE ? score : -score;
    }
    
    int getMaterialBalance(const Position& pos) const {
        int material = 0;
        
        // Count material for each piece type
        material += (popcount(pos.getPieceBitboard(PAWN, WHITE)) - 
                    popcount(pos.getPieceBitboard(PAWN, BLACK))) * PieceValue::PAWN;
        material += (popcount(pos.getPieceBitboard(KNIGHT, WHITE)) - 
                    popcount(pos.getPieceBitboard(KNIGHT, BLACK))) * PieceValue::KNIGHT;
        material += (popcount(pos.getPieceBitboard(BISHOP, WHITE)) - 
                    popcount(pos.getPieceBitboard(BISHOP, BLACK))) * PieceValue::BISHOP;
        material += (popcount(pos.getPieceBitboard(ROOK, WHITE)) - 
                    popcount(pos.getPieceBitboard(ROOK, BLACK))) * PieceValue::ROOK;
        material += (popcount(pos.getPieceBitboard(QUEEN, WHITE)) - 
                    popcount(pos.getPieceBitboard(QUEEN, BLACK))) * PieceValue::QUEEN;
        
        return material;
    }
    
    int getPieceSquareScore(const Position& pos) const {
        int score = 0;
        bool endgame = isEndgame(pos);
        
        for (Color c : {WHITE, BLACK}) {
            int colorScore = 0;
            
            for (PieceType pt = PAWN; pt <= KING; pt = static_cast<PieceType>(pt + 1)) {
                Bitboard pieces = pos.getPieceBitboard(pt, c);
                while (pieces) {
                    Square sq = popLsb(pieces);
                    colorScore += getPieceSquareValue(pt, sq, c, endgame);
                }
            }
            
            score += (c == WHITE) ? colorScore : -colorScore;
        }
        
        return score;
    }
    
    int evaluatePawnStructure(const Position& pos) const {
        int score = 0;
        
        Bitboard whitePawns = pos.getPieceBitboard(PAWN, WHITE);
        Bitboard blackPawns = pos.getPieceBitboard(PAWN, BLACK);
        
        // Doubled pawns penalty
        for (int file = 0; file < 8; ++file) {
            Bitboard fileMask = FILE_A << file;
            int whitePawnsOnFile = popcount(whitePawns & fileMask);
            int blackPawnsOnFile = popcount(blackPawns & fileMask);
            
            if (whitePawnsOnFile > 1) score -= 10 * (whitePawnsOnFile - 1);
            if (blackPawnsOnFile > 1) score += 10 * (blackPawnsOnFile - 1);
        }
        
        // Isolated pawns penalty
        for (int file = 0; file < 8; ++file) {
            Bitboard fileMask = FILE_A << file;
            Bitboard adjacentFiles = 0;
            if (file > 0) adjacentFiles |= FILE_A << (file - 1);
            if (file < 7) adjacentFiles |= FILE_A << (file + 1);
            
            if ((whitePawns & fileMask) && !(whitePawns & adjacentFiles)) {
                score -= 15;  // Isolated pawn penalty
            }
            if ((blackPawns & fileMask) && !(blackPawns & adjacentFiles)) {
                score += 15;
            }
        }
        
        // Passed pawns bonus
        Bitboard whitePassed = getPassedPawns(whitePawns, blackPawns, WHITE);
        Bitboard blackPassed = getPassedPawns(blackPawns, whitePawns, BLACK);
        
        while (whitePassed) {
            Square sq = popLsb(whitePassed);
            int rank = rankOf(sq);
            score += 10 + rank * rank * 5;  // Bonus increases with advancement
        }
        
        while (blackPassed) {
            Square sq = popLsb(blackPassed);
            int rank = 7 - rankOf(sq);
            score -= 10 + rank * rank * 5;
        }
        
        return score;
    }
    
    int evaluateMobility(const Position& pos) const {
        // Simplified mobility evaluation
        // Count number of squares each piece can move to
        int score = 0;
        Bitboard occupied = pos.getOccupiedBitboard();
        
        // Knight mobility
        Bitboard whiteKnights = pos.getPieceBitboard(KNIGHT, WHITE);
        Bitboard blackKnights = pos.getPieceBitboard(KNIGHT, BLACK);
        
        while (whiteKnights) {
            Square sq = popLsb(whiteKnights);
            score += popcount(knightAttacksBB(sq) & ~pos.getColorBitboard(WHITE)) * 4;
        }
        
        while (blackKnights) {
            Square sq = popLsb(blackKnights);
            score -= popcount(knightAttacksBB(sq) & ~pos.getColorBitboard(BLACK)) * 4;
        }
        
        // Bishop mobility
        Bitboard whiteBishops = pos.getPieceBitboard(BISHOP, WHITE);
        Bitboard blackBishops = pos.getPieceBitboard(BISHOP, BLACK);
        
        while (whiteBishops) {
            Square sq = popLsb(whiteBishops);
            score += popcount(bishopAttacksBB(sq, occupied) & ~pos.getColorBitboard(WHITE)) * 3;
        }
        
        while (blackBishops) {
            Square sq = popLsb(blackBishops);
            score -= popcount(bishopAttacksBB(sq, occupied) & ~pos.getColorBitboard(BLACK)) * 3;
        }
        
        return score;
    }
    
    int evaluateKingSafety(const Position& pos, Color color) const {
        Square kingSquare = lsb(pos.getPieceBitboard(KING, color));
        int safety = 0;
        
        // Penalty for exposed king
        Bitboard kingZone = kingAttacksBB(kingSquare);
        Bitboard ourPawns = pos.getPieceBitboard(PAWN, color);
        
        // Count pawn shield
        int pawnShield = popcount(kingZone & ourPawns);
        safety += pawnShield * 10;
        
        // Penalty for open files near king
        int kingFile = fileOf(kingSquare);
        for (int f = std::max(0, kingFile - 1); f <= std::min(7, kingFile + 1); ++f) {
            Bitboard fileMask = FILE_A << f;
            if (!(ourPawns & fileMask)) {
                safety -= 20;  // Open file penalty
            }
        }
        
        return safety;
    }
    
    int evaluateCenterControl(const Position& pos) const {
        int score = 0;
        
        // Control of center squares
        Bitboard whiteControl = 0;
        Bitboard blackControl = 0;
        
        // Simplified - just count pieces attacking center
        const Square centerSquares[] = {
            makeSquare(3, 3), // D4
            makeSquare(4, 3), // E4
            makeSquare(3, 4), // D5
            makeSquare(4, 4)  // E5
        };
        for (Square sq : centerSquares) {
            if (pos.isSquareAttacked(sq, WHITE)) whiteControl |= squareBB(sq);
            if (pos.isSquareAttacked(sq, BLACK)) blackControl |= squareBB(sq);
        }
        
        score += popcount(whiteControl) * 10;
        score -= popcount(blackControl) * 10;
        
        // Pieces on center squares
        Bitboard centerBitboard = squareBB(makeSquare(3, 3)) | squareBB(makeSquare(4, 3)) |
                                 squareBB(makeSquare(3, 4)) | squareBB(makeSquare(4, 4));
        score += popcount(centerBitboard & pos.getColorBitboard(WHITE)) * 15;
        score -= popcount(centerBitboard & pos.getColorBitboard(BLACK)) * 15;
        
        return score;
    }
    
    bool isEndgame(const Position& pos) const {
        // Simple endgame detection based on material
        int totalMaterial = 0;
        
        totalMaterial += popcount(pos.getPieceBitboard(QUEEN, WHITE)) * PieceValue::QUEEN;
        totalMaterial += popcount(pos.getPieceBitboard(QUEEN, BLACK)) * PieceValue::QUEEN;
        totalMaterial += popcount(pos.getPieceBitboard(ROOK, WHITE)) * PieceValue::ROOK;
        totalMaterial += popcount(pos.getPieceBitboard(ROOK, BLACK)) * PieceValue::ROOK;
        
        return totalMaterial < 2000;  // Less than 2 rooks + 2 queens
    }
    
private:
    Bitboard getPassedPawns(Bitboard ourPawns, Bitboard theirPawns, Color us) const {
        Bitboard passed = 0;
        Bitboard pawns = ourPawns;
        
        while (pawns) {
            Square sq = popLsb(pawns);
            int file = fileOf(sq);
            int rank = rankOf(sq);
            
            // Check if pawn is passed
            Bitboard frontSpan = 0;
            if (us == WHITE) {
                for (int r = rank + 1; r < 8; ++r) {
                    if (file > 0) frontSpan |= squareBB(makeSquare(file - 1, r));
                    frontSpan |= squareBB(makeSquare(file, r));
                    if (file < 7) frontSpan |= squareBB(makeSquare(file + 1, r));
                }
            } else {
                for (int r = rank - 1; r >= 0; --r) {
                    if (file > 0) frontSpan |= squareBB(makeSquare(file - 1, r));
                    frontSpan |= squareBB(makeSquare(file, r));
                    if (file < 7) frontSpan |= squareBB(makeSquare(file + 1, r));
                }
            }
            
            if (!(frontSpan & theirPawns)) {
                passed |= squareBB(sq);
            }
        }
        
        return passed;
    }
};

Evaluator::Evaluator() : pImpl(std::make_unique<Impl>()) {}
Evaluator::~Evaluator() = default;

int Evaluator::evaluate(const Position& position) const {
    return pImpl->evaluate(position);
}

int Evaluator::getMaterialBalance(const Position& position) const {
    return pImpl->getMaterialBalance(position);
}

int Evaluator::evaluatePawnStructure(const Position& position) const {
    return pImpl->evaluatePawnStructure(position);
}

int Evaluator::evaluateKingSafety(const Position& position, Color color) const {
    return pImpl->evaluateKingSafety(position, color);
}

int Evaluator::evaluateMobility(const Position& position) const {
    return pImpl->evaluateMobility(position);
}

int Evaluator::evaluateCenterControl(const Position& position) const {
    return pImpl->evaluateCenterControl(position);
}

bool Evaluator::isEndgame(const Position& position) const {
    return pImpl->isEndgame(position);
}

void Evaluator::setParameter(const std::string& paramName, int value) {
    // TODO: Implement parameter tuning
}

} // namespace chess 