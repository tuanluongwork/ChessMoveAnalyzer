#include "chess_analyzer.h"
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

using namespace chess;

void printUsage(const char* programName) {
    std::cout << "Chess Move Analyzer CLI\n\n";
    std::cout << "Usage: " << programName << " <command> [options]\n\n";
    std::cout << "Commands:\n";
    std::cout << "  analyze <fen>     Analyze a position and explain all legal moves\n";
    std::cout << "  explain <fen> <move>  Explain a specific move in a position\n";
    std::cout << "  best <fen> [depth]    Find the best move in a position\n";
    std::cout << "  game <pgn-file>       Analyze all moves in a PGN game\n";
    std::cout << "  help                  Show this help message\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " analyze \"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\"\n";
    std::cout << "  " << programName << " explain \"startpos\" e2e4\n";
    std::cout << "  " << programName << " best \"r1bqkb1r/pppp1ppp/2n2n2/4p3/2B1P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 4 4\"\n";
}

void analyzePosition(const std::string& fen) {
    try {
        ChessAnalyzer analyzer;
        Position pos(fen == "startpos" ? "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" : fen);
        
        std::cout << "\nPosition Analysis\n";
        std::cout << "================\n";
        std::cout << "FEN: " << pos.toFEN() << "\n";
        std::cout << "Evaluation: " << analyzer.evaluatePosition(pos) << " centipawns\n\n";
        
        // Get tactical themes
        auto themes = analyzer.getTacticalThemes(pos);
        if (!themes.empty()) {
            std::cout << "Tactical Themes: ";
            for (const auto& theme : themes) {
                std::cout << theme << " ";
            }
            std::cout << "\n\n";
        }
        
        // Generate and explain all legal moves
        auto moves = analyzer.generateMoves(pos);
        std::cout << "Legal Moves (" << moves.size() << "):\n";
        std::cout << std::string(80, '-') << "\n";
        
        for (const auto& move : moves) {
            std::string moveStr = move.toAlgebraic(pos);
            std::string explanation = analyzer.explainMove(pos, move);
            
            std::cout << std::left << std::setw(10) << moveStr 
                      << explanation << "\n";
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

void explainMove(const std::string& fen, const std::string& moveStr) {
    try {
        ChessAnalyzer analyzer;
        Position pos(fen == "startpos" ? "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" : fen);
        Move move = Move::fromUCI(moveStr);
        
        if (move.isNull()) {
            std::cerr << "Invalid move notation: " << moveStr << "\n";
            return;
        }
        
        if (!analyzer.isLegalMove(pos, move)) {
            std::cerr << "Illegal move: " << moveStr << "\n";
            return;
        }
        
        std::cout << "\nMove Explanation\n";
        std::cout << "================\n";
        std::cout << "Move: " << move.toAlgebraic(pos) << " (" << moveStr << ")\n\n";
        
        std::string explanation = analyzer.explainMove(pos, move);
        std::cout << explanation << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

void findBestMove(const std::string& fen, int depth = 6) {
    try {
        ChessAnalyzer analyzer;
        Position pos(fen == "startpos" ? "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" : fen);
        
        std::cout << "\nSearching for best move (depth " << depth << ")...\n";
        
        Move bestMove = analyzer.findBestMove(pos, depth);
        
        if (bestMove.isNull()) {
            std::cout << "No legal moves available!\n";
            return;
        }
        
        std::cout << "\nBest Move: " << bestMove.toAlgebraic(pos) 
                  << " (" << bestMove.toUCI() << ")\n\n";
        
        std::string explanation = analyzer.explainMove(pos, bestMove);
        std::cout << "Explanation: " << explanation << "\n";
        
        Position afterMove = pos.makeMove(bestMove);
        int evaluation = analyzer.evaluatePosition(afterMove);
        std::cout << "Evaluation after move: " << evaluation << " centipawns\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string command = argv[1];
    
    if (command == "help" || command == "-h" || command == "--help") {
        printUsage(argv[0]);
        return 0;
    }
    
    if (command == "analyze" && argc >= 3) {
        analyzePosition(argv[2]);
    }
    else if (command == "explain" && argc >= 4) {
        explainMove(argv[2], argv[3]);
    }
    else if (command == "best" && argc >= 3) {
        int depth = (argc >= 4) ? std::stoi(argv[4]) : 6;
        findBestMove(argv[2], depth);
    }
    else if (command == "game" && argc >= 3) {
        std::cout << "PGN analysis not yet implemented\n";
    }
    else {
        std::cerr << "Invalid command or missing arguments\n\n";
        printUsage(argv[0]);
        return 1;
    }
    
    return 0;
} 