#pragma once

#include "chess_analyzer/core/position.h"
#include "chess_analyzer/core/move.h"
#include "chess_analyzer/core/move_generator.h"
#include "chess_analyzer/evaluation/evaluator.h"
#include "chess_analyzer/explanation/move_explainer.h"
#include "chess_analyzer/notation/pgn_parser.h"

#include <string>
#include <vector>
#include <memory>

namespace chess {

/**
 * @brief Main interface for the Chess Move Analyzer library
 * 
 * This class provides high-level functionality for analyzing chess positions,
 * generating moves, evaluating positions, and explaining moves in natural language.
 */
class ChessAnalyzer {
public:
    ChessAnalyzer();
    ~ChessAnalyzer();

    /**
     * @brief Generate all legal moves for a given position
     * @param position The chess position to analyze
     * @return Vector of all legal moves
     */
    std::vector<Move> generateMoves(const Position& position) const;

    /**
     * @brief Evaluate a chess position
     * @param position The position to evaluate
     * @return Evaluation score in centipawns (positive favors white)
     */
    int evaluatePosition(const Position& position) const;

    /**
     * @brief Generate a human-readable explanation for a move
     * @param position The position before the move
     * @param move The move to explain
     * @return Natural language explanation of the move
     */
    std::string explainMove(const Position& position, const Move& move) const;

    /**
     * @brief Find the best move in a position
     * @param position The position to analyze
     * @param depth Search depth (default: 6)
     * @return The best move found
     */
    Move findBestMove(const Position& position, int depth = 6) const;

    /**
     * @brief Analyze a complete game from PGN
     * @param pgn The PGN string of the game
     * @return Vector of move explanations for each move in the game
     */
    std::vector<std::string> analyzeGame(const std::string& pgn) const;

    /**
     * @brief Check if a move is legal in a given position
     * @param position The current position
     * @param move The move to validate
     * @return true if the move is legal, false otherwise
     */
    bool isLegalMove(const Position& position, const Move& move) const;

    /**
     * @brief Get tactical themes in a position
     * @param position The position to analyze
     * @return Vector of tactical themes (e.g., "pin", "fork", "discovered attack")
     */
    std::vector<std::string> getTacticalThemes(const Position& position) const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace chess 