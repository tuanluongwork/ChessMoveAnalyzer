#include "chess_analyzer.h"
#include <iostream>
#include <chrono>
#include <vector>

using namespace chess;
using namespace std::chrono;

// Perft (Performance Test) - counts all leaf nodes at a given depth
// This is the standard test for move generation correctness and performance
struct PerftResult {
    uint64_t nodes;
    uint64_t captures;
    uint64_t enPassant;
    uint64_t castles;
    uint64_t promotions;
    uint64_t checks;
    uint64_t checkmates;
};

class PerftTester {
public:
    PerftResult perft(const Position& pos, int depth) {
        if (depth == 0) {
            return {1, 0, 0, 0, 0, 0, 0};
        }
        
        PerftResult result = {0, 0, 0, 0, 0, 0, 0};
        ChessAnalyzer analyzer;
        std::vector<Move> moves = analyzer.generateMoves(pos);
        
        for (const Move& move : moves) {
            Position newPos = pos.makeMove(move);
            
            if (depth == 1) {
                result.nodes++;
                
                // Count move types
                if (pos.getPieceAt(move.to()) != NO_PIECE) {
                    result.captures++;
                }
                if (move.isEnPassant()) {
                    result.enPassant++;
                }
                if (move.isCastling()) {
                    result.castles++;
                }
                if (move.isPromotion()) {
                    result.promotions++;
                }
                if (newPos.isInCheck()) {
                    result.checks++;
                }
                
                // Check for checkmate
                std::vector<Move> replies = analyzer.generateMoves(newPos);
                if (replies.empty() && newPos.isInCheck()) {
                    result.checkmates++;
                }
            } else {
                PerftResult subResult = perft(newPos, depth - 1);
                result.nodes += subResult.nodes;
                result.captures += subResult.captures;
                result.enPassant += subResult.enPassant;
                result.castles += subResult.castles;
                result.promotions += subResult.promotions;
                result.checks += subResult.checks;
                result.checkmates += subResult.checkmates;
            }
        }
        
        return result;
    }
    
    void runPerftSuite() {
        struct TestPosition {
            std::string name;
            std::string fen;
            std::vector<uint64_t> expectedNodes;  // Expected nodes at each depth
        };
        
        std::vector<TestPosition> testPositions = {
            {
                "Starting Position",
                "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                {1, 20, 400, 8902, 197281, 4865609}
            },
            {
                "Kiwipete",
                "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
                {1, 48, 2039, 97862, 4085603}
            },
            {
                "Position 3",
                "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
                {1, 14, 191, 2812, 43238, 674624}
            },
            {
                "Position 4",
                "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
                {1, 6, 264, 9467, 422333}
            },
            {
                "Position 5",
                "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
                {1, 44, 1486, 62379, 2103487}
            }
        };
        
        std::cout << "PERFT TEST SUITE\n";
        std::cout << "================\n\n";
        
        for (const auto& test : testPositions) {
            std::cout << "Testing: " << test.name << "\n";
            std::cout << "FEN: " << test.fen << "\n\n";
            
            Position pos(test.fen);
            
            for (size_t depth = 1; depth <= test.expectedNodes.size() && depth <= 5; ++depth) {
                auto start = high_resolution_clock::now();
                PerftResult result = perft(pos, depth);
                auto end = high_resolution_clock::now();
                auto duration = duration_cast<milliseconds>(end - start);
                
                bool passed = (result.nodes == test.expectedNodes[depth - 1]);
                
                std::cout << "Depth " << depth << ": ";
                std::cout << result.nodes << " nodes";
                
                if (depth <= test.expectedNodes.size()) {
                    std::cout << " (expected: " << test.expectedNodes[depth - 1] << ")";
                    std::cout << " [" << (passed ? "PASS" : "FAIL") << "]";
                }
                
                std::cout << " - " << duration.count() << " ms";
                
                if (duration.count() > 0) {
                    uint64_t nps = (result.nodes * 1000) / duration.count();
                    std::cout << " (" << nps << " nps)";
                }
                
                std::cout << "\n";
                
                if (!passed) {
                    std::cout << "ERROR: Node count mismatch!\n";
                    // Detailed breakdown for debugging
                    if (depth == 1) {
                        std::cout << "  Captures: " << result.captures << "\n";
                        std::cout << "  En passant: " << result.enPassant << "\n";
                        std::cout << "  Castles: " << result.castles << "\n";
                        std::cout << "  Promotions: " << result.promotions << "\n";
                        std::cout << "  Checks: " << result.checks << "\n";
                    }
                }
            }
            
            std::cout << "\n";
        }
    }
};

int main() {
    std::cout << "Chess Move Analyzer - Performance Test (Perft)\n";
    std::cout << "=============================================\n\n";
    
    PerftTester tester;
    
    try {
        tester.runPerftSuite();
        
        std::cout << "Performance test completed.\n";
        std::cout << "\nNote: This implementation uses a simplified move generator.\n";
        std::cout << "Full perft compliance requires complete chess rules implementation.\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
} 