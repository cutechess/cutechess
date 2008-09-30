#include <QStringList>
#include <QChar>
#include "fen.h"
#include "chesspiece.h"
#include "zobrist.h"

Fen::Fen(Chessboard* board, const QString& fenString)
{
	m_board = board;
	m_boardSize = board->width() * board->height();
	m_squares.resize(m_boardSize);
	m_reversibleMoveCount = 0;
	m_moveCount = 0;
	m_isValid = false;
	
	QStringList fenItems = fenString.split(' ', QString::SkipEmptyParts);
	if (fenItems.count() < 4)
		return;

	if (!getSquares(fenItems[0]))
		return;
	getRookSquares();

	if (fenItems[1] == "w")
		m_side = Chessboard::White;
	else if (fenItems[1] == "b")
		m_side = Chessboard::Black;
	else
		return;

	foreach (QChar c, fenItems[2])
	{
		if (c == '-')
			break;
		else if (c == 'K')
		{
			if (!addCastlingRights(Chessboard::White, CastlingRights::RightSide))
				return;
		}
		else if (c == 'Q')
		{
			if (!addCastlingRights(Chessboard::White, CastlingRights::LeftSide))
				return;
		}
		else if (c == 'k')
		{
			if (!addCastlingRights(Chessboard::Black, CastlingRights::RightSide))
				return;
		}
		else if (c == 'q')
		{
			if (!addCastlingRights(Chessboard::Black, CastlingRights::LeftSide))
				return;
		}
		else
		{
			Chessboard::ChessSide tmpSide = (c.isUpper()) ? Chessboard::White : Chessboard::Black;
			int file = c.toLower().toAscii() - 'a';
			if (file < 0 || file >= m_board->width())
				return;
			int rank = (tmpSide == Chessboard::White) ? m_board->height() - 1 : 0;
			int square = (rank * m_board->width()) + file;
			int cside;
			if (square < m_kingSquares[tmpSide])
				cside = CastlingRights::LeftSide;
			else
				cside = CastlingRights::RightSide;
			m_rookSquares[tmpSide][cside] = square;
			
			if (!addCastlingRights(tmpSide, cside))
				return;
		}
	}
	
	m_enpassantSquare = m_board->squareFromString(fenItems[3]);
	if (m_enpassantSquare == -1 && fenItems[3] != "-")
		return;
	
	if (fenItems.count() > 4)
	{
		bool ok;
		m_reversibleMoveCount = fenItems[4].toInt(&ok);
		if (!ok || m_reversibleMoveCount < 0)
			return;
	}
	
	if (fenItems.count() > 5)
	{
		bool ok;
		m_moveCount = fenItems[5].toInt(&ok);
		if (!ok || m_moveCount <= 0)
			return;
		m_moveCount = (m_moveCount - 1) * 2;
		if (m_board->side() == Chessboard::Black)
			m_moveCount++;
	}
	
	m_isValid = true;
}

bool Fen::addCastlingRights(Chessboard::ChessSide side, int castlingSide)
{
	int kingRank = m_kingSquares[side] / m_board->width();
	if (side == Chessboard::White && kingRank != m_board->height() - 1)
		return false;
	if (side == Chessboard::Black && kingRank != 0)
		return false;
	
	int rookSquare = m_rookSquares[side][castlingSide];
	if (rookSquare == -1)
		return false;
	
	int sign = (side == Chessboard::White) ? 1 : -1;
	if (m_squares[rookSquare] != ChessPiece::PT_Rook * sign)
		return false;
	
	m_castlingRights[side].enable(castlingSide);
	return true;
}

bool Fen::isValid() const
{
	return m_isValid;
}

void Fen::apply()
{
	Q_ASSERT(isValid());
	
	m_board->m_side = m_side;
	m_board->m_moveCount = m_moveCount;
	m_board->setReversibleMoveCount(m_reversibleMoveCount);
	m_board->setEnpassantSquare(m_enpassantSquare);

	m_board->clear();
	for (int i = 0; i < m_boardSize; i++)
	{
		Chessboard::ChessSide pieceSide = (m_squares[i] >= 0) ? Chessboard::White : Chessboard::Black;
		ChessPiece::PieceType type = (ChessPiece::PieceType)abs(m_squares[i]);
		int square = m_board->squareToIndex(i);

		if (type == ChessPiece::PT_None)
			m_board->setSquare(square, 0);
		else
		{
			ChessPiece* newPiece = ChessPiece::fromType(pieceSide, type, square, m_board);
			Q_CHECK_PTR(newPiece);
			m_board->addPiece(newPiece);
		}
	}

	int kingTargets[2][2];
	kingTargets[Chessboard::White][CastlingRights::LeftSide] = (m_boardSize - m_board->width()) + 2;
	kingTargets[Chessboard::White][CastlingRights::RightSide] = m_boardSize - 2;
	kingTargets[Chessboard::Black][CastlingRights::LeftSide] = 2;
	kingTargets[Chessboard::Black][CastlingRights::RightSide] = m_board->width() - 2;
	
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			if (!m_castlingRights[i].canCastle(j))
				continue;
			int kingTarget = m_board->squareToIndex(kingTargets[i][j]);
			int rookSquare = m_board->squareToIndex(m_rookSquares[i][j]);
			m_castlingRights[i].setKingTarget(kingTarget, j);
			m_castlingRights[i].setRook(m_board->pieceAt(rookSquare), j);
		}
	}
	
	m_board->setCastlingRights(Chessboard::White, m_castlingRights[Chessboard::White]);
	m_board->setCastlingRights(Chessboard::Black, m_castlingRights[Chessboard::Black]);
	m_board->setZobristKey(Zobrist::key(m_board));
}

