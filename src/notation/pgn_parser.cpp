#include "chess_analyzer/notation/pgn_parser.h"
#include "chess_analyzer/core/move_generator.h"
#include <sstream>
#include <regex>
#include <cctype>

namespace chess {

class PGNParser::Impl {
public:
    mutable std::string lastError;
    
    Game parseGameImpl(const std::string& pgn) const {
        Game game;
        lastError.clear();
        
        std::istringstream stream(pgn);
        std::string line;
        std::string moveText;
        
        // Parse headers
        while (std::getline(stream, line)) {
            if (line.empty()) break;
            
            if (line[0] == '[') {
                // Parse header
                std::regex headerRegex(R"(\[(\w+)\s+"([^"]+)"\])");
                std::smatch match;
                if (std::regex_match(line, match, headerRegex)) {
                    game.headers[match[1]] = match[2];
                    
                    if (match[1] == "FEN") {
                        game.initialFEN = match[2];
                    } else if (match[1] == "Result") {
                        game.result = match[2];
                    }
                }
            } else {
                // Start of move text
                moveText = line;
                break;
            }
        }
        
        // Continue reading move text
        while (std::getline(stream, line)) {
            moveText += " " + line;
        }
        
        // Parse moves
        game.moves = parseMoveText(moveText, game.initialFEN);
        
        return game;
    }
    
    std::vector<Move> parseMoveText(const std::string& moveText, const std::string& initialFEN) const {
        std::vector<Move> moves;
        Position pos(initialFEN.empty() ? 
                    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" : 
                    initialFEN);
        
        // Remove comments and variations
        std::string cleanText = removeCommentsAndVariations(moveText);
        
        // Tokenize
        std::vector<std::string> tokens = tokenize(cleanText);
        
        for (const std::string& token : tokens) {
            // Skip move numbers and results
            if (std::isdigit(token[0]) || token == "1-0" || token == "0-1" || 
                token == "1/2-1/2" || token == "*") {
                continue;
            }
            
            Move move = parseAlgebraicMoveImpl(pos, token);
            if (!move.isNull()) {
                moves.push_back(move);
                pos = pos.makeMove(move);
            } else {
                lastError = "Invalid move: " + token;
                break;
            }
        }
        
        return moves;
    }
    
    Move parseAlgebraicMoveImpl(const Position& pos, const std::string& moveStr) const {
        if (moveStr.empty()) return NULL_MOVE;
        
        // Handle castling
        if (moveStr == "O-O" || moveStr == "0-0") {
            Square kingSquare = pos.getSideToMove() == WHITE ? E1 : E8;
            Square targetSquare = pos.getSideToMove() == WHITE ? G1 : G8;
            return Move(kingSquare, targetSquare, CASTLING);
        }
        if (moveStr == "O-O-O" || moveStr == "0-0-0") {
            Square kingSquare = pos.getSideToMove() == WHITE ? E1 : E8;
            Square targetSquare = pos.getSideToMove() == WHITE ? C1 : C8;
            return Move(kingSquare, targetSquare, CASTLING);
        }
        
        // Parse standard algebraic notation
        std::string str = moveStr;
        
        // Remove check/checkmate symbols
        if (str.back() == '+' || str.back() == '#') {
            str.pop_back();
        }
        
        // Extract promotion
        PromotionType promotion = PROMOTE_TO_QUEEN;
        bool isPromotion = false;
        if (str.length() >= 2 && str[str.length() - 2] == '=') {
            isPromotion = true;
            switch (std::tolower(str.back())) {
                case 'q': promotion = PROMOTE_TO_QUEEN; break;
                case 'r': promotion = PROMOTE_TO_ROOK; break;
                case 'b': promotion = PROMOTE_TO_BISHOP; break;
                case 'n': promotion = PROMOTE_TO_KNIGHT; break;
                default: return NULL_MOVE;
            }
            str = str.substr(0, str.length() - 2);
        }
        
        // Extract capture
        bool isCapture = false;
        size_t capturePos = str.find('x');
        if (capturePos != std::string::npos) {
            isCapture = true;
            str.erase(capturePos, 1);
        }
        
        // Extract destination square
        if (str.length() < 2) return NULL_MOVE;
        
        std::string destStr = str.substr(str.length() - 2);
        Square to = stringToSquare(destStr);
        if (to == NO_SQUARE) return NULL_MOVE;
        
        str = str.substr(0, str.length() - 2);
        
        // Determine piece type
        PieceType pieceType = PAWN;
        if (!str.empty() && std::isupper(str[0])) {
            switch (str[0]) {
                case 'N': pieceType = KNIGHT; break;
                case 'B': pieceType = BISHOP; break;
                case 'R': pieceType = ROOK; break;
                case 'Q': pieceType = QUEEN; break;
                case 'K': pieceType = KING; break;
                default: return NULL_MOVE;
            }
            str = str.substr(1);
        }
        
        // Find the piece that can make this move
        MoveGenerator gen;
        std::vector<Move> legalMoves = gen.generateLegalMoves(pos);
        std::vector<Move> candidates;
        
        for (const Move& move : legalMoves) {
            if (move.to() != to) continue;
            
            Piece piece = pos.getPieceAt(move.from());
            if (typeOf(piece) != pieceType) continue;
            
            if (isCapture && pos.getPieceAt(to) == NO_PIECE && !move.isEnPassant()) continue;
            if (!isCapture && (pos.getPieceAt(to) != NO_PIECE || move.isEnPassant())) continue;
            
            if (isPromotion && !move.isPromotion()) continue;
            if (!isPromotion && move.isPromotion()) continue;
            
            if (isPromotion && move.promotionType() != promotion) continue;
            
            candidates.push_back(move);
        }
        
        // Disambiguation
        if (candidates.size() > 1 && !str.empty()) {
            std::vector<Move> filtered;
            
            for (const Move& move : candidates) {
                bool matches = true;
                
                for (char c : str) {
                    if (c >= 'a' && c <= 'h') {
                        // File disambiguation
                        if (fileOf(move.from()) != c - 'a') {
                            matches = false;
                            break;
                        }
                    } else if (c >= '1' && c <= '8') {
                        // Rank disambiguation
                        if (rankOf(move.from()) != c - '1') {
                            matches = false;
                            break;
                        }
                    }
                }
                
                if (matches) {
                    filtered.push_back(move);
                }
            }
            
            candidates = filtered;
        }
        
        return candidates.size() == 1 ? candidates[0] : NULL_MOVE;
    }
    
private:
    std::string removeCommentsAndVariations(const std::string& text) const {
        std::string result;
        int braceLevel = 0;
        int parenLevel = 0;
        
        for (size_t i = 0; i < text.length(); ++i) {
            char c = text[i];
            
            if (c == '{') {
                braceLevel++;
            } else if (c == '}') {
                braceLevel--;
            } else if (c == '(') {
                parenLevel++;
            } else if (c == ')') {
                parenLevel--;
            } else if (braceLevel == 0 && parenLevel == 0) {
                result += c;
            }
        }
        
        return result;
    }
    
    std::vector<std::string> tokenize(const std::string& text) const {
        std::vector<std::string> tokens;
        std::istringstream stream(text);
        std::string token;
        
        while (stream >> token) {
            // Split move numbers from moves (e.g., "1.e4" -> "1." and "e4")
            size_t dotPos = token.find('.');
            if (dotPos != std::string::npos && dotPos + 1 < token.length()) {
                tokens.push_back(token.substr(0, dotPos + 1));
                tokens.push_back(token.substr(dotPos + 1));
            } else {
                tokens.push_back(token);
            }
        }
        
        return tokens;
    }
};

PGNParser::PGNParser() : pImpl(std::make_unique<Impl>()) {}
PGNParser::~PGNParser() = default;

std::vector<Game> PGNParser::parsePGN(const std::string& pgn) const {
    std::vector<Game> games;
    
    // Split multiple games
    size_t pos = 0;
    while (pos < pgn.length()) {
        size_t nextGame = pgn.find("\n\n[", pos);
        std::string gameText;
        
        if (nextGame != std::string::npos) {
            gameText = pgn.substr(pos, nextGame - pos);
            pos = nextGame + 2;
        } else {
            gameText = pgn.substr(pos);
            pos = pgn.length();
        }
        
        if (!gameText.empty()) {
            games.push_back(parseGame(gameText));
        }
    }
    
    return games;
}

Game PGNParser::parseGame(const std::string& pgn) const {
    return pImpl->parseGameImpl(pgn);
}

std::string PGNParser::gameToPGN(const Game& game) const {
    std::ostringstream pgn;
    
    // Write headers
    for (const auto& [key, value] : game.headers) {
        pgn << "[" << key << " \"" << value << "\"]\n";
    }
    
    if (!game.headers.empty()) {
        pgn << "\n";
    }
    
    // Write moves
    Position pos(game.initialFEN.empty() ? 
                "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" : 
                game.initialFEN);
    
    for (size_t i = 0; i < game.moves.size(); ++i) {
        if (i % 2 == 0) {
            pgn << (i / 2 + 1) << ". ";
        }
        
        pgn << moveToAlgebraic(pos, game.moves[i]) << " ";
        pos = pos.makeMove(game.moves[i]);
        
        if ((i + 1) % 2 == 0 && i + 1 < game.moves.size()) {
            pgn << "\n";
        }
    }
    
    if (!game.result.empty()) {
        pgn << game.result;
    }
    
    return pgn.str();
}

Move PGNParser::parseAlgebraicMove(const Position& position, const std::string& moveStr) const {
    return pImpl->parseAlgebraicMoveImpl(position, moveStr);
}

std::string PGNParser::moveToAlgebraic(const Position& position, const Move& move) const {
    return move.toAlgebraic(position);
}

bool PGNParser::validatePGN(const std::string& pgn) const {
    try {
        parseGame(pgn);
        return pImpl->lastError.empty();
    } catch (...) {
        return false;
    }
}

std::string PGNParser::getLastError() const {
    return pImpl->lastError;
}

} // namespace chess 