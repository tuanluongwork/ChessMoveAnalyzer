#include "chess_analyzer/core/move.h"
#include "chess_analyzer/core/position.h"
#include <sstream>
#include <cctype>

namespace chess {

std::string Move::toUCI() const {
    if (isNull()) return "0000";
    
    std::string uci = squareToString(from()) + squareToString(to());
    
    if (isPromotion()) {
        switch (promotionType()) {
            case PROMOTE_TO_QUEEN:  uci += 'q'; break;
            case PROMOTE_TO_ROOK:   uci += 'r'; break;
            case PROMOTE_TO_BISHOP: uci += 'b'; break;
            case PROMOTE_TO_KNIGHT: uci += 'n'; break;
        }
    }
    
    return uci;
}

std::string Move::toAlgebraic(const Position& pos) const {
    if (isNull()) return "--";
    
    // Handle castling
    if (isCastling()) {
        return (to() > from()) ? "O-O" : "O-O-O";
    }
    
    std::ostringstream san;
    Piece piece = pos.getPieceAt(from());
    PieceType pieceType = typeOf(piece);
    
    // Add piece symbol (pawns have no symbol)
    if (pieceType != PAWN) {
        const char* symbols = " NBRQK";
        san << symbols[pieceType];
        
        // Add disambiguation if needed
        // Check if another piece of same type can move to same square
        // This is a simplified version - full implementation would use move generator
        for (Square sq = A1; sq <= H8; ++sq) {
            if (sq == from()) continue;
            Piece p = pos.getPieceAt(sq);
            if (p != NO_PIECE && typeOf(p) == pieceType && colorOf(p) == pos.getSideToMove()) {
                // Add file or rank disambiguation
                if (fileOf(sq) != fileOf(from())) {
                    san << static_cast<char>('a' + fileOf(from()));
                } else {
                    san << static_cast<char>('1' + rankOf(from()));
                }
                break;
            }
        }
    }
    
    // Add capture symbol
    if (pos.getPieceAt(to()) != NO_PIECE || isEnPassant()) {
        if (pieceType == PAWN) {
            san << static_cast<char>('a' + fileOf(from()));
        }
        san << 'x';
    }
    
    // Add destination square
    san << squareToString(to());
    
    // Add promotion
    if (isPromotion()) {
        san << '=';
        const char* symbols = "QRBN";
        san << symbols[promotionType()];
    }
    
    // Check if move gives check or checkmate
    Position afterMove = pos.makeMove(*this);
    if (afterMove.isInCheck()) {
        // Simplified - would need full move generation to detect checkmate
        san << '+';
    }
    
    return san.str();
}

Move Move::fromUCI(const std::string& uci) {
    if (uci.length() < 4 || uci.length() > 5) {
        return NULL_MOVE;
    }
    
    Square from = stringToSquare(uci.substr(0, 2));
    Square to = stringToSquare(uci.substr(2, 2));
    
    if (from == NO_SQUARE || to == NO_SQUARE) {
        return NULL_MOVE;
    }
    
    // Check for promotion
    if (uci.length() == 5) {
        PromotionType promo;
        switch (std::tolower(uci[4])) {
            case 'q': promo = PROMOTE_TO_QUEEN; break;
            case 'r': promo = PROMOTE_TO_ROOK; break;
            case 'b': promo = PROMOTE_TO_BISHOP; break;
            case 'n': promo = PROMOTE_TO_KNIGHT; break;
            default: return NULL_MOVE;
        }
        return Move(from, to, PROMOTION, promo);
    }
    
    // Check for castling (king moves two squares)
    if (std::abs(to - from) == 2 && (from == E1 || from == E8)) {
        return Move(from, to, CASTLING);
    }
    
    // Normal move (en passant detection would require position context)
    return Move(from, to);
}

} // namespace chess 