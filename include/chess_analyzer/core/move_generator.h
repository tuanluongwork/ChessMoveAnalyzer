#pragma once

#include "chess_analyzer/core/types.h"
#include "chess_analyzer/core/position.h"
#include "chess_analyzer/core/move.h"
#include <vector>
#include <memory>

namespace chess {

/**
 * @brief High-performance move generator using bitboards
 * 
 * This class generates all legal moves for a given position using
 * efficient bitboard algorithms including magic bitboards for sliding pieces.
 */
class MoveGenerator {
public:
    MoveGenerator();
    ~MoveGenerator();

    /**
     * @brief Generate all legal moves for a position
     * @param position The position to generate moves for
     * @return Vector of all legal moves
     */
    std::vector<Move> generateLegalMoves(const Position& position) const;

    /**
     * @brief Generate all pseudo-legal moves (may leave king in check)
     * @param position The position to generate moves for
     * @return Vector of pseudo-legal moves
     */
    std::vector<Move> generatePseudoLegalMoves(const Position& position) const;

    /**
     * @brief Generate only capture moves
     * @param position The position to generate moves for
     * @return Vector of capture moves
     */
    std::vector<Move> generateCaptures(const Position& position) const;

    /**
     * @brief Generate only quiet moves (non-captures)
     * @param position The position to generate moves for
     * @return Vector of quiet moves
     */
    std::vector<Move> generateQuietMoves(const Position& position) const;

    /**
     * @brief Check if a move is legal in the given position
     * @param position The current position
     * @param move The move to check
     * @return true if the move is legal
     */
    bool isLegal(const Position& position, const Move& move) const;

    /**
     * @brief Get attack bitboard for a piece on a square
     * @param piece The piece type
     * @param square The square the piece is on
     * @param occupied Bitboard of all occupied squares
     * @return Bitboard of attacked squares
     */
    static Bitboard getAttacks(PieceType piece, Square square, Bitboard occupied);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace chess 