#ifndef COMPLETE_CHESS_MOVE_H
#define COMPLETE_CHESS_MOVE_H

#include <QtGlobal>
#include "chessmove.h"

class CompleteChessMove: public ChessMove
{
public:
	CompleteChessMove();
	
	CompleteChessMove(int sourceSquare,
	                  int targetSquare,
	                  ChessPiece* capture = 0,
	                  ChessPiece::PieceType promotion = ChessPiece::PT_None,
	                  const ChessPiece* promotedPawn = 0,
	                  ChessPiece* castlingRook = 0);
	
	ChessPiece* capture() const;
	ChessPiece* castlingRook() const;
	const ChessPiece* promotedPawn() const;
	int rookSquare() const;

private:
	ChessPiece* m_capture;
	ChessPiece* m_castlingRook;
	const ChessPiece* m_promotedPawn;
	int m_rookSquare;
};

class MoveData
{
	public:
		CompleteChessMove m_move;
		int m_reversibleMoveCount;
		int m_enpassantSquare;
		quint64 m_zobristKey;
		CastlingRights m_castlingRights[2];
};

#endif // COMPLETE_CHESS_MOVE