void Fen::getRookSquares()
{
	bool foundRook[2][2] = { {false, false}, {false, false} };
	
	for (int i = 0; i < 2; i++)
	{
		int rank = m_kingSquares[i] / m_board->width();
		int start = rank * m_board->width();
		int end = start + m_board->width();

		for (int j = start; j < end; j++)
		{
			int piece = m_squares[j];
			Chessboard::ChessSide side = (piece >= 0) ? Chessboard::White : Chessboard::Black;
			ChessPiece::PieceType type = (ChessPiece::PieceType)abs(piece);
			if (side == i && type == ChessPiece::PT_Rook)
			{
				int cside;
				if (j < m_kingSquares[i])
					cside = CastlingRights::LeftSide;
				else
					cside = CastlingRights::RightSide;
				if (foundRook[i][cside])
					m_rookSquares[i][cside] = -1;
				else
				{
					m_rookSquares[i][cside] = j;
					foundRook[i][cside] = true;
				}
			}
		}
	}
}

/* Convert a FEN board into mailbox format.  */
bool Fen::getSquares(const QString& fen)
{
	int square = 0;
	int rankEndSquare = 0;	/* last square of the previous rank */
	int width = m_board->width();

	if (fen.length() < 15)
		return false;

	m_kingSquares[Chessboard::White] = -1;
	m_kingSquares[Chessboard::Black] = -1;
	m_rookSquares[Chessboard::White][CastlingRights::LeftSide] = -1;
	m_rookSquares[Chessboard::White][CastlingRights::RightSide] = -1;
	m_rookSquares[Chessboard::Black][CastlingRights::LeftSide] = -1;
	m_rookSquares[Chessboard::Black][CastlingRights::RightSide] = -1;
	
	for (int i = 0; i < fen.length(); i++)
	{
		QChar c = fen[i];
		
		/* Move to the next rank.  */
		if (c == '/')
		{
			/* Reject the FEN string if the rank didn't
			   have exactly 'width()' squares.  */
			if (square - rankEndSquare != width)
				return false;
			rankEndSquare = square;
			continue;
		}
		/* Add empty squares.  */
		if (c.isDigit())
		{
			int j;
			int nempty;
			if (i < (fen.length() - 1) && fen[i + 1].isDigit())
			{
				nempty = fen.mid(i, 2).toInt();
				i++;
			}
			else
				nempty = c.digitValue();
			
			if (nempty > width || square + nempty > m_boardSize)
				return false;
			for (j = 0; j < nempty; j++)
				m_squares[square++] = 0;
			continue;
		}
		
		if (square >= m_boardSize)
			return false;
		
		ChessPiece::PieceType type = ChessPiece::pieceTypeFromChar(c.toUpper());
		if (type == ChessPiece::PT_None)
			return false;
		Chessboard::ChessSide side = (c.isUpper()) ? Chessboard::White : Chessboard::Black;
		int sign = (side == Chessboard::White) ? 1 : -1;
		if (type == ChessPiece::PT_King)
		{
			if (m_kingSquares[side] != -1)
				return false;
			m_kingSquares[side] = square;
		}
		
		m_squares[square++] = (int)type * sign;
	}
	/* The board must have exactly 'boardSize' squares and each rank
	   must have exactly 'width()' squares.  */
	if (square != m_boardSize || square - rankEndSquare != width)
		return false;

	if (m_kingSquares[Chessboard::White] == -1 || m_kingSquares[Chessboard::Black] == -1)
		return false;

	return true;
}

QString Fen::fenString(const Chessboard* board)
{
	QString str;
	int emptyCount = 0;
	int boardSize = board->width() * board->height();
	
	for (int i = 0; i < boardSize; i++)
	{
		int square = board->squareToIndex(i);
		Q_ASSERT(board->isValidSquare(square));

		ChessPiece* piece = board->pieceAt(square);
		if (piece == 0)
			emptyCount++;
		
		bool lastFile = ((i % board->width()) == board->width() - 1);
		if (lastFile || piece != 0)
		{
			if (emptyCount > 0)
				str += QString::number(emptyCount);
			emptyCount = 0;
			if (piece != 0)
				str += piece->toString();
			if (lastFile && i < boardSize - 1)
				str += "/";
		}
	}
	
	if (board->side() == Chessboard::White)
		str += " w ";
	else
		str += " b ";
	
	QString castling;
	const QChar castlingSymbols[2] = { 'Q', 'K' };
	for (int side = 0; side < 2; side++)
	{
		const CastlingRights* cr = board->castlingRights((Chessboard::ChessSide)side);
		QChar c;
		for (int cside = 1; cside >= 0; cside--)
		{
			if (!cr->canCastle(cside))
				continue;
			
			// Use the Shredder FEN notation for FRC games
			if (board->variant() == Chessboard::FischerRandomChess)
			{
				int rookSquare = cr->rook(cside)->square();
				int file = (rookSquare % board->arrayWidth()) - 1;
				c = 'a' + file;
			}
			else
				c = castlingSymbols[cside];
			
			if (side == Chessboard::White)
				c = c.toUpper();
			else
				c = c.toLower();
			castling += c;
		}
	}
	if (castling.isEmpty())
		castling = '-';
	str += castling + ' ';
	
	if (board->enpassantSquare() != -1)
		str += board->stringFromSquare(board->enpassantSquare()) + " ";
	else
		str += "- ";
	
	str += QString::number(board->reversibleMoveCount()) + " ";
	
	str += QString::number((board->m_moveCount / 2) + 1);
	
	return str;
}
