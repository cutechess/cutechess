#include "knight.h"
#include "completechessmove.h"


Knight::Knight(Chessboard::ChessSide side, int square, Chessboard* board)
: ChessPiece(side, square, board)
{
	int vStep = board->arrayWidth();
	m_hopOffsetCount = 8;
	m_slideOffsetCount = 0;

	m_hopOffsets[0] = -vStep * 2 - 1;
	m_hopOffsets[1] = -vStep * 2 + 1;
	m_hopOffsets[2] = -vStep - 2;
	m_hopOffsets[3] = -vStep + 2;
	m_hopOffsets[4] = vStep - 2;
	m_hopOffsets[5] = vStep + 2;
	m_hopOffsets[6] = vStep * 2 - 1;
	m_hopOffsets[7] = vStep * 2 + 1;
}

ChessPiece::PieceType Knight::type() const
{
	return PT_Knight;
}
