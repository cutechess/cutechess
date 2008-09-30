#ifndef ARCHBISHOP_H
#define ARCHBISHOP_H

#include "chesspiece.h"

class Archbishop: public ChessPiece
{
	public:
		Archbishop(Chessboard::ChessSide side, int square, Chessboard* board);
		
		PieceType type() const;
};

#endif // ARCHBISHOP
