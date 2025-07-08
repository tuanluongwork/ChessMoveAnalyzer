#include "chess_analyzer/core/move_generator.h"
#include "chess_analyzer/core/bitboard_attacks.h"
#include <algorithm>

namespace chess {

// Pre-calculated attack tables for non-sliding pieces
namespace {
    Bitboard pawnAttacks[2][64];
    Bitboard knightAttacks[64];
    Bitboard kingAttacks[64];
    
    // Magic bitboards for sliding pieces (simplified version)
    struct Magic {
        Bitboard mask;
        Bitboard magic;
        Bitboard* attacks;
        unsigned shift;
    };
    
    Magic rookMagics[64];
    Magic bishopMagics[64];
    
    // Initialize attack tables at startup
    struct AttackTableInitializer {
        AttackTableInitializer() {
            initializePawnAttacks();
            initializeKnightAttacks();
            initializeKingAttacks();
            // Magic bitboard initialization would go here
        }
        
        void initializePawnAttacks() {
            for (Square sq = A1; sq <= H8; ++sq) {
                Bitboard sqBB = squareBB(sq);
                int file = fileOf(sq);
                
                // White pawn attacks
                pawnAttacks[WHITE][sq] = 0;
                if (file > 0) pawnAttacks[WHITE][sq] |= sqBB << 7;
                if (file < 7) pawnAttacks[WHITE][sq] |= sqBB << 9;
                
                // Black pawn attacks
                pawnAttacks[BLACK][sq] = 0;
                if (file > 0) pawnAttacks[BLACK][sq] |= sqBB >> 9;
                if (file < 7) pawnAttacks[BLACK][sq] |= sqBB >> 7;
            }
        }
        
