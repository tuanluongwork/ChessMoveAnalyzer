#include "chess_analyzer.h"
#include "chess_analyzer/core/move_generator.h"
#include "chess_analyzer/evaluation/evaluator.h"
#include "chess_analyzer/explanation/move_explainer.h"
#include "chess_analyzer/notation/pgn_parser.h"
#include <algorithm>
#include <limits>

namespace chess {

class ChessAnalyzer::Impl {
public:
    Impl() : moveGen(), evaluator(), explainer(), pgnParser() {}
    
    MoveGenerator moveGen;
    Evaluator evaluator;
    MoveExplainer explainer;
    PGNParser pgnParser;
    
    // Simple minimax search for finding best move
    struct SearchResult {
        Move move;
        int score;
    };
    
    SearchResult search(const Position& pos, int depth, int alpha, int beta) {
        if (depth == 0) {
            return {NULL_MOVE, evaluator.evaluate(pos)};
        }
        
        std::vector<Move> moves = moveGen.generateLegalMoves(pos);
        
        if (moves.empty()) {
            // No legal moves - checkmate or stalemate
            if (pos.isInCheck()) {
                return {NULL_MOVE, -20000 + depth};  // Checkmate (prefer faster mates)
            } else {
                return {NULL_MOVE, 0};  // Stalemate
            }
        }
        
        SearchResult best = {moves[0], -std::numeric_limits<int>::max()};
        
        // Order moves for better pruning (captures first)
        std::sort(moves.begin(), moves.end(), [&pos](const Move& a, const Move& b) {
            bool aCapture = pos.getPieceAt(a.to()) != NO_PIECE;
            bool bCapture = pos.getPieceAt(b.to()) != NO_PIECE;
            return aCapture > bCapture;
        });
        
        for (const Move& move : moves) {
            Position newPos = pos.makeMove(move);
            SearchResult result = search(newPos, depth - 1, -beta, -alpha);
            result.score = -result.score;
            
            if (result.score > best.score) {
                best.move = move;
                best.score = result.score;
            }
            
            alpha = std::max(alpha, result.score);
            if (alpha >= beta) {
                break;  // Beta cutoff
            }
        }
        
        return best;
    }
};

ChessAnalyzer::ChessAnalyzer() : pImpl(std::make_unique<Impl>()) {}
ChessAnalyzer::~ChessAnalyzer() = default;

std::vector<Move> ChessAnalyzer::generateMoves(const Position& position) const {
    return pImpl->moveGen.generateLegalMoves(position);
}

int ChessAnalyzer::evaluatePosition(const Position& position) const {
    return pImpl->evaluator.evaluate(position);
}

std::string ChessAnalyzer::explainMove(const Position& position, const Move& move) const {
    return pImpl->explainer.explainMove(position, move);
}

Move ChessAnalyzer::findBestMove(const Position& position, int depth) const {
    auto result = pImpl->search(position, depth, 
                               -std::numeric_limits<int>::max(), 
                               std::numeric_limits<int>::max());
    return result.move;
}

std::vector<std::string> ChessAnalyzer::analyzeGame(const std::string& pgn) const {
    std::vector<std::string> analysis;
    
    try {
        Game game = pImpl->pgnParser.parseGame(pgn);
        Position pos(game.initialFEN.empty() ? 
                    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" : 
                    game.initialFEN);
        
        for (const Move& move : game.moves) {
            std::string explanation = explainMove(pos, move);
            analysis.push_back(explanation);
            pos = pos.makeMove(move);
        }
    } catch (const std::exception& e) {
        analysis.push_back("Error parsing game: " + std::string(e.what()));
    }
    
    return analysis;
}

bool ChessAnalyzer::isLegalMove(const Position& position, const Move& move) const {
    return pImpl->moveGen.isLegal(position, move);
}

std::vector<std::string> ChessAnalyzer::getTacticalThemes(const Position& position) const {
    std::vector<std::string> themes;
    
    auto tactics = pImpl->explainer.identifyTactics(position);
    for (auto theme : tactics) {
        themes.push_back(tacticalThemeToString(theme));
    }
    
    return themes;
}

} // namespace chess 