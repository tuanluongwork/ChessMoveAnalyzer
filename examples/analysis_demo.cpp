#include "chess_analyzer.h"
#include <iostream>
#include <iomanip>
#include <chrono>

using namespace chess;
using namespace std::chrono;

void printSeparator() {
    std::cout << std::string(80, '=') << "\n";
}

void demonstratePositionAnalysis() {
    std::cout << "\n1. POSITION ANALYSIS DEMO\n";
    printSeparator();
    
    ChessAnalyzer analyzer;
    
    // Italian Game position
    Position pos("r1bqkb1r/pppp1ppp/2n2n2/4p3/2B1P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 4 4");
    
    std::cout << "Position (Italian Game):\n";
    std::cout << "FEN: " << pos.toFEN() << "\n\n";
    
    // Evaluate position
    int eval = analyzer.evaluatePosition(pos);
    std::cout << "Static Evaluation: " << eval << " centipawns ";
    std::cout << "(from " << (pos.getSideToMove() == WHITE ? "White" : "Black") << "'s perspective)\n\n";
    
    // Find best move
    std::cout << "Searching for best move (depth 6)...\n";
    auto start = high_resolution_clock::now();
    Move bestMove = analyzer.findBestMove(pos, 6);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    
    std::cout << "Best move: " << bestMove.toAlgebraic(pos) << " (" << bestMove.toUCI() << ")\n";
    std::cout << "Search time: " << duration.count() << " ms\n\n";
    
    // Explain the best move
    std::string explanation = analyzer.explainMove(pos, bestMove);
    std::cout << "Move explanation: " << explanation << "\n";
}

void demonstrateMoveGeneration() {
    std::cout << "\n2. MOVE GENERATION DEMO\n";
    printSeparator();
    
    ChessAnalyzer analyzer;
    Position pos;  // Starting position
    
    std::cout << "Starting position move generation:\n\n";
    
    auto start = high_resolution_clock::now();
    std::vector<Move> moves = analyzer.generateMoves(pos);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    
    std::cout << "Legal moves: " << moves.size() << "\n";
    std::cout << "Generation time: " << duration.count() << " μs\n\n";
    
    // Group moves by piece type
    std::cout << "Moves by piece:\n";
    
    int pawnMoves = 0, knightMoves = 0, otherMoves = 0;
    for (const Move& move : moves) {
        Piece piece = pos.getPieceAt(move.from());
        switch (typeOf(piece)) {
            case PAWN: pawnMoves++; break;
            case KNIGHT: knightMoves++; break;
            default: otherMoves++; break;
        }
    }
    
    std::cout << "- Pawn moves: " << pawnMoves << "\n";
    std::cout << "- Knight moves: " << knightMoves << "\n";
    std::cout << "- Other moves: " << otherMoves << "\n";
}

void demonstrateTacticalAnalysis() {
    std::cout << "\n3. TACTICAL ANALYSIS DEMO\n";
    printSeparator();
    
    ChessAnalyzer analyzer;
    
    // Position with a fork
    std::cout << "Analyzing tactical position (Knight fork opportunity):\n";
    Position forkPos("r1bqkb1r/pppp1ppp/2n5/4p3/3Pn3/3B1N2/PPP2PPP/RNBQK2R b KQkq - 1 5");
    std::cout << "FEN: " << forkPos.toFEN() << "\n\n";
    
    // Find the forking move
    Move forkMove = Move::fromUCI("e4d2");  // Nxd2+ forking king and queen
    
    if (analyzer.isLegalMove(forkPos, forkMove)) {
        std::cout << "Tactical move: " << forkMove.toAlgebraic(forkPos) << "\n";
        std::string explanation = analyzer.explainMove(forkPos, forkMove);
        std::cout << "Explanation: " << explanation << "\n\n";
        
        // Analyze position after the fork
        Position afterFork = forkPos.makeMove(forkMove);
        std::cout << "Position after fork:\n";
        std::cout << "White is in check: " << (afterFork.isInCheck() ? "Yes" : "No") << "\n";
        std::cout << "Evaluation: " << analyzer.evaluatePosition(afterFork) << " centipawns\n";
    }
}

