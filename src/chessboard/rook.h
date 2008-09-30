#ifndef ROOK_H
#define ROOK_H

#include "chesspiece.h"

class Rook: public ChessPiece
{
	public:
		Rook(Chessboard::ChessSide side, int square, Chessboard* board);
		
		PieceType type() const;
		void disable();
		void makeMove(const CompleteChessMove& move);
	
	private:
		void disableCastlingRights() const;
};

#endif // ROOK