        void initializeKnightAttacks() {
            const int knightMoves[8][2] = {
                {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
                {1, -2}, {1, 2}, {2, -1}, {2, 1}
            };
            
            for (Square sq = A1; sq <= H8; ++sq) {
                Bitboard attacks = 0;
                int rank = rankOf(sq);
                int file = fileOf(sq);
                
                for (auto& move : knightMoves) {
                    int newRank = rank + move[0];
                    int newFile = file + move[1];
                    if (newRank >= 0 && newRank < 8 && newFile >= 0 && newFile < 8) {
                        attacks |= squareBB(makeSquare(newFile, newRank));
                    }
                }
                knightAttacks[sq] = attacks;
            }
        }
        
        void initializeKingAttacks() {
            const int kingMoves[8][2] = {
                {-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
                {0, 1}, {1, -1}, {1, 0}, {1, 1}
            };
            
            for (Square sq = A1; sq <= H8; ++sq) {
                Bitboard attacks = 0;
                int rank = rankOf(sq);
                int file = fileOf(sq);
                
                for (auto& move : kingMoves) {
                    int newRank = rank + move[0];
                    int newFile = file + move[1];
                    if (newRank >= 0 && newRank < 8 && newFile >= 0 && newFile < 8) {
                        attacks |= squareBB(makeSquare(newFile, newRank));
                    }
                }
                kingAttacks[sq] = attacks;
            }
        }
    } attackTableInit;
    
    // Ray attacks for sliding pieces (simplified)
    Bitboard getRookAttacks(Square sq, Bitboard occupied) {
        Bitboard attacks = 0;
        int rank = rankOf(sq);
        int file = fileOf(sq);
        
        // North
        for (int r = rank + 1; r < 8; ++r) {
            attacks |= squareBB(makeSquare(file, r));
            if (occupied & squareBB(makeSquare(file, r))) break;
        }
        
        // South
        for (int r = rank - 1; r >= 0; --r) {
            attacks |= squareBB(makeSquare(file, r));
            if (occupied & squareBB(makeSquare(file, r))) break;
        }
        
        // East
        for (int f = file + 1; f < 8; ++f) {
            attacks |= squareBB(makeSquare(f, rank));
            if (occupied & squareBB(makeSquare(f, rank))) break;
        }
        
        // West
        for (int f = file - 1; f >= 0; --f) {
            attacks |= squareBB(makeSquare(f, rank));
            if (occupied & squareBB(makeSquare(f, rank))) break;
        }
        
        return attacks;
    }
    
    Bitboard getBishopAttacks(Square sq, Bitboard occupied) {
        Bitboard attacks = 0;
        int rank = rankOf(sq);
        int file = fileOf(sq);
        
        // Northeast
        for (int r = rank + 1, f = file + 1; r < 8 && f < 8; ++r, ++f) {
            attacks |= squareBB(makeSquare(f, r));
            if (occupied & squareBB(makeSquare(f, r))) break;
        }
        
        // Southeast
        for (int r = rank - 1, f = file + 1; r >= 0 && f < 8; --r, ++f) {
            attacks |= squareBB(makeSquare(f, r));
            if (occupied & squareBB(makeSquare(f, r))) break;
        }
        
        // Southwest
        for (int r = rank - 1, f = file - 1; r >= 0 && f >= 0; --r, --f) {
            attacks |= squareBB(makeSquare(f, r));
            if (occupied & squareBB(makeSquare(f, r))) break;
        }
        
        // Northwest
        for (int r = rank + 1, f = file - 1; r < 8 && f >= 0; ++r, --f) {
            attacks |= squareBB(makeSquare(f, r));
            if (occupied & squareBB(makeSquare(f, r))) break;
        }
        
        return attacks;
    }
}

class MoveGenerator::Impl {
public:
    std::vector<Move> generateAllMoves(const Position& pos) const {
        std::vector<Move> moves;
        moves.reserve(256);  // Typical upper bound
        
        Color us = pos.getSideToMove();
        Bitboard ourPieces = pos.getColorBitboard(us);
        Bitboard theirPieces = pos.getColorBitboard(~us);
        Bitboard occupied = ourPieces | theirPieces;
        
        // Generate pawn moves
        generatePawnMoves(pos, moves, us, ourPieces, theirPieces, occupied);
        
        // Generate knight moves
        generatePieceMoves<KNIGHT>(pos, moves, us, ourPieces, occupied);
        
        // Generate bishop moves
        generatePieceMoves<BISHOP>(pos, moves, us, ourPieces, occupied);
        
        // Generate rook moves
        generatePieceMoves<ROOK>(pos, moves, us, ourPieces, occupied);
        
        // Generate queen moves
        generatePieceMoves<QUEEN>(pos, moves, us, ourPieces, occupied);
        
        // Generate king moves
        generateKingMoves(pos, moves, us, ourPieces, occupied);
        
        // Generate castling moves
        generateCastlingMoves(pos, moves, us, occupied);
        
        return moves;
    }
    
private:
    void generatePawnMoves(const Position& pos, std::vector<Move>& moves,
                          Color us, Bitboard ourPieces, Bitboard theirPieces, 
                          Bitboard occupied) const {
        Bitboard pawns = pos.getPieceBitboard(PAWN, us);
        const int pawnPush = (us == WHITE) ? 8 : -8;
        const int doublePush = (us == WHITE) ? 16 : -16;
        const Bitboard rank2 = (us == WHITE) ? 0xFF00ULL : 0x00FF000000000000ULL;
        const Bitboard rank7 = (us == WHITE) ? 0x00FF000000000000ULL : 0xFF00ULL;
        
        while (pawns) {
            Square from = popLsb(pawns);
            
            // Single push
            Square to = from + pawnPush;
            if (!(occupied & squareBB(to))) {
                if (squareBB(from) & rank7) {
                    // Promotion
                    moves.emplace_back(from, to, PROMOTION, PROMOTE_TO_QUEEN);
                    moves.emplace_back(from, to, PROMOTION, PROMOTE_TO_ROOK);
                    moves.emplace_back(from, to, PROMOTION, PROMOTE_TO_BISHOP);
                    moves.emplace_back(from, to, PROMOTION, PROMOTE_TO_KNIGHT);
                } else {
                    moves.emplace_back(from, to);
                    
                    // Double push
                    if ((squareBB(from) & rank2) && !(occupied & squareBB(from + doublePush))) {
                        moves.emplace_back(from, from + doublePush);
                    }
                }
            }
            
            // Captures
            Bitboard attacks = pawnAttacks[us][from] & theirPieces;
            while (attacks) {
                Square captureSq = popLsb(attacks);
                if (squareBB(from) & rank7) {
                    // Capture with promotion
                    moves.emplace_back(from, captureSq, PROMOTION, PROMOTE_TO_QUEEN);
                    moves.emplace_back(from, captureSq, PROMOTION, PROMOTE_TO_ROOK);
                    moves.emplace_back(from, captureSq, PROMOTION, PROMOTE_TO_BISHOP);
                    moves.emplace_back(from, captureSq, PROMOTION, PROMOTE_TO_KNIGHT);
                } else {
                    moves.emplace_back(from, captureSq);
                }
            }
            
            // En passant
            Square epSquare = pos.getEnPassantSquare();
            if (epSquare != NO_SQUARE && (pawnAttacks[us][from] & squareBB(epSquare))) {
                moves.emplace_back(from, epSquare, EN_PASSANT);
            }
        }
    }
    
    template<PieceType PT>
    void generatePieceMoves(const Position& pos, std::vector<Move>& moves,
                           Color us, Bitboard ourPieces, Bitboard occupied) const {
        Bitboard pieces = pos.getPieceBitboard(PT, us);
        
        while (pieces) {
            Square from = popLsb(pieces);
            Bitboard attacks = getAttacks<PT>(from, occupied) & ~ourPieces;
            
            while (attacks) {
                Square to = popLsb(attacks);
                moves.emplace_back(from, to);
            }
        }
    }
    
