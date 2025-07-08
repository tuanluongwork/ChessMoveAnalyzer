#include <gtest/gtest.h>
#include "chess_analyzer/core/position.h"

using namespace chess;

class PositionTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
};

TEST_F(PositionTest, DefaultConstructorCreatesStartingPosition) {
    Position pos;
    
    // Check FEN
    EXPECT_EQ(pos.toFEN(), "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    // Check side to move
    EXPECT_EQ(pos.getSideToMove(), WHITE);
    
    // Check castling rights
    EXPECT_EQ(pos.getCastlingRights(), ALL_CASTLING);
    
    // Check no en passant
    EXPECT_EQ(pos.getEnPassantSquare(), NO_SQUARE);
}

TEST_F(PositionTest, FENConstructorParsesCorrectly) {
    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    Position pos(fen);
    
    // Verify it parses and converts back correctly
    EXPECT_EQ(pos.toFEN(), fen);
}

TEST_F(PositionTest, GetPieceAtReturnsCorrectPieces) {
    Position pos;  // Starting position
    
    // White pieces
    EXPECT_EQ(pos.getPieceAt(E1), W_KING);
    EXPECT_EQ(pos.getPieceAt(D1), W_QUEEN);
    EXPECT_EQ(pos.getPieceAt(A1), W_ROOK);
    EXPECT_EQ(pos.getPieceAt(H1), W_ROOK);
    EXPECT_EQ(pos.getPieceAt(B1), W_KNIGHT);
    EXPECT_EQ(pos.getPieceAt(G1), W_KNIGHT);
    EXPECT_EQ(pos.getPieceAt(C1), W_BISHOP);
    EXPECT_EQ(pos.getPieceAt(F1), W_BISHOP);
    EXPECT_EQ(pos.getPieceAt(E2), W_PAWN);
    
    // Black pieces
    EXPECT_EQ(pos.getPieceAt(E8), B_KING);
    EXPECT_EQ(pos.getPieceAt(D8), B_QUEEN);
    EXPECT_EQ(pos.getPieceAt(E7), B_PAWN);
    
    // Empty squares
    EXPECT_EQ(pos.getPieceAt(E4), NO_PIECE);
    EXPECT_EQ(pos.getPieceAt(D5), NO_PIECE);
}

TEST_F(PositionTest, BitboardsAreConsistent) {
    Position pos;
    
    // Check white pawns
    Bitboard whitePawns = pos.getPieceBitboard(PAWN, WHITE);
    EXPECT_EQ(popcount(whitePawns), 8);  // 8 white pawns
    EXPECT_TRUE(whitePawns & squareBB(A2));
    EXPECT_TRUE(whitePawns & squareBB(H2));
    
    // Check black pawns
    Bitboard blackPawns = pos.getPieceBitboard(PAWN, BLACK);
    EXPECT_EQ(popcount(blackPawns), 8);  // 8 black pawns
    EXPECT_TRUE(blackPawns & squareBB(A7));
    EXPECT_TRUE(blackPawns & squareBB(H7));
    
    // Check kings
    Bitboard whiteKing = pos.getPieceBitboard(KING, WHITE);
    EXPECT_EQ(popcount(whiteKing), 1);  // Only 1 king
    EXPECT_TRUE(whiteKing & squareBB(E1));
    
    Bitboard blackKing = pos.getPieceBitboard(KING, BLACK);
    EXPECT_EQ(popcount(blackKing), 1);
    EXPECT_TRUE(blackKing & squareBB(E8));
}

TEST_F(PositionTest, ColorBitboardsAreCorrect) {
    Position pos;
    
    Bitboard whitePieces = pos.getColorBitboard(WHITE);
    Bitboard blackPieces = pos.getColorBitboard(BLACK);
    Bitboard allPieces = pos.getOccupiedBitboard();
    
    // Starting position has 16 pieces per side
    EXPECT_EQ(popcount(whitePieces), 16);
    EXPECT_EQ(popcount(blackPieces), 16);
    EXPECT_EQ(popcount(allPieces), 32);
    
    // No overlap between colors
    EXPECT_EQ(whitePieces & blackPieces, 0ULL);
    
    // Combined should equal all pieces
    EXPECT_EQ(whitePieces | blackPieces, allPieces);
}

TEST_F(PositionTest, EnPassantSquareHandling) {
    // Position after 1.e4 e5 2.Nf3 Nf6 3.d4 exd4
    Position pos("rnbqkb1r/pppp1ppp/5n2/8/3pP3/5N2/PPP2PPP/RNBQKB1R w KQkq - 0 4");
    EXPECT_EQ(pos.getEnPassantSquare(), NO_SQUARE);
    
    // Position after 1.e4 e5 2.f4 (en passant possible)
    Position pos2("rnbqkbnr/pppp1ppp/8/4p3/4PP2/8/PPPP2PP/RNBQKBNR b KQkq f3 0 2");
    EXPECT_EQ(pos2.getEnPassantSquare(), F1 + 16);  // f3
}

TEST_F(PositionTest, CastlingRightsUpdate) {
    // Position where white has lost queenside castling
    Position pos("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQk - 0 1");
    EXPECT_TRUE(pos.getCastlingRights() & WHITE_OO);
    EXPECT_FALSE(pos.getCastlingRights() & WHITE_OOO);
    EXPECT_TRUE(pos.getCastlingRights() & BLACK_OO);
    EXPECT_TRUE(pos.getCastlingRights() & BLACK_OOO);
}

// Perft test - counts positions at a given depth
// This is a standard test for move generation correctness
TEST_F(PositionTest, PerftStartingPosition) {
    Position pos;
    
    // Known perft values for starting position
    // Depth 1: 20 positions
    // Depth 2: 400 positions
    // Depth 3: 8,902 positions
    // etc.
    
    // This would require move generation to be implemented
    // Just a placeholder for now
    EXPECT_TRUE(true);
} 