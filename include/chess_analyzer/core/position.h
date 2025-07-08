#pragma once

#include "chess_analyzer/core/types.h"
#include <string>
#include <array>

namespace chess {

/**
 * @brief Represents a chess position using bitboards for high performance
 * 
 * This class uses 64-bit bitboards to represent piece positions, enabling
 * fast move generation and position manipulation through bit operations.
 */
class Position {
public:
    /**
     * @brief Default constructor creates the standard starting position
     */
    Position();

    /**
     * @brief Construct position from FEN (Forsyth-Edwards Notation) string
     * @param fen The FEN string representation
     */
    explicit Position(const std::string& fen);

    /**
     * @brief Get bitboard for a specific piece type and color
     * @param piece The piece type
     * @param color The piece color
     * @return 64-bit bitboard with 1s where pieces are located
     */
    Bitboard getPieceBitboard(PieceType piece, Color color) const;

    /**
     * @brief Get bitboard for all pieces of a specific color
     * @param color The piece color
     * @return Bitboard of all pieces of that color
     */
    Bitboard getColorBitboard(Color color) const;

    /**
     * @brief Get bitboard of all occupied squares
     * @return Bitboard with 1s on all occupied squares
     */
    Bitboard getOccupiedBitboard() const;

    /**
     * @brief Get the piece at a specific square
     * @param square The square to check (0-63)
     * @return The piece at that square, or NONE if empty
     */
    Piece getPieceAt(Square square) const;

    /**
     * @brief Check whose turn it is to move
     * @return WHITE or BLACK
     */
    Color getSideToMove() const { return sideToMove; }

    /**
     * @brief Get castling rights
     * @return Bitfield of castling rights (K=1, Q=2, k=4, q=8)
     */
    uint8_t getCastlingRights() const { return castlingRights; }

    /**
     * @brief Get en passant target square
     * @return Square that can be captured en passant, or NO_SQUARE
     */
    Square getEnPassantSquare() const { return enPassantSquare; }

    /**
     * @brief Get the number of halfmoves since last pawn move or capture
     * @return Halfmove clock for 50-move rule
     */
    int getHalfmoveClock() const { return halfmoveClock; }

    /**
     * @brief Get the full move number
     * @return Current move number
     */
    int getFullmoveNumber() const { return fullmoveNumber; }

    /**
     * @brief Make a move on the position
     * @param move The move to make
     * @return New position after the move
     */
    Position makeMove(const Move& move) const;

    /**
     * @brief Check if the current side is in check
     * @return true if in check, false otherwise
     */
    bool isInCheck() const;

    /**
     * @brief Check if a square is attacked by the opponent
     * @param square The square to check
     * @param byColor The attacking color
     * @return true if the square is attacked
     */
    bool isSquareAttacked(Square square, Color byColor) const;

    /**
     * @brief Convert position to FEN string
     * @return FEN representation of the position
     */
    std::string toFEN() const;

    /**
     * @brief Get Zobrist hash of the position
     * @return 64-bit hash for transposition table
     */
    uint64_t getHash() const { return zobristHash; }

    /**
     * @brief Check if position is a draw by repetition or 50-move rule
     * @return true if position is drawn
     */
    bool isDraw() const;

    /**
     * @brief Equality operator for positions
     */
    bool operator==(const Position& other) const;

private:
    // Bitboards for each piece type and color
    std::array<std::array<Bitboard, 6>, 2> pieceBitboards;  // [color][piece_type]
    
    // Game state
    Color sideToMove;
    uint8_t castlingRights;
    Square enPassantSquare;
    int halfmoveClock;
    int fullmoveNumber;
    
    // Zobrist hash for fast position comparison
    uint64_t zobristHash;
    
    // Helper methods
    void initializeFromFEN(const std::string& fen);
    void updateHash(const Move& move);
    void clearSquare(Square square);
    void putPiece(Square square, PieceType piece, Color color);
};

} // namespace chess 