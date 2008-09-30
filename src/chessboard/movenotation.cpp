#include <QChar>
#include <QString>
#include "chessboard.h"
#include "completechessmove.h"
#include "chesspiece.h"

QString Chessboard::stringFromSquare(int square) const
{
	Q_ASSERT(isValidSquare(square));
	
	QString rank = QString::number(m_height - (square / arrayWidth()));
	QString file(QChar('a' + (square % arrayWidth()) - 1));
	
	return file + rank;
}

int Chessboard::squareFromString(const QString& str) const
{
	if (str.length() != 2)
		return -1;
	int file = (str[0].toAscii() - 'a') + 1;
	int rank = (m_height - str[1].digitValue());
	
	int square = (rank * arrayWidth()) + file;
	if (!isValidSquare(square))
		return -1;
	
	return square;
}

QString Chessboard::coordStringFromMove(const CompleteChessMove& move)
{
	if (variant() == FischerRandomChess && move.rookSquare() != -1)
	{
		if (move.rookSquare() < move.sourceSquare())
			return "O-O-O";
		else
			return "O-O";
	}

	QString str = stringFromSquare(move.sourceSquare()) +
		stringFromSquare(move.targetSquare());
	if (move.promotion() != ChessPiece::PT_None)
		str += ChessPiece::charFromPieceType(move.promotion());
	
	return str;
}

CompleteChessMove Chessboard::moveFromCoordString(const QString& str)
{
	if (variant() == FischerRandomChess && str.startsWith("O-O"))
		return moveFromSanString(str);
	
	if (str.length() < 4)
		return CompleteChessMove();
	
	int source = squareFromString(str.mid(0, 2));
	int target = squareFromString(str.mid(2, 2));
	if (source == -1 || target == -1)
		return CompleteChessMove();
	
	ChessPiece::PieceType promotion = ChessPiece::PT_None;
	if (str.length() > 4)
	{
		promotion = ChessPiece::pieceTypeFromChar(str[4]);
		if (promotion == ChessPiece::PT_None)
			return CompleteChessMove();
	}

	ChessPiece* piece = pieceAt(source);
	if (piece == 0)
		return CompleteChessMove();
	
	QList<CompleteChessMove> moves;
	piece->generateMoves(&moves);
	foreach (CompleteChessMove move, moves)
	{
		if (!isMoveLegal(move))
			continue;
		if (variant() == FischerRandomChess && move.rookSquare() != -1)
			continue;
		if (move.targetSquare() == target
		&&  move.promotion() == promotion)
			return move;
	}
	
	return CompleteChessMove();
}

QString Chessboard::sanStringFromMove(const CompleteChessMove& move)
{
	QString str;
	ChessPiece *piece = pieceAt(move.sourceSquare());
	Q_ASSERT(piece != 0);
	ChessPiece::PieceType pieceType = piece->type();

	int rank = move.sourceSquare() / arrayWidth();
	int file = move.sourceSquare() % arrayWidth();
	bool needRank = false;
	bool needFile = false;

	if (pieceType == ChessPiece::PT_Pawn)
	{
		if (move.capture())
			needFile = true;
	}
	else if (pieceType == ChessPiece::PT_King)
	{
		if (move.rookSquare() != -1)
		{
			if (move.rookSquare() < move.sourceSquare())
				str = "O-O-O";
			else
				str = "O-O";
			if (isMoveCheck(move))
			{
				if (isMoveMate(move))
					str += "#";
				else
					str += "+";
			}
			return str;
		}
		else
			str += ChessPiece::charFromPieceType(pieceType);
	}
	else
	{
		str += ChessPiece::charFromPieceType(pieceType);

		QList<CompleteChessMove> moves;
		foreach (ChessPiece* piece, m_pieces[m_side])
		{
			if (piece->square() == move.sourceSquare()
			||  piece->type() != pieceType)
				continue;
			piece->generateMoves(&moves);
		}

		foreach (CompleteChessMove tmpMove, moves)
		{
			if (!isMoveLegal(tmpMove))
				continue;
			if (tmpMove.targetSquare() == move.targetSquare())
			{
				int rank2 = tmpMove.sourceSquare() / arrayWidth();
				int file2 = tmpMove.sourceSquare() % arrayWidth();
				if (file2 != file)
					needFile = true;
				else if (rank2 != rank)
					needRank = true;
			}
		}
	}
	if (needFile)
		str += QChar('a' + (file - 1));
	if (needRank)
		str += QString::number(m_height - rank);

	if (move.capture())
		str += "x";
	
	str += stringFromSquare(move.targetSquare());

	if (move.promotion())
		str += QString("=") + ChessPiece::charFromPieceType(move.promotion());

	if (isMoveCheck(move))
	{
		if (isMoveMate(move))
			str += "#";
		else
			str += "+";
	}
	
	return str;
}

