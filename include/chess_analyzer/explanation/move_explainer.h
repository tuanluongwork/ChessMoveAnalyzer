#pragma once

#include "chess_analyzer/core/types.h"
#include "chess_analyzer/core/move.h"
#include "chess_analyzer/core/position.h"
#include <string>
#include <vector>
#include <memory>

namespace chess {

/**
 * @brief Tactical themes that can be identified in a position
 */
enum class TacticalTheme {
    PIN,
    FORK,
    SKEWER,
    DISCOVERED_ATTACK,
    DOUBLE_ATTACK,
    REMOVE_DEFENDER,
    DEFLECTION,
    DECOY,
    INTERFERENCE,
    ZUGZWANG,
    STALEMATE_TRAP,
    PERPETUAL_CHECK,
    BACK_RANK_MATE,
    SMOTHERED_MATE
};

/**
 * @brief Strategic concepts for move explanations
 */
enum class StrategicConcept {
    CENTER_CONTROL,
    PIECE_DEVELOPMENT,
    KING_SAFETY,
    PAWN_STRUCTURE,
    PIECE_ACTIVITY,
    SPACE_ADVANTAGE,
    WEAK_SQUARES,
    OPEN_FILES,
    OUTPOST,
    MINORITY_ATTACK,
    PAWN_BREAK,
    PIECE_COORDINATION,
    INITIATIVE,
    TIME_ADVANTAGE
};

/**
 * @brief Provides natural language explanations for chess moves
 * 
 * This class analyzes moves in context and generates human-readable
 * explanations that help players understand the purpose and consequences
 * of each move.
 */
class MoveExplainer {
public:
    MoveExplainer();
    ~MoveExplainer();

    /**
     * @brief Generate a comprehensive explanation for a move
     * @param position The position before the move
     * @param move The move to explain
     * @return Natural language explanation
     */
    std::string explainMove(const Position& position, const Move& move) const;

    /**
     * @brief Identify tactical themes in a position
     * @param position The position to analyze
     * @return Vector of identified tactical themes
     */
    std::vector<TacticalTheme> identifyTactics(const Position& position) const;

    /**
     * @brief Identify strategic concepts relevant to a move
     * @param position The position before the move
     * @param move The move to analyze
     * @return Vector of relevant strategic concepts
     */
    std::vector<StrategicConcept> identifyStrategicConcepts(
        const Position& position, const Move& move) const;

    /**
     * @brief Explain the immediate consequences of a move
     * @param position The position before the move
     * @param move The move to analyze
     * @return Description of immediate effects (captures, checks, etc.)
     */
    std::string explainImmediateEffects(const Position& position, const Move& move) const;

    /**
     * @brief Explain the positional impact of a move
     * @param position The position before the move
     * @param move The move to analyze
     * @return Description of positional changes
     */
    std::string explainPositionalImpact(const Position& position, const Move& move) const;

    /**
     * @brief Generate opening-specific explanations
     * @param position The position before the move
     * @param move The move to analyze
     * @return Opening-related explanation if applicable
     */
    std::string explainOpeningMove(const Position& position, const Move& move) const;

    /**
     * @brief Generate endgame-specific explanations
     * @param position The position before the move
     * @param move The move to analyze
     * @return Endgame-related explanation if applicable
     */
    std::string explainEndgameMove(const Position& position, const Move& move) const;

    /**
     * @brief Set the explanation detail level
     * @param level 0 = brief, 1 = normal, 2 = detailed
     */
    void setDetailLevel(int level);

    /**
     * @brief Set the target audience skill level
     * @param rating Approximate rating (800-2800)
     */
    void setTargetAudience(int rating);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Convert tactical theme to string
 */
std::string tacticalThemeToString(TacticalTheme theme);

/**
 * @brief Convert strategic concept to string
 */
std::string strategicConceptToString(StrategicConcept concept);

} // namespace chess 