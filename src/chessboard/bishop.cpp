#include "bishop.h"


Bishop::Bishop(Chessboard::ChessSide side, int square, Chessboard* board)
: ChessPiece(side, square, board)
{
	int vStep = board->arrayWidth();
	m_hopOffsetCount = 0;
	m_slideOffsetCount = 4;

	m_slideOffsets[0] = -vStep + 1;
	m_slideOffsets[1] = -vStep - 1;
	m_slideOffsets[2] = vStep - 1;
	m_slideOffsets[3] = vStep + 1;
}

ChessPiece::PieceType Bishop::type() const
{
	return PT_Bishop;
}
