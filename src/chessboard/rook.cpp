#include "rook.h"
#include "chessmove.h"
#include "zobrist.h"


Rook::Rook(Chessboard::ChessSide side, int square, Chessboard* board)
: ChessPiece(side, square, board)
{
	int vStep = board->arrayWidth();
	m_hopOffsetCount = 0;
	m_slideOffsetCount = 4;

	m_slideOffsets[0] = -vStep;
	m_slideOffsets[1] = -1;
	m_slideOffsets[2] = 1;
	m_slideOffsets[3] = vStep;
}

ChessPiece::PieceType Rook::type() const
{
	return PT_Rook;
}

void Rook::disableCastlingRights() const
{
	const CastlingRights* cr = m_board->castlingRights(m_side);
	for (int cside = 0; cside < 2; cside++)
	{
		if (this == cr->rook(cside))
		{
			m_board->disableCastlingRights(m_side, cside);
			m_board->updateZobristKey(Zobrist::castling(m_side, cside));
		}
	}
}

void Rook::disable()
{
	ChessPiece::disable();
	disableCastlingRights();
}

void Rook::makeMove(const CompleteChessMove& move)
{
	ChessPiece::makeMove(move);
	disableCastlingRights();
}
