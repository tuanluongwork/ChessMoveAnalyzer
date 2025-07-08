#include "chess_analyzer/core/position.h"
#include <sstream>
#include <cctype>

namespace chess {

// Starting position FEN
constexpr const char* STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

Position::Position() {
    initializeFromFEN(STARTING_FEN);
}

Position::Position(const std::string& fen) {
    initializeFromFEN(fen);
}

void Position::initializeFromFEN(const std::string& fen) {
    // Clear all bitboards
    for (auto& colorBB : pieceBitboards) {
        for (auto& pieceBB : colorBB) {
            pieceBB = 0;
        }
    }
    
    std::istringstream ss(fen);
    std::string board, color, castling, enPassant;
    
    ss >> board >> color >> castling >> enPassant >> halfmoveClock >> fullmoveNumber;
    
    // Parse board
    Square sq = A8;
    for (char ch : board) {
        if (ch == '/') {
            sq -= 16;  // Next rank
        } else if (std::isdigit(ch)) {
            sq += (ch - '0');  // Empty squares
        } else {
            // Piece
            Color c = std::isupper(ch) ? WHITE : BLACK;
            ch = std::tolower(ch);
            
            PieceType pt;
            switch (ch) {
                case 'p': pt = PAWN; break;
                case 'n': pt = KNIGHT; break;
                case 'b': pt = BISHOP; break;
                case 'r': pt = ROOK; break;
                case 'q': pt = QUEEN; break;
                case 'k': pt = KING; break;
                default: continue;
            }
            
            putPiece(sq, pt, c);
            sq++;
        }
    }
    
    // Parse side to move
    sideToMove = (color == "w") ? WHITE : BLACK;
    
    // Parse castling rights
    castlingRights = NO_CASTLING;
    for (char ch : castling) {
        switch (ch) {
            case 'K': castlingRights |= WHITE_OO; break;
            case 'Q': castlingRights |= WHITE_OOO; break;
            case 'k': castlingRights |= BLACK_OO; break;
            case 'q': castlingRights |= BLACK_OOO; break;
        }
    }
    
    // Parse en passant square
    enPassantSquare = (enPassant == "-") ? NO_SQUARE : stringToSquare(enPassant);
    
    // TODO: Calculate zobrist hash
    zobristHash = 0;
}

Bitboard Position::getPieceBitboard(PieceType piece, Color color) const {
    return pieceBitboards[color][piece];
}

Bitboard Position::getColorBitboard(Color color) const {
    Bitboard bb = 0;
    for (int pt = PAWN; pt <= KING; ++pt) {
        bb |= pieceBitboards[color][pt];
    }
    return bb;
}

Bitboard Position::getOccupiedBitboard() const {
    return getColorBitboard(WHITE) | getColorBitboard(BLACK);
}

Piece Position::getPieceAt(Square square) const {
    Bitboard sqBB = squareBB(square);
    
    for (Color c : {WHITE, BLACK}) {
        for (PieceType pt = PAWN; pt <= KING; pt = static_cast<PieceType>(pt + 1)) {
            if (pieceBitboards[c][pt] & sqBB) {
                return makePiece(c, pt);
            }
        }
    }
    
    return NO_PIECE;
}

void Position::putPiece(Square square, PieceType piece, Color color) {
    pieceBitboards[color][piece] |= squareBB(square);
}

void Position::clearSquare(Square square) {
    Bitboard sqBB = squareBB(square);
    for (auto& colorBB : pieceBitboards) {
        for (auto& pieceBB : colorBB) {
            pieceBB &= ~sqBB;
        }
    }
}

std::string Position::toFEN() const {
    std::ostringstream fen;
    
    // Board
    for (int rank = 7; rank >= 0; --rank) {
        int emptyCount = 0;
        for (int file = 0; file < 8; ++file) {
            Square sq = makeSquare(file, rank);
            Piece p = getPieceAt(sq);
            
            if (p == NO_PIECE) {
                emptyCount++;
            } else {
                if (emptyCount > 0) {
                    fen << emptyCount;
                    emptyCount = 0;
                }
                
                char ch;
                PieceType pt = typeOf(p);
                switch (pt) {
                    case PAWN: ch = 'p'; break;
                    case KNIGHT: ch = 'n'; break;
                    case BISHOP: ch = 'b'; break;
                    case ROOK: ch = 'r'; break;
                    case QUEEN: ch = 'q'; break;
                    case KING: ch = 'k'; break;
                    default: ch = '?';
                }
                
                if (colorOf(p) == WHITE) {
                    ch = std::toupper(ch);
                }
                fen << ch;
            }
        }
        
        if (emptyCount > 0) {
            fen << emptyCount;
        }
        
        if (rank > 0) {
            fen << '/';
        }
    }
    
    // Side to move
    fen << ' ' << (sideToMove == WHITE ? 'w' : 'b');
    
    // Castling rights
    fen << ' ';
    if (castlingRights == NO_CASTLING) {
        fen << '-';
    } else {
        if (castlingRights & WHITE_OO) fen << 'K';
        if (castlingRights & WHITE_OOO) fen << 'Q';
        if (castlingRights & BLACK_OO) fen << 'k';
        if (castlingRights & BLACK_OOO) fen << 'q';
    }
    
    // En passant
    fen << ' ' << (enPassantSquare == NO_SQUARE ? "-" : squareToString(enPassantSquare));
    
    // Clocks
    fen << ' ' << halfmoveClock << ' ' << fullmoveNumber;
    
    return fen.str();
}

bool Position::operator==(const Position& other) const {
    return pieceBitboards == other.pieceBitboards &&
           sideToMove == other.sideToMove &&
           castlingRights == other.castlingRights &&
           enPassantSquare == other.enPassantSquare;
}

// TODO: Implement remaining methods
Position Position::makeMove(const Move& move) const {
    Position newPos = *this;
    // Implementation needed
    return newPos;
}

bool Position::isInCheck() const {
    // Implementation needed
    return false;
}

bool Position::isSquareAttacked(Square square, Color byColor) const {
    // Implementation needed
    return false;
}

bool Position::isDraw() const {
    // Check 50-move rule
    if (halfmoveClock >= 100) {
        return true;
    }
    
    // TODO: Check for insufficient material, repetition
    return false;
}

} // namespace chess 