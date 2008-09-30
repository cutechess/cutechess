#include "queen.h"
#include "chessmove.h"


Queen::Queen(Chessboard::ChessSide side, int square, Chessboard* board)
: ChessPiece(side, square, board)
{
	int vStep = board->arrayWidth();
	m_hopOffsetCount = 0;
	m_slideOffsetCount = 8;

	m_slideOffsets[0] = -vStep + 1;
	m_slideOffsets[1] = -vStep - 1;
	m_slideOffsets[2] = vStep - 1;
	m_slideOffsets[3] = vStep + 1;
	m_slideOffsets[4] = -vStep;
	m_slideOffsets[5] = -1;
	m_slideOffsets[6] = 1;
	m_slideOffsets[7] = vStep;
}

ChessPiece::PieceType Queen::type() const
{
	return PT_Queen;
}
