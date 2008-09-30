#include "chesspiece.h"
#include "pawn.h"
#include "knight.h"
#include "bishop.h"
#include "rook.h"
#include "archbishop.h"
#include "chancellor.h"
#include "queen.h"
#include "king.h"
#include "completechessmove.h"


ChessPiece::ChessPiece(Chessboard::ChessSide side, int square, Chessboard* board)
{
	Q_ASSERT(side != Chessboard::NoSide);
	Q_ASSERT(board->isValidSquare(square));
	
	m_enabled = true;
	m_side = side;
	m_square = square;
	m_board = board;
}

QChar ChessPiece::charFromPieceType(PieceType type)
{
	switch (type)
	{
	case PT_Pawn:
		return 'P';
	case PT_Knight:
		return 'N';
	case PT_Bishop:
		return 'B';
	case PT_Rook:
		return 'R';
	case PT_Queen:
		return 'Q';
	case PT_King:
		return 'K';
	case PT_Archbishop:
		return 'A';
	case PT_Chancellor:
		return 'C';
	default:
		qDebug("charFromPieceType: Invalid piece type");
	}

	return '\0';
}

ChessPiece::PieceType ChessPiece::pieceTypeFromChar(const QChar& c)
{
	switch (c.toAscii())
	{
	case 'P':
		return PT_Pawn;
	case 'N':
		return PT_Knight;
	case 'B':
		return PT_Bishop;
	case 'R':
		return PT_Rook;
	case 'Q':
		return PT_Queen;
	case 'K':
		return PT_King;
	case 'A':
		return PT_Archbishop;
	case 'C':
		return PT_Chancellor;
	}

	return PT_None;
}

ChessPiece* ChessPiece::fromType(Chessboard::ChessSide side, PieceType type, int square, Chessboard* board)
{
	switch (type)
	{
	case PT_Pawn:
		return new Pawn(side, square, board);
	case PT_Knight:
		return new Knight(side, square, board);
	case PT_Bishop:
		return new Bishop(side, square, board);
	case PT_Rook:
		return new Rook(side, square, board);
	case PT_Queen:
		return new Queen(side, square, board);
	case PT_King:
		return new King(side, square, board);
	case PT_Archbishop:
		return new Archbishop(side, square, board);
	case PT_Chancellor:
		return new Chancellor(side, square, board);
	default:
		qDebug("fromType: Invalid piece type");
	}

	return 0;
}

ChessPiece* ChessPiece::fromChar(const QChar& c, int square, Chessboard* board)
{
	Chessboard::ChessSide side = (c.isUpper()) ? Chessboard::White : Chessboard::Black;
	PieceType type = pieceTypeFromChar(c.toUpper());
	
	return ChessPiece::fromType(side, type, square, board);
}

QString ChessPiece::toString() const
{
	QChar c = charFromPieceType(type());
	if (m_side == Chessboard::White)
		return c.toUpper();
	return c.toLower();
}

bool ChessPiece::canAttack(int square) const
{
	for (int i = 0; i < m_hopOffsetCount; i++)
	{
		if (square == (m_square + m_hopOffsets[i]))
			return true;
	}
	
	for (int i = 0; i < m_slideOffsetCount; i++)
	{
		int target = m_square + m_slideOffsets[i];

		while (m_board->isValidSquare(target))
		{
			if (square == target)
				return true;
			if (m_board->pieceAt(target) != 0)
				break;
			target += m_slideOffsets[i];
		}
	}

	return false;
}

void ChessPiece::generateMoves(QList<CompleteChessMove>* moves) const
{
	for (int i = 0; i < m_hopOffsetCount; i++)
	{
		int target = m_square + m_hopOffsets[i];
		if (!m_board->isValidSquare(target))
			continue;
		ChessPiece* capture = m_board->pieceAt(target);
		if (capture == 0 || capture->side() != m_side)
			moves->append(CompleteChessMove(m_square, target, capture));
	}

	for (int i = 0; i < m_slideOffsetCount; i++)
	{
		int target = m_square + m_slideOffsets[i];

		while (m_board->isValidSquare(target))
		{
			ChessPiece* capture = m_board->pieceAt(target);
			if (capture != 0 && capture->side() == m_side)
				break;
			
			moves->append(CompleteChessMove(m_square, target, capture));
			if (capture != 0)
				break;
			
			target += m_slideOffsets[i];
		}
	}
}

void ChessPiece::makeMove(const CompleteChessMove& move)
{
	if (move.capture())
		move.capture()->disable();

	setSquare(move.targetSquare());
	m_board->setEnpassantSquare(-1);
}

void ChessPiece::undoMove(const CompleteChessMove& move)
{
	setSquare(move.sourceSquare());

	if (move.capture() != 0)
		move.capture()->enable();
}

void ChessPiece::setSquare(int square)
{
	Q_ASSERT(m_board->isValidSquare(square));
	
	if (m_board->pieceAt(m_square) == this)
		m_board->setSquare(m_square, 0);
	
	m_board->setSquare(square, this);
	m_square = square;
}

void ChessPiece::disable()
{
	m_enabled = false;
	m_board->removePiece(this);
	if (m_board->pieceAt(m_square) == this)
		m_board->setSquare(m_square, 0);
}

void ChessPiece::enable()
{
	m_enabled = true;
	m_board->addPiece(this);
	if (m_board->pieceAt(m_square) == 0)
		m_board->setSquare(m_square, this);
}

Chessboard::ChessSide ChessPiece::side() const
{
	return m_side;
}

int ChessPiece::square() const
{
	return m_square;
}

bool ChessPiece::enabled() const
{
	return m_enabled;
}
