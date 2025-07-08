#pragma once

#include "chess_analyzer/core/types.h"
#include "chess_analyzer/core/position.h"
#include <memory>

namespace chess {

/**
 * @brief Chess position evaluator with multiple evaluation terms
 * 
 * This class evaluates chess positions using various factors including
 * material balance, piece activity, pawn structure, king safety, and more.
 */
class Evaluator {
public:
    Evaluator();
    ~Evaluator();

    /**
     * @brief Evaluate a position from the perspective of the side to move
     * @param position The position to evaluate
     * @return Evaluation in centipawns (positive = good for side to move)
     */
    int evaluate(const Position& position) const;

    /**
     * @brief Get material balance
     * @param position The position to evaluate
     * @return Material difference in centipawns
     */
    int getMaterialBalance(const Position& position) const;

    /**
     * @brief Evaluate pawn structure
     * @param position The position to evaluate
     * @return Pawn structure evaluation
     */
    int evaluatePawnStructure(const Position& position) const;

    /**
     * @brief Evaluate king safety
     * @param position The position to evaluate
     * @param color The color of the king to evaluate
     * @return King safety score
     */
    int evaluateKingSafety(const Position& position, Color color) const;

    /**
     * @brief Evaluate piece mobility
     * @param position The position to evaluate
     * @return Mobility evaluation
     */
    int evaluateMobility(const Position& position) const;

    /**
     * @brief Evaluate center control
     * @param position The position to evaluate
     * @return Center control score
     */
    int evaluateCenterControl(const Position& position) const;

    /**
     * @brief Check if position is in endgame
     * @param position The position to check
     * @return true if endgame, false if middlegame
     */
    bool isEndgame(const Position& position) const;

    /**
     * @brief Set evaluation parameters
     * @param paramName Parameter name
     * @param value New value
     */
    void setParameter(const std::string& paramName, int value);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// Piece values in centipawns
namespace PieceValue {
    constexpr int PAWN = 100;
    constexpr int KNIGHT = 320;
    constexpr int BISHOP = 330;
    constexpr int ROOK = 500;
    constexpr int QUEEN = 900;
    constexpr int KING = 20000;
}

} // namespace chess 