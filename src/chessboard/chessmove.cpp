#include "chessmove.h"
#include "util.h"

ChessMove::ChessMove()
{
	m_move = 0;
	m_sanMoveString = "";
	m_coordMoveString = "";
}

ChessMove::ChessMove(Chessboard *board,
	             Chessboard::ChessSquare sourceSquare,
	             Chessboard::ChessSquare targetSquare,
	             Chessboard::ChessPiece promotion)
{
	m_move = board->intMove(sourceSquare, targetSquare, promotion);
	if (m_move == 0)
		throw -1; // the move is illegal

	m_coordMoveString = board->coordMoveString(m_move);
	m_sanMoveString = board->sanMoveString(m_move);
}

Chessboard::ChessSquare ChessMove::sourceSquare() const
{
	return (Chessboard::ChessSquare) GET_FROM(m_move);
}

Chessboard::ChessSquare ChessMove::targetSquare() const
{
	return (Chessboard::ChessSquare) GET_TO(m_move);
}

Chessboard::ChessPiece ChessMove::movingPiece() const
{
	return (Chessboard::ChessPiece) GET_PC(m_move);
}

Chessboard::ChessPiece ChessMove::promotion() const
{
	return (Chessboard::ChessPiece) GET_PROM(m_move);
}

Chessboard::ChessPiece ChessMove::capture() const
{
	return (Chessboard::ChessPiece) GET_CAPT(m_move);
}

quint32 ChessMove::integerFormat() const
{
	return m_move;
}

QString ChessMove::sanMoveString() const
{
	return m_sanMoveString;
}

QString ChessMove::coordMoveString() const
{
	return m_coordMoveString;
}

