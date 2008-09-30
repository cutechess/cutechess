#include "chessmove.h"


ChessMove::ChessMove()
{
	m_sourceSquare = -1;
	m_targetSquare = -1;
	m_promotion = ChessPiece::PT_None;
	m_isEmpty = true;
}

ChessMove::ChessMove(int sourceSquare,
	             int targetSquare,
	             ChessPiece::PieceType promotion)
{
	m_sourceSquare = sourceSquare;
	m_targetSquare = targetSquare;
	m_promotion = promotion;
	m_isEmpty = false;
}

int ChessMove::sourceSquare() const
{
	return m_sourceSquare;
}

int ChessMove::targetSquare() const
{
	return m_targetSquare;
}

ChessPiece::PieceType ChessMove::promotion() const
{
	return m_promotion;
}

bool ChessMove::isEmpty() const
{
	return m_isEmpty;
}
