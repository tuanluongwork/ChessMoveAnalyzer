# Chess Move Analyzer

A high-performance C++ library for analyzing chess positions and explaining moves in human-readable language. This project demonstrates modern C++ practices, deep chess knowledge, and the ability to make chess concepts accessible to players of all levels.

## 🎯 Features

- **Bitboard-based board representation** for optimal performance
- **Complete move generation** with full chess rules implementation
- **Position evaluation** using multiple chess heuristics
- **Natural language move explanations** that help players understand chess concepts
- **PGN (Portable Game Notation) support** for analyzing real games
- **Extensive test coverage** ensuring reliability
- **Clean, modern C++ (C++17)** with best practices

## 🏗️ Architecture

The library is designed with modularity and performance in mind:

```
src/
├── core/           # Board representation and move generation
├── evaluation/     # Position evaluation algorithms
├── explanation/    # Natural language move explanations
├── notation/       # PGN and algebraic notation handling
└── utils/          # Common utilities
```

## 🚀 Quick Start

```bash
# Clone the repository
git clone https://github.com/yourusername/chess-move-analyzer.git
cd chess-move-analyzer

# Build the project
mkdir build && cd build
cmake ..
make

# Run tests
./tests/run_tests

# Try the CLI tool
./bin/chess-analyzer analyze "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
```

## 📖 Example Usage

```cpp
#include "chess_analyzer.h"

int main() {
    ChessAnalyzer analyzer;
    
    // Load a position (starting position)
    Position pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    // Generate and analyze all legal moves
    auto moves = analyzer.generateMoves(pos);
    
    for (const auto& move : moves) {
        auto explanation = analyzer.explainMove(pos, move);
        std::cout << move.toAlgebraic() << ": " << explanation << std::endl;
    }
    
    // Example output:
    // e4: Controls the center and opens lines for bishop and queen development
    // d4: Classical center control, preparing for piece development
    // Nf3: Develops a knight toward the center while preparing to castle kingside
}
```

## 🧠 Move Explanation Examples

The analyzer provides context-aware explanations:

- **Opening moves**: "Controls the center and enables rapid development"
- **Tactical moves**: "Forks the rook and knight, winning material"
- **Defensive moves**: "Blocks the check while developing the bishop"
- **Endgame moves**: "Centralizes the king for the endgame"

## 🛠️ Technical Highlights

- **Bitboards**: 64-bit integers for ultra-fast board operations
- **Magic Bitboards**: For sliding piece move generation
- **Transposition Tables**: Caching for repeated position analysis
- **Multi-threaded Analysis**: Parallel move evaluation for performance
- **Template Metaprogramming**: Compile-time optimizations

## 📊 Performance

- Generates **~5 million positions/second** on modern hardware
- Evaluates positions with **<1ms latency**
- Handles games with **1000+ moves** without performance degradation

## 🧪 Testing

Comprehensive test suite including:
- Unit tests for all components
- Perft tests for move generation correctness
- Integration tests with real game positions
- Benchmark tests for performance regression

## 📄 License

MIT License - see [LICENSE](LICENSE) for details.

## 🎓 Learning Resources

This project demonstrates:
- Modern C++ best practices (RAII, smart pointers, STL algorithms)
- Chess programming techniques (bitboards, move generation, evaluation)
- Software engineering practices (testing, documentation, CI/CD)
- Performance optimization strategies
