# Chess Move Analyzer API Documentation

## Overview

The Chess Move Analyzer is a high-performance C++ library for chess position analysis, move generation, and natural language move explanations.

## Core Classes

### `ChessAnalyzer`

The main interface class that provides high-level chess analysis functionality.

#### Methods

##### `std::vector<Move> generateMoves(const Position& position)`
Generates all legal moves for the given position.
- **Parameters**: `position` - The chess position to analyze
- **Returns**: Vector of all legal moves
- **Performance**: ~5 million positions/second on modern hardware

##### `int evaluatePosition(const Position& position)`
Evaluates the position from the perspective of the side to move.
- **Parameters**: `position` - The position to evaluate
- **Returns**: Evaluation score in centipawns (positive = favorable)
- **Note**: Uses material balance, piece-square tables, pawn structure, mobility, and king safety

##### `std::string explainMove(const Position& position, const Move& move)`
Generates a human-readable explanation for a chess move.
- **Parameters**: 
  - `position` - The position before the move
  - `move` - The move to explain
- **Returns**: Natural language explanation string
- **Example**: "Moves the knight from b1 to c3, developing a piece toward the center"

##### `Move findBestMove(const Position& position, int depth = 6)`
Finds the best move using minimax search with alpha-beta pruning.
- **Parameters**: 
  - `position` - The position to analyze
  - `depth` - Search depth (default: 6 half-moves)
- **Returns**: The best move found
- **Algorithm**: Minimax with alpha-beta pruning and move ordering

### `Position`

Represents a chess position using bitboards for optimal performance.

#### Constructors

##### `Position()`
Creates the standard starting position.

##### `Position(const std::string& fen)`
Creates a position from FEN notation.
- **Parameters**: `fen` - Forsyth-Edwards Notation string
- **Example**: `"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"`

#### Key Methods

##### `Position makeMove(const Move& move) const`
Creates a new position after making a move.
- **Parameters**: `move` - The move to make
- **Returns**: New position after the move
- **Note**: Original position is unchanged (immutable)

##### `bool isInCheck() const`
Checks if the current side to move is in check.

##### `std::string toFEN() const`
Converts the position to FEN notation.

### `Move`

Represents a chess move with compact 16-bit encoding.

#### Constructors

##### `Move(Square from, Square to)`
Creates a normal move.

##### `Move(Square from, Square to, MoveType type, PromotionType promotion)`
Creates a special move (castling, en passant, promotion).

#### Methods

##### `std::string toUCI() const`
Converts to UCI notation (e.g., "e2e4", "e7e8q").

##### `std::string toAlgebraic(const Position& pos) const`
Converts to standard algebraic notation (e.g., "e4", "Nf3", "O-O").

##### `static Move fromUCI(const std::string& uci)`
Parses a move from UCI notation.

## Types and Constants

### Basic Types
```cpp
using Bitboard = uint64_t;  // 64-bit board representation
using Square = int;         // 0-63 (a1=0, h8=63)
```

### Enumerations

#### `Color`
```cpp
enum Color : uint8_t {
    WHITE = 0,
    BLACK = 1
};
```

#### `PieceType`
```cpp
enum PieceType : uint8_t {
    PAWN = 0,
    KNIGHT = 1,
    BISHOP = 2,
    ROOK = 3,
    QUEEN = 4,
    KING = 5
};
```

#### `MoveType`
```cpp
enum MoveType : uint8_t {
    NORMAL = 0,
    PROMOTION = 1,
    EN_PASSANT = 2,
    CASTLING = 3
};
```

## Usage Examples

### Basic Position Analysis
```cpp
#include "chess_analyzer.h"

using namespace chess;

ChessAnalyzer analyzer;
Position pos;  // Starting position

// Generate all legal moves
std::vector<Move> moves = analyzer.generateMoves(pos);
std::cout << "Legal moves: " << moves.size() << std::endl;

// Evaluate the position
int eval = analyzer.evaluatePosition(pos);
std::cout << "Evaluation: " << eval << " centipawns" << std::endl;

// Find the best move
Move best = analyzer.findBestMove(pos, 6);
std::cout << "Best move: " << best.toAlgebraic(pos) << std::endl;
```

### Move Explanation
```cpp
// Explain a specific move
Move e4 = Move::fromUCI("e2e4");
std::string explanation = analyzer.explainMove(pos, e4);
std::cout << explanation << std::endl;
// Output: "Moves the pawn from e2 to e4. Controls the center"
```

### Working with FEN
```cpp
// Load position from FEN
Position customPos("r1bqkb1r/pppp1ppp/2n2n2/4p3/2B1P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 4 4");

// Convert back to FEN
std::string fen = customPos.toFEN();
```

### Making Moves
```cpp
// Make a move
Move move = Move::fromUCI("e2e4");
Position newPos = pos.makeMove(move);

// Check game state
if (newPos.isInCheck()) {
    std::cout << "Check!" << std::endl;
}
```

## Performance Considerations

- **Bitboards**: All piece positions are stored as 64-bit integers for fast operations
- **Move Generation**: Uses pre-calculated attack tables for non-sliding pieces
- **Immutable Positions**: Positions are immutable, ensuring thread safety
- **Memory Usage**: Each position uses approximately 200 bytes

## Thread Safety

- `Position` objects are immutable and thread-safe
- `ChessAnalyzer` methods are const and can be called concurrently
- Move generation and evaluation do not modify global state

## Error Handling

- Invalid FEN strings throw `std::invalid_argument`
- Invalid moves return `NULL_MOVE`
- All methods provide strong exception safety guarantee

## Future Enhancements

- Magic bitboards for sliding piece move generation
- Opening book integration
- Endgame tablebase support
- UCI protocol implementation
- Neural network evaluation 