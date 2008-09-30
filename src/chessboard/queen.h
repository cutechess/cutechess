#ifndef QUEEN_H
#define QUEEN_H

#include "chesspiece.h"

class Queen: public ChessPiece
{
	public:
		Queen(Chessboard::ChessSide side, int square, Chessboard* board);
		
		PieceType type() const;
};

#endif // QUEEN
