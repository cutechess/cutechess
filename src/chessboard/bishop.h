#ifndef BISHOP_H
#define BISHOP_H

#include "chesspiece.h"

class Bishop: public ChessPiece
{
	public:
		Bishop(Chessboard::ChessSide side, int square, Chessboard* board);
		
		PieceType type() const;
};

#endif // BISHOP
