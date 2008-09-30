#ifndef CHANCELLOR_H
#define CHANCELLOR_H

#include "chesspiece.h"

class Chancellor: public ChessPiece
{
	public:
		Chancellor(Chessboard::ChessSide side, int square, Chessboard* board);
		
		PieceType type() const;
};

#endif // CHANCELLOR
