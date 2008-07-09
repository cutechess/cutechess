#include "chessmove.h"
#include "notation.h"
#include "util.h"

ChessMove::ChessMove()
{
	m_sourceSquare = Chessboard::NoSquare;
	m_targetSquare = Chessboard::NoSquare;
	m_promotion = Chessboard::NoPiece;
}

ChessMove::ChessMove(Chessboard::ChessSquare sourceSquare,
	             Chessboard::ChessSquare targetSquare,
	             Chessboard::ChessPiece promotion)
{
	m_sourceSquare = sourceSquare;
	m_targetSquare = targetSquare;
	if (promotion == (Chessboard::ChessPiece)0)
		promotion = Chessboard::NoPiece;
	m_promotion = promotion;
}

Chessboard::ChessSquare ChessMove::sourceSquare() const
{
	return m_sourceSquare;
}

Chessboard::ChessSquare ChessMove::targetSquare() const
{
	return m_targetSquare;
}

Chessboard::ChessPiece ChessMove::promotion() const
{
	return m_promotion;
}