CompleteChessMove Chessboard::moveFromSanString(const QString& str)
{
	QString moveString = str;
	while (moveString.endsWith('+') || moveString.endsWith('#') ||
	       moveString.endsWith('!') || moveString.endsWith('?'))
	{
		moveString.chop(1);
	}
	
	if (moveString.length() < 2)
		return CompleteChessMove();

	const QChar *c = moveString.data();

	if (moveString.startsWith("O-O"))
	{
		int cside;
		if (moveString == "O-O")
			cside = CastlingRights::RightSide;
		else if (moveString == "O-O-O")
			cside = CastlingRights::LeftSide;
		else
			return CompleteChessMove();
		
		QList<CompleteChessMove> moves;
		m_king[m_side]->generateMoves(&moves);
		foreach (CompleteChessMove move, moves)
		{
			if (!isMoveLegal(move))
				continue;
			if (move.targetSquare() == castlingRights()->kingTarget(cside)
			&&  move.rookSquare() != -1)
				return move;
		}
		return CompleteChessMove();
	}
	
	int target = -1;
	int file = -1;
	int rank = -1;
	
	// piece type
	ChessPiece::PieceType pieceType = ChessPiece::pieceTypeFromChar(*c);
	if (pieceType == ChessPiece::PT_None)
	{
		pieceType = ChessPiece::PT_Pawn;
		target = squareFromString(QString(c, 2));
		c += 2;
	}
	else
		c++;

	if (target == -1)
	{
		// source square's file
		file = (c->toAscii() - 'a') + 1;
		if (file <= 0 || file >= arrayWidth())
			file = -1;
		else
			c++;
		if (c->isNull())
			return CompleteChessMove();

		// source square's rank
		if (c->isDigit())
		{
			rank = m_height - c->digitValue();
			if (rank < 0 || rank >= m_height)
				return CompleteChessMove();
			c++;
		}
		if (c->isNull())
		{
			if (rank != -1 && file != -1)
			{
				target = (rank * arrayWidth()) + file;
				rank = -1;
				file = -1;
			}
			else
				return CompleteChessMove();
		}
		
		// capture
		if (!c->isNull() && *c == 'x')
		{
			c++;
			if (c->isNull())
				return CompleteChessMove();
		}
		
		// target square
		if (target == -1)
		{
			if (c->isNull() || (c + 1)->isNull())
				return CompleteChessMove();
			target = squareFromString(QString(c, 2));
			c += 2;
		}
	}
	
	if (target == -1)
		return CompleteChessMove();
	
	// promotion
	ChessPiece::PieceType promotion = ChessPiece::PT_None;
	if (!c->isNull())
	{
		if (*c == '=')
		{
			c++;
			if (c->isNull())
				return CompleteChessMove();
			promotion = ChessPiece::pieceTypeFromChar(*c);
			if (promotion == ChessPiece::PT_None)
				return CompleteChessMove();
		}
		else
			return CompleteChessMove();
	}

	foreach (ChessPiece* piece, m_pieces[m_side])
	{
		if (piece->type() != pieceType)
			continue;
		if (rank != -1 && (piece->square() / arrayWidth()) != rank)
			continue;
		if (file != -1 && (piece->square() % arrayWidth()) != file)
			continue;
		
		QList<CompleteChessMove> moves;
		piece->generateMoves(&moves);
		foreach (CompleteChessMove move, moves)
		{
			if (!isMoveLegal(move))
				continue;
			if (move.targetSquare() == target)
				return move;
		}
	}

	return CompleteChessMove();
}

QString Chessboard::moveString(const ChessMove& move, MoveNotation notation)
{
	if (notation == LongAlgebraic)
		return coordStringFromMove(completeMove(move));
	return sanStringFromMove(completeMove(move));
}

ChessMove Chessboard::moveFromString(const QString& str)
{
	CompleteChessMove move = moveFromCoordString(str);
	if (move.isEmpty())
		move = moveFromSanString(str);
	
	if (move.isEmpty())
		return ChessMove();

	int source_rank = move.sourceSquare() / arrayWidth();
	int source_file = (move.sourceSquare() % arrayWidth()) - 1;
	int source_square = (source_rank * m_width) + source_file;

	int target_rank = move.targetSquare() / arrayWidth();
	int target_file = (move.targetSquare() % arrayWidth()) - 1;
	int target_square = (target_rank * m_width) + target_file;

	return ChessMove(source_square, target_square, move.promotion());
}
