#ifndef KNIGHT_H
#define KNIGHT_H

#include "chesspiece.h"

class Knight: public ChessPiece
{
	public:
		Knight(Chessboard::ChessSide side, int square, Chessboard* board);

		PieceType type() const;
};

#endif // KNIGHT