void demonstrateOpeningAnalysis() {
    std::cout << "\n4. OPENING ANALYSIS DEMO\n";
    printSeparator();
    
    ChessAnalyzer analyzer;
    Position pos;  // Starting position
    
    std::cout << "Analyzing common opening moves:\n\n";
    
    // Analyze popular first moves
    std::vector<std::string> openingMoves = {"e2e4", "d2d4", "g1f3", "c2c4"};
    
    for (const std::string& moveStr : openingMoves) {
        Move move = Move::fromUCI(moveStr);
        if (analyzer.isLegalMove(pos, move)) {
            std::cout << std::left << std::setw(8) << move.toAlgebraic(pos);
            std::cout << analyzer.explainMove(pos, move) << "\n";
        }
    }
}

void demonstrateEndgameAnalysis() {
    std::cout << "\n5. ENDGAME ANALYSIS DEMO\n";
    printSeparator();
    
    ChessAnalyzer analyzer;
    
    // King and pawn endgame
    Position endgame("8/8/8/8/4k3/8/4P3/4K3 w - - 0 1");
    std::cout << "King and Pawn Endgame:\n";
    std::cout << "FEN: " << endgame.toFEN() << "\n\n";
    
    // Generate all moves
    std::vector<Move> moves = analyzer.generateMoves(endgame);
    
    std::cout << "Available moves and evaluations:\n";
    for (const Move& move : moves) {
        Position afterMove = endgame.makeMove(move);
        int eval = analyzer.evaluatePosition(afterMove);
        
        std::cout << std::left << std::setw(8) << move.toAlgebraic(endgame);
        std::cout << "Eval: " << std::setw(6) << eval;
        std::cout << " | " << analyzer.explainMove(endgame, move) << "\n";
    }
}

void demonstratePerformance() {
    std::cout << "\n6. PERFORMANCE BENCHMARK\n";
    printSeparator();
    
    ChessAnalyzer analyzer;
    
    // Complex middlegame position
    Position complex("r2q1rk1/ppp2ppp/2n1bn2/2bpp3/3P4/2N1PN2/PPP1BPPP/R1BQK2R w KQ - 0 8");
    
    std::cout << "Benchmarking complex position:\n";
    std::cout << "FEN: " << complex.toFEN() << "\n\n";
    
    // Benchmark move generation
    const int iterations = 10000;
    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        volatile auto moves = analyzer.generateMoves(complex);
    }
    
    auto end = high_resolution_clock::now();
    auto totalTime = duration_cast<milliseconds>(end - start);
    
    std::cout << "Move generation benchmark:\n";
    std::cout << "- Iterations: " << iterations << "\n";
    std::cout << "- Total time: " << totalTime.count() << " ms\n";
    std::cout << "- Average time: " << (totalTime.count() / static_cast<double>(iterations)) << " ms\n";
    std::cout << "- Positions/second: " << (iterations * 1000.0 / totalTime.count()) << "\n\n";
    
    // Benchmark evaluation
    start = high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        volatile int eval = analyzer.evaluatePosition(complex);
    }
    
    end = high_resolution_clock::now();
    totalTime = duration_cast<milliseconds>(end - start);
    
    std::cout << "Position evaluation benchmark:\n";
    std::cout << "- Iterations: " << iterations << "\n";
    std::cout << "- Total time: " << totalTime.count() << " ms\n";
    std::cout << "- Average time: " << (totalTime.count() / static_cast<double>(iterations)) << " ms\n";
    std::cout << "- Evaluations/second: " << (iterations * 1000.0 / totalTime.count()) << "\n";
}

int main() {
    std::cout << "CHESS MOVE ANALYZER - COMPREHENSIVE DEMO\n";
    printSeparator();
    std::cout << "This demo showcases the capabilities of the Chess Move Analyzer library.\n";
    
    try {
        demonstratePositionAnalysis();
        demonstrateMoveGeneration();
        demonstrateTacticalAnalysis();
        demonstrateOpeningAnalysis();
        demonstrateEndgameAnalysis();
        demonstratePerformance();
        
        std::cout << "\n";
        printSeparator();
        std::cout << "Demo completed successfully!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
} 