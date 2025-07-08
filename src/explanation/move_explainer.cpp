#include "chess_analyzer/explanation/move_explainer.h"
#include "chess_analyzer/core/move_generator.h"
#include <sstream>
#include <algorithm>

namespace chess {

class MoveExplainer::Impl {
public:
    int detailLevel = 1;
    int targetRating = 1500;
    
    std::string generateExplanation(const Position& pos, const Move& move) const;
    std::string analyzeTactics(const Position& pos, const Move& move) const;
    std::string analyzeStrategy(const Position& pos, const Move& move) const;
};

MoveExplainer::MoveExplainer() : pImpl(std::make_unique<Impl>()) {}
MoveExplainer::~MoveExplainer() = default;

std::string MoveExplainer::explainMove(const Position& position, const Move& move) const {
    return pImpl->generateExplanation(position, move);
}

std::string MoveExplainer::Impl::generateExplanation(const Position& pos, const Move& move) const {
    std::ostringstream explanation;
    
    // Get basic move info
    Piece movedPiece = pos.getPieceAt(move.from());
    PieceType pieceType = typeOf(movedPiece);
    std::string pieceName;
    
    switch (pieceType) {
        case PAWN: pieceName = "pawn"; break;
        case KNIGHT: pieceName = "knight"; break;
        case BISHOP: pieceName = "bishop"; break;
        case ROOK: pieceName = "rook"; break;
        case QUEEN: pieceName = "queen"; break;
        case KING: pieceName = "king"; break;
        default: pieceName = "piece";
    }
    
    // Special moves
    if (move.isCastling()) {
        if (move.to() > move.from()) {
            explanation << "Castles kingside, bringing the king to safety ";
            explanation << "while activating the rook";
        } else {
            explanation << "Castles queenside, securing the king ";
            explanation << "while bringing the rook to the center";
        }
        return explanation.str();
    }
    
    // Basic move description
    explanation << "Moves the " << pieceName;
    explanation << " from " << squareToString(move.from());
    explanation << " to " << squareToString(move.to());
    
    // Check if it's a capture
    Piece capturedPiece = pos.getPieceAt(move.to());
    if (capturedPiece != NO_PIECE || move.isEnPassant()) {
        explanation << ", capturing ";
        if (move.isEnPassant()) {
            explanation << "the pawn en passant";
        } else {
            PieceType capturedType = typeOf(capturedPiece);
            switch (capturedType) {
                case PAWN: explanation << "a pawn"; break;
                case KNIGHT: explanation << "a knight"; break;
                case BISHOP: explanation << "a bishop"; break;
                case ROOK: explanation << "a rook"; break;
                case QUEEN: explanation << "the queen"; break;
                default: explanation << "a piece";
            }
        }
    }
    
    // Check for promotion
    if (move.isPromotion()) {
        explanation << " and promotes to a ";
        switch (move.promotionType()) {
            case PROMOTE_TO_QUEEN: explanation << "queen"; break;
            case PROMOTE_TO_ROOK: explanation << "rook"; break;
            case PROMOTE_TO_BISHOP: explanation << "bishop"; break;
            case PROMOTE_TO_KNIGHT: explanation << "knight"; break;
        }
    }
    
    // Add strategic/tactical analysis based on detail level
    if (detailLevel >= 1) {
        std::string tactics = analyzeTactics(pos, move);
        if (!tactics.empty()) {
            explanation << ". " << tactics;
        }
    }
    
    if (detailLevel >= 2) {
        std::string strategy = analyzeStrategy(pos, move);
        if (!strategy.empty()) {
            explanation << ". " << strategy;
        }
    }
    
    return explanation.str();
}

std::string MoveExplainer::Impl::analyzeTactics(const Position& pos, const Move& move) const {
    std::ostringstream tactics;
    
    // Check if move gives check
    Position afterMove = pos.makeMove(move);
    if (afterMove.isInCheck()) {
        tactics << "This move gives check";
        
        // TODO: Check for checkmate patterns
        // if (isCheckmate) {
        //     tactics << " and delivers checkmate!";
        // }
    }
    
    // TODO: Implement tactical pattern recognition
    // - Forks
    // - Pins
    // - Skewers
    // - Discovered attacks
    
    return tactics.str();
}

std::string MoveExplainer::Impl::analyzeStrategy(const Position& pos, const Move& move) const {
    std::ostringstream strategy;
    
    // Analyze based on game phase
    int pieceCount = popcount(pos.getOccupiedBitboard());
    
    if (pos.getFullmoveNumber() <= 10) {
        // Opening phase
        if (typeOf(pos.getPieceAt(move.from())) == PAWN) {
            Square to = move.to();
            if (fileOf(to) >= 3 && fileOf(to) <= 4 && rankOf(to) >= 3 && rankOf(to) <= 4) {
                strategy << "Controls the center";
            }
        } else if (typeOf(pos.getPieceAt(move.from())) == KNIGHT) {
            strategy << "Develops a piece toward the center";
        }
    } else if (pieceCount <= 14) {
        // Endgame phase
        if (typeOf(pos.getPieceAt(move.from())) == KING) {
            strategy << "Activates the king for the endgame";
        }
    }
    
    return strategy.str();
}

void MoveExplainer::setDetailLevel(int level) {
    pImpl->detailLevel = std::max(0, std::min(2, level));
}

void MoveExplainer::setTargetAudience(int rating) {
    pImpl->targetRating = rating;
}

// Helper functions
std::string tacticalThemeToString(TacticalTheme theme) {
    switch (theme) {
        case TacticalTheme::PIN: return "Pin";
        case TacticalTheme::FORK: return "Fork";
        case TacticalTheme::SKEWER: return "Skewer";
        case TacticalTheme::DISCOVERED_ATTACK: return "Discovered Attack";
        case TacticalTheme::DOUBLE_ATTACK: return "Double Attack";
        case TacticalTheme::REMOVE_DEFENDER: return "Remove Defender";
        case TacticalTheme::DEFLECTION: return "Deflection";
        case TacticalTheme::DECOY: return "Decoy";
        case TacticalTheme::INTERFERENCE: return "Interference";
        case TacticalTheme::ZUGZWANG: return "Zugzwang";
        case TacticalTheme::STALEMATE_TRAP: return "Stalemate Trap";
        case TacticalTheme::PERPETUAL_CHECK: return "Perpetual Check";
        case TacticalTheme::BACK_RANK_MATE: return "Back Rank Mate";
        case TacticalTheme::SMOTHERED_MATE: return "Smothered Mate";
        default: return "Unknown Tactic";
    }
}

std::string strategicConceptToString(StrategicConcept concept) {
    switch (concept) {
        case StrategicConcept::CENTER_CONTROL: return "Center Control";
        case StrategicConcept::PIECE_DEVELOPMENT: return "Piece Development";
        case StrategicConcept::KING_SAFETY: return "King Safety";
        case StrategicConcept::PAWN_STRUCTURE: return "Pawn Structure";
        case StrategicConcept::PIECE_ACTIVITY: return "Piece Activity";
        case StrategicConcept::SPACE_ADVANTAGE: return "Space Advantage";
        case StrategicConcept::WEAK_SQUARES: return "Weak Squares";
        case StrategicConcept::OPEN_FILES: return "Open Files";
        case StrategicConcept::OUTPOST: return "Outpost";
        case StrategicConcept::MINORITY_ATTACK: return "Minority Attack";
        case StrategicConcept::PAWN_BREAK: return "Pawn Break";
        case StrategicConcept::PIECE_COORDINATION: return "Piece Coordination";
        case StrategicConcept::INITIATIVE: return "Initiative";
        case StrategicConcept::TIME_ADVANTAGE: return "Time Advantage";
        default: return "Unknown Concept";
    }
}

// TODO: Implement remaining methods
std::vector<TacticalTheme> MoveExplainer::identifyTactics(const Position& position) const {
    return {};
}

std::vector<StrategicConcept> MoveExplainer::identifyStrategicConcepts(
    const Position& position, const Move& move) const {
    return {};
}

std::string MoveExplainer::explainImmediateEffects(const Position& position, const Move& move) const {
    return pImpl->generateExplanation(position, move);
}

std::string MoveExplainer::explainPositionalImpact(const Position& position, const Move& move) const {
    return pImpl->analyzeStrategy(position, move);
}

std::string MoveExplainer::explainOpeningMove(const Position& position, const Move& move) const {
    return "";
}

std::string MoveExplainer::explainEndgameMove(const Position& position, const Move& move) const {
    return "";
}

} // namespace chess 