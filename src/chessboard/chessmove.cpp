#include "chessmove.h"
#include "notation.h"
#include "util.h"

ChessMove::ChessMove()
{
	m_move = NULLMOVE;
	m_sanMoveString = "";
	m_coordMoveString = "";
}

ChessMove::ChessMove(Chessboard *board,
	             Chessboard::ChessSquare sourceSquare,
	             Chessboard::ChessSquare targetSquare,
	             Chessboard::ChessPiece promotion)
{
	m_move = board->intMove(sourceSquare, targetSquare, promotion);
	if (m_move == NULLMOVE)
		throw -1; // the move is illegal

	m_coordMoveString = board->coordMoveString(m_move);
	m_sanMoveString = board->sanMoveString(m_move);
}

ChessMove::ChessMove(Chessboard *board, quint32 move)
{
	m_move = move;
	m_coordMoveString = board->coordMoveString(m_move);
	m_sanMoveString = board->sanMoveString(m_move);
}

ChessMove::ChessMove(Chessboard *board, const QString& moveString)
{
	if (isMoveString(moveString)) {
		m_move = board->moveFromCoord(moveString);
		m_coordMoveString = moveString;
		m_sanMoveString = board->sanMoveString(m_move);
	} else {
		m_move = board->moveFromSan(moveString);
		m_sanMoveString = moveString;
		m_coordMoveString = board->coordMoveString(m_move);
	}
	
	if (m_move == NULLMOVE || m_move == MOVE_ERROR)
		throw -1;
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

