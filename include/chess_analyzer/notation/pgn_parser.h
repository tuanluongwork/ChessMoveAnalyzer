#pragma once

#include "chess_analyzer/core/types.h"
#include "chess_analyzer/core/move.h"
#include "chess_analyzer/core/position.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace chess {

/**
 * @brief Represents a complete chess game with metadata
 */
struct Game {
    std::map<std::string, std::string> headers;  // PGN headers (Event, Date, White, Black, etc.)
    std::vector<Move> moves;                     // List of moves in the game
    std::string initialFEN;                      // Starting position (if not standard)
    std::string result;                          // Game result (1-0, 0-1, 1/2-1/2, *)
};

/**
 * @brief Parser for Portable Game Notation (PGN) files
 * 
 * This class can parse PGN files and convert between algebraic notation
 * and internal move representation.
 */
class PGNParser {
public:
    PGNParser();
    ~PGNParser();

    /**
     * @brief Parse a PGN string containing one or more games
     * @param pgn The PGN string to parse
     * @return Vector of parsed games
     */
    std::vector<Game> parsePGN(const std::string& pgn) const;

    /**
     * @brief Parse a single game from PGN
     * @param pgn The PGN string containing a single game
     * @return Parsed game
     */
    Game parseGame(const std::string& pgn) const;

    /**
     * @brief Convert a game to PGN format
     * @param game The game to convert
     * @return PGN string representation
     */
    std::string gameToPGN(const Game& game) const;

    /**
     * @brief Parse a move in algebraic notation
     * @param position The current position
     * @param moveStr The move string (e.g., "Nf3", "e4", "O-O")
     * @return The parsed move, or NULL_MOVE if invalid
     */
    Move parseAlgebraicMove(const Position& position, const std::string& moveStr) const;

    /**
     * @brief Convert a move to standard algebraic notation
     * @param position The position before the move
     * @param move The move to convert
     * @return Algebraic notation string
     */
    std::string moveToAlgebraic(const Position& position, const Move& move) const;

    /**
     * @brief Validate a PGN file
     * @param pgn The PGN string to validate
     * @return true if valid, false otherwise
     */
    bool validatePGN(const std::string& pgn) const;

    /**
     * @brief Get error message from last parse operation
     * @return Error message, or empty string if no error
     */
    std::string getLastError() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace chess 