#ifndef PAWN_H
#define PAWN_H

#include "chesspiece.h"

class Pawn: public ChessPiece
{
public:
	Pawn(Chessboard::ChessSide side, int square, Chessboard* board);

	PieceType type() const;

	void generateMoves(QList<CompleteChessMove>* moves) const;
	bool canAttack(int square) const;

	// promotions and en passant captures
	void makeMove(const CompleteChessMove& move);
	void undoMove(const CompleteChessMove& move);

private:
	void addMoves(int target, ChessPiece* capture, bool isPromotion, QList<CompleteChessMove>* moves) const;
	
	int m_sign;
	int m_offsets[2];
	int m_captureOffsets[2];
	int m_epOffsets[2];
};

#endif // PAWN
