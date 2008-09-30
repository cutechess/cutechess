#ifndef KING_H
#define KING_H

#include "chesspiece.h"

class King: public ChessPiece
{
	public:
		King(Chessboard::ChessSide side, int square, Chessboard* board);
		
		PieceType type() const;

		void generateMoves(QList<CompleteChessMove>* moves) const;

		// castling moves
		void makeMove(const CompleteChessMove& move);
	
	private:
		bool canCastle(int castleSide) const;
};

#endif // KING_H
