#pragma once

#include "chess_analyzer/core/types.h"
#include <string>

namespace chess {

/**
 * @brief Represents a chess move with compact encoding
 * 
 * Moves are encoded in 16 bits:
 * - bits 0-5: from square (0-63)
 * - bits 6-11: to square (0-63)
 * - bits 12-13: promotion piece (queen, rook, bishop, knight)
 * - bits 14-15: move type (normal, promotion, en passant, castling)
 */
class Move {
public:
    /**
     * @brief Default constructor creates a null move
     */
    Move() : data(0) {}

    /**
     * @brief Construct a normal move
     * @param from Source square
     * @param to Destination square
     */
    Move(Square from, Square to) : data(encodeMove(from, to, NORMAL, 0)) {}

    /**
     * @brief Construct a special move (promotion, en passant, castling)
     * @param from Source square
     * @param to Destination square
     * @param type Move type
     * @param promotion Promotion piece type (for promotions only)
     */
    Move(Square from, Square to, MoveType type, PromotionType promotion = PROMOTE_TO_QUEEN)
        : data(encodeMove(from, to, type, promotion)) {}

    /**
     * @brief Get the source square
     * @return From square (0-63)
     */
    Square from() const { return data & 0x3F; }

    /**
     * @brief Get the destination square
     * @return To square (0-63)
     */
    Square to() const { return (data >> 6) & 0x3F; }

    /**
     * @brief Get the move type
     * @return Move type (normal, promotion, en passant, castling)
     */
    MoveType type() const { return static_cast<MoveType>((data >> 14) & 0x3); }

    /**
     * @brief Get the promotion piece type
     * @return Promotion type (only valid for promotion moves)
     */
    PromotionType promotionType() const { 
        return static_cast<PromotionType>((data >> 12) & 0x3); 
    }

    /**
     * @brief Check if this is a null move
     * @return true if null move
     */
    bool isNull() const { return data == 0; }

    /**
     * @brief Check if this is a promotion
     * @return true if promotion move
     */
    bool isPromotion() const { return type() == PROMOTION; }

    /**
     * @brief Check if this is an en passant capture
     * @return true if en passant
     */
    bool isEnPassant() const { return type() == EN_PASSANT; }

    /**
     * @brief Check if this is a castling move
     * @return true if castling
     */
    bool isCastling() const { return type() == CASTLING; }

    /**
     * @brief Convert move to algebraic notation
     * @return String in format "e2e4" or "e7e8q" for promotions
     */
    std::string toUCI() const;

    /**
     * @brief Convert move to standard algebraic notation (requires position context)
     * @param pos The position before the move
     * @return String like "Nf3", "e4", "O-O", etc.
     */
    std::string toAlgebraic(const class Position& pos) const;

    /**
     * @brief Parse move from UCI notation
     * @param uci String in format "e2e4" or "e7e8q"
     * @return Parsed move or null move if invalid
     */
    static Move fromUCI(const std::string& uci);

    /**
     * @brief Equality operator
     */
    bool operator==(const Move& other) const { return data == other.data; }
    bool operator!=(const Move& other) const { return data != other.data; }

    /**
     * @brief Get raw move data (for hashing, etc.)
     */
    uint16_t getRaw() const { return data; }

private:
    uint16_t data;

    static uint16_t encodeMove(Square from, Square to, MoveType type, PromotionType promotion) {
        return from | (to << 6) | (promotion << 12) | (type << 14);
    }
};

/**
 * @brief Null move constant
 */
constexpr Move NULL_MOVE{};

} // namespace chess 