#include "completechessmove.h"
#include "chesspiece.h"


CompleteChessMove::CompleteChessMove()
: ChessMove()
{
	m_capture = 0;
}

CompleteChessMove::CompleteChessMove(int sourceSquare,
	int targetSquare,
	ChessPiece* capture,
	ChessPiece::PieceType promotion,
	const ChessPiece* promotedPawn,
	ChessPiece* castlingRook)
: ChessMove(sourceSquare, targetSquare, promotion)
{
	m_capture = capture;
	m_castlingRook = castlingRook;
	m_promotedPawn = promotedPawn;
	if (m_castlingRook != 0)
		m_rookSquare = m_castlingRook->square();
	else
		m_rookSquare = -1;
}

ChessPiece* CompleteChessMove::capture() const
{
	return m_capture;
}

ChessPiece* CompleteChessMove::castlingRook() const
{
	return m_castlingRook;
}

const ChessPiece* CompleteChessMove::promotedPawn() const
{
	return m_promotedPawn;
}

int CompleteChessMove::rookSquare() const
{
	return m_rookSquare;
}
