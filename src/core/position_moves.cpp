#include "chess_analyzer/core/position.h"
#include "chess_analyzer/core/move_generator.h"
#include "chess_analyzer/core/bitboard_attacks.h"

namespace chess {

Position Position::makeMove(const Move& move) const {
    Position newPos = *this;
    
    Square from = move.from();
    Square to = move.to();
    Piece movedPiece = getPieceAt(from);
    Piece capturedPiece = getPieceAt(to);
    PieceType pieceType = typeOf(movedPiece);
    Color us = sideToMove;
    Color them = ~us;
    
    // Clear the source square
    newPos.clearSquare(from);
    
    // Handle captures
    if (capturedPiece != NO_PIECE) {
        newPos.clearSquare(to);
        newPos.halfmoveClock = 0;  // Reset on capture
    } else {
        newPos.halfmoveClock++;
    }
    
    // Handle special moves
    if (move.isCastling()) {
        // Move the king
        newPos.putPiece(to, KING, us);
        
        // Move the rook
        if (to > from) {  // Kingside
            Square rookFrom = (us == WHITE) ? H1 : H8;
            Square rookTo = (us == WHITE) ? F1 : F8;
            newPos.clearSquare(rookFrom);
            newPos.putPiece(rookTo, ROOK, us);
        } else {  // Queenside
            Square rookFrom = (us == WHITE) ? A1 : A8;
            Square rookTo = (us == WHITE) ? D1 : D8;
            newPos.clearSquare(rookFrom);
            newPos.putPiece(rookTo, ROOK, us);
        }
    } else if (move.isEnPassant()) {
        // Place pawn on destination
        newPos.putPiece(to, PAWN, us);
        
        // Remove captured pawn
        Square capturedPawnSq = (us == WHITE) ? to - 8 : to + 8;
        newPos.clearSquare(capturedPawnSq);
        newPos.halfmoveClock = 0;  // Reset on pawn capture
    } else if (move.isPromotion()) {
        // Place promoted piece
        PieceType promotedPiece;
        switch (move.promotionType()) {
            case PROMOTE_TO_QUEEN:  promotedPiece = QUEEN; break;
            case PROMOTE_TO_ROOK:   promotedPiece = ROOK; break;
            case PROMOTE_TO_BISHOP: promotedPiece = BISHOP; break;
            case PROMOTE_TO_KNIGHT: promotedPiece = KNIGHT; break;
        }
        newPos.putPiece(to, promotedPiece, us);
        newPos.halfmoveClock = 0;  // Reset on pawn move
    } else {
        // Normal move
        newPos.putPiece(to, pieceType, us);
        if (pieceType == PAWN) {
            newPos.halfmoveClock = 0;  // Reset on pawn move
        }
    }
    
    // Update castling rights
    if (pieceType == KING) {
        // King moved, lose all castling rights
        if (us == WHITE) {
            newPos.castlingRights &= ~(WHITE_OO | WHITE_OOO);
        } else {
            newPos.castlingRights &= ~(BLACK_OO | BLACK_OOO);
        }
    } else if (pieceType == ROOK) {
        // Rook moved, lose castling rights on that side
        if (from == A1) newPos.castlingRights &= ~WHITE_OOO;
        else if (from == H1) newPos.castlingRights &= ~WHITE_OO;
        else if (from == A8) newPos.castlingRights &= ~BLACK_OOO;
        else if (from == H8) newPos.castlingRights &= ~BLACK_OO;
    }
    
    // Captures on rook squares also affect castling
    if (to == A1) newPos.castlingRights &= ~WHITE_OOO;
    else if (to == H1) newPos.castlingRights &= ~WHITE_OO;
    else if (to == A8) newPos.castlingRights &= ~BLACK_OOO;
    else if (to == H8) newPos.castlingRights &= ~BLACK_OO;
    
    // Update en passant square
    if (pieceType == PAWN && std::abs(to - from) == 16) {
        // Double pawn push, set en passant square
        newPos.enPassantSquare = (us == WHITE) ? from + 8 : from - 8;
    } else {
        newPos.enPassantSquare = NO_SQUARE;
    }
    
    // Switch side to move
    newPos.sideToMove = them;
    
    // Update fullmove number
    if (us == BLACK) {
        newPos.fullmoveNumber++;
    }
    
    // TODO: Update zobrist hash
    newPos.updateHash(move);
    
    return newPos;
}

bool Position::isInCheck() const {
    Square kingSquare = lsb(getPieceBitboard(KING, sideToMove));
    return isSquareAttacked(kingSquare, ~sideToMove);
}

bool Position::isSquareAttacked(Square square, Color byColor) const {
    Bitboard occupied = getOccupiedBitboard();
    
    // Check pawn attacks
    Bitboard enemyPawns = getPieceBitboard(PAWN, byColor);
    if (byColor == WHITE) {
        if ((shift<Direction::SOUTH_WEST>(squareBB(square)) | 
             shift<Direction::SOUTH_EAST>(squareBB(square))) & enemyPawns) {
            return true;
        }
    } else {
        if ((shift<Direction::NORTH_WEST>(squareBB(square)) | 
             shift<Direction::NORTH_EAST>(squareBB(square))) & enemyPawns) {
            return true;
        }
    }
    
    // Check knight attacks
    if (knightAttacksBB(square) & getPieceBitboard(KNIGHT, byColor)) {
        return true;
    }
    
    // Check bishop/queen attacks
    Bitboard bishopAttacks = bishopAttacksBB(square, occupied);
    if (bishopAttacks & (getPieceBitboard(BISHOP, byColor) | 
                        getPieceBitboard(QUEEN, byColor))) {
        return true;
    }
    
    // Check rook/queen attacks
    Bitboard rookAttacks = rookAttacksBB(square, occupied);
    if (rookAttacks & (getPieceBitboard(ROOK, byColor) | 
                      getPieceBitboard(QUEEN, byColor))) {
        return true;
    }
    
    // Check king attacks
    if (kingAttacksBB(square) & getPieceBitboard(KING, byColor)) {
        return true;
    }
    
    return false;
}

void Position::updateHash(const Move& move) {
    // TODO: Implement zobrist hashing
    // For now, just increment to ensure different positions have different hashes
    zobristHash++;
}

} // namespace chess 