    template<PieceType PT>
    Bitboard getAttacks(Square sq, Bitboard occupied) const {
        switch (PT) {
            case KNIGHT: return knightAttacks[sq];
            case BISHOP: return getBishopAttacks(sq, occupied);
            case ROOK:   return getRookAttacks(sq, occupied);
            case QUEEN:  return getBishopAttacks(sq, occupied) | getRookAttacks(sq, occupied);
            default:     return 0;
        }
    }
    
    void generateKingMoves(const Position& pos, std::vector<Move>& moves,
                          Color us, Bitboard ourPieces, Bitboard occupied) const {
        Square kingSquare = lsb(pos.getPieceBitboard(KING, us));
        Bitboard attacks = kingAttacks[kingSquare] & ~ourPieces;
        
        while (attacks) {
            Square to = popLsb(attacks);
            moves.emplace_back(kingSquare, to);
        }
    }
    
    void generateCastlingMoves(const Position& pos, std::vector<Move>& moves,
                              Color us, Bitboard occupied) const {
        if (pos.isInCheck()) return;  // Can't castle out of check
        
        uint8_t rights = pos.getCastlingRights();
        Square kingSquare = lsb(pos.getPieceBitboard(KING, us));
        
        if (us == WHITE) {
            // White kingside
            if ((rights & WHITE_OO) && 
                !(occupied & (squareBB(F1) | squareBB(G1))) &&
                !pos.isSquareAttacked(F1, BLACK) &&
                !pos.isSquareAttacked(G1, BLACK)) {
                moves.emplace_back(E1, G1, CASTLING);
            }
            
            // White queenside
            if ((rights & WHITE_OOO) && 
                !(occupied & (squareBB(B1) | squareBB(C1) | squareBB(D1))) &&
                !pos.isSquareAttacked(C1, BLACK) &&
                !pos.isSquareAttacked(D1, BLACK)) {
                moves.emplace_back(E1, C1, CASTLING);
            }
        } else {
            // Black kingside
            if ((rights & BLACK_OO) && 
                !(occupied & (squareBB(F8) | squareBB(G8))) &&
                !pos.isSquareAttacked(F8, WHITE) &&
                !pos.isSquareAttacked(G8, WHITE)) {
                moves.emplace_back(E8, G8, CASTLING);
            }
            
            // Black queenside
            if ((rights & BLACK_OOO) && 
                !(occupied & (squareBB(B8) | squareBB(C8) | squareBB(D8))) &&
                !pos.isSquareAttacked(C8, WHITE) &&
                !pos.isSquareAttacked(D8, WHITE)) {
                moves.emplace_back(E8, C8, CASTLING);
            }
        }
    }
};

MoveGenerator::MoveGenerator() : pImpl(std::make_unique<Impl>()) {}
MoveGenerator::~MoveGenerator() = default;

std::vector<Move> MoveGenerator::generateLegalMoves(const Position& position) const {
    std::vector<Move> pseudoLegal = generatePseudoLegalMoves(position);
    std::vector<Move> legal;
    legal.reserve(pseudoLegal.size());
    
    for (const Move& move : pseudoLegal) {
        Position newPos = position.makeMove(move);
        if (!newPos.isInCheck()) {
            legal.push_back(move);
        }
    }
    
    return legal;
}

std::vector<Move> MoveGenerator::generatePseudoLegalMoves(const Position& position) const {
    return pImpl->generateAllMoves(position);
}

std::vector<Move> MoveGenerator::generateCaptures(const Position& position) const {
    std::vector<Move> allMoves = generatePseudoLegalMoves(position);
    std::vector<Move> captures;
    
    std::copy_if(allMoves.begin(), allMoves.end(), std::back_inserter(captures),
                 [&position](const Move& move) {
                     return position.getPieceAt(move.to()) != NO_PIECE || 
                            move.isEnPassant();
                 });
    
    return captures;
}

std::vector<Move> MoveGenerator::generateQuietMoves(const Position& position) const {
    std::vector<Move> allMoves = generatePseudoLegalMoves(position);
    std::vector<Move> quiet;
    
    std::copy_if(allMoves.begin(), allMoves.end(), std::back_inserter(quiet),
                 [&position](const Move& move) {
                     return position.getPieceAt(move.to()) == NO_PIECE && 
                            !move.isEnPassant();
                 });
    
    return quiet;
}

bool MoveGenerator::isLegal(const Position& position, const Move& move) const {
    // Simple check - make the move and see if king is in check
    Position newPos = position.makeMove(move);
    return !newPos.isInCheck();
}

Bitboard MoveGenerator::getAttacks(PieceType piece, Square square, Bitboard occupied) {
    switch (piece) {
        case PAWN:   return pawnAttacks[WHITE][square];  // Caller must handle color
        case KNIGHT: return knightAttacks[square];
        case BISHOP: return getBishopAttacks(square, occupied);
        case ROOK:   return getRookAttacks(square, occupied);
        case QUEEN:  return getBishopAttacks(square, occupied) | getRookAttacks(square, occupied);
        case KING:   return kingAttacks[square];
        default:     return 0;
    }
}

} // namespace chess 