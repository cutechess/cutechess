/*
    This file is part of Cute Chess.

    Cute Chess is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Cute Chess is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Cute Chess.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QStringList>
#include "chessboard.h"

using namespace Chess;


Board::FenData::FenData()
	: isRandom(false)
{
	cr.rookSquare[White][QueenSide] = 0;
	cr.rookSquare[White][KingSide] = 0;
	cr.rookSquare[Black][QueenSide] = 0;
	cr.rookSquare[Black][KingSide] = 0;
	kingSquare[White] = 0;
	kingSquare[Black] = 0;
}

bool Board::parseCastlingRights(FenData& fd, QChar c) const
{
	int offset = 0;
	int cside = -1;
	Side side = (c.isUpper()) ? White : Black;
	c = c.toLower();
	
	if (c == 'q')
	{
		cside = QueenSide;
		offset = -1;
	}
	else if (c == 'k')
	{
		cside = KingSide;
		offset = 1;
	}
	
	int kingSq = fd.kingSquare[side];
	
	if (offset != 0)
	{
		Piece piece;
		int i = kingSq + offset;
		int rookSq = 0;
		
		// Locate the outernmost rook on the castling side
		while (!(piece = fd.squares[i]).isWall())
		{
			if (piece == Piece(side, Piece::Rook))
				rookSq = i;
			i += offset;
		}
		if (rookSq != 0)
		{
			fd.cr.rookSquare[side][cside] = rookSq;
			// If the rook is not in the corner, we've got
			// a random chess variant like Chess960.
			if (rookSq + offset != i)
				fd.isRandom = true;
			return true;
		}
	}
	else	// Shredder FEN or X-FEN
	{
		fd.isRandom = true;
		
		int file = c.toAscii() - 'a';
		if (file < 0 || file >= m_width)
			return false;
		
		// Get the rook's source square
		int rookSq;
		if (side == White)
			rookSq = (m_height + 1) * m_arwidth + 1 + file;
		else
			rookSq = 2 * m_arwidth + 1 + file;
		
		// Make sure the king and the rook are on the same rank
		if (abs(kingSq - rookSq) >= m_width)
			return false;
		
		// Update castling rights in the FenData object
		if (fd.squares[rookSq] == Piece(side, Piece::Rook))
		{
			cside = (rookSq > kingSq);
			fd.cr.rookSquare[side][cside] = rookSq;
			return true;
		}
	}
	
	return false;
}

bool Board::setFenString(const QString& fen)
{
	QStringList strList = fen.split(' ');
	if (strList.size() < 4 || strList.size() > 6)
		return false;
	
	QStringList::iterator token = strList.begin();
	
	if (token->length() < 15)
		return false;
	
	int square = 0;
	int rankEndSquare = 0;	// last square of the previous rank
	int boardSize = m_width * m_height;
	int k = m_arwidth * 2 + 1;
	
	FenData fd;
	fd.squares.clear();
	for (int i = 0; i < m_squares.size(); i++)
		fd.squares.append(Piece::WallPiece);
	
	// Get the board contents (squares)
	for (int i = 0; i < token->length(); i++)
	{
		QChar c = token->at(i);
		
		// Move to the next rank
		if (c == '/')
		{
			// Reject the FEN string if the rank didn't
			// have exactly 'm_width' squares.
			if (square - rankEndSquare != m_width)
				return false;
			rankEndSquare = square;
			k += 2;
			continue;
		}
		// Add empty squares
		if (c.isDigit())
		{
			int j;
			int nempty;
			if (i < (token->length() - 1) && token->at(i + 1).isDigit())
			{
				nempty = token->mid(i, 2).toInt();
				i++;
			}
			else
				nempty = c.digitValue();
			
			if (nempty > m_width || square + nempty > boardSize)
				return false;
			for (j = 0; j < nempty; j++)
			{
				square++;
				fd.squares[k++] = Piece::NoPiece;
			}
			continue;
		}
		
		if (square >= boardSize)
			return false;
		
		Piece piece(c);
		if (!piece.isValid())
			return false;

		if (piece.type() == Piece::King)
		{
			// Make sure 'side' has exactly one king
			if (fd.kingSquare[piece.side()] != 0)
				return false;
			fd.kingSquare[piece.side()] = k;
		}
		
		square++;
		fd.squares[k++] = piece;
	}
	
	// The board must have exactly 'boardSize' squares and each rank
	// must have exactly 'm_width' squares.
	if (square != boardSize || square - rankEndSquare != m_width)
		return false;
	
	// Both players need a king
	if (fd.kingSquare[White] == 0 || fd.kingSquare[Black] == 0)
		return false;
	
	// Side to move
	++token;
	Side side;
	if (*token == "w")
		side = White;
	else if (*token == "b")
		side = Black;
	else
		return false;
	
	// Castling rights
	++token;
	if (*token != "-")
	{
		QString::const_iterator c;
		for (c = token->begin(); c != token->end(); ++c)
		{
			if (!parseCastlingRights(fd, *c))
				return false;
		}
	}
	
	// En-passant square
	++token;
	int epSq = 0;
	if (*token != "-")
	{
		epSq = squareIndex(*token);
		if (epSq == 0)
			return false;
		
		// Ignore the en-passant square if an en-passant
		// capture isn't possible.
		int sign = (side == White) ? 1 : -1;
		int pawnSq = epSq + m_arwidth * sign;
		if ((fd.squares[pawnSq - 1].code() * sign) != Piece::Pawn
		&&  (fd.squares[pawnSq + 1].code() * sign) != Piece::Pawn)
			epSq = 0;
	}
	
	// Reversible halfmove count
	++token;
	if (token != strList.end())
	{
		bool ok;
		int tmp = token->toInt(&ok);
		if (!ok || tmp < 0)
			return false;
		m_reversibleMoveCount = tmp;
	}
	else
		m_reversibleMoveCount = 0;
	
	// The full move number is ignored. It's rarely useful
	
	// Copy the FEN data to the Board object
	m_side = side;
	m_sign = (m_side == White) ? 1 : -1;
	m_enpassantSquare = epSq;
	m_castlingRights = fd.cr;
	m_kingSquare[White] = fd.kingSquare[White];
	m_kingSquare[Black] = fd.kingSquare[Black];
	m_squares = fd.squares;
	m_history.clear();
	m_startFen = fenString();
	
	// It sucks if this legality check fails, because at this point
	// we can't undo the changes, and the board is unusable until
	// it gets a new, valid FEN string.
	if (!isLegalPosition())
		return false;
	
	initZobristKey();
	emit boardReset();
	return true;
}

bool Board::reset()
{
	return setFenString(m_variant.startingFen());
}

QString Board::castlingRightsString(FenNotation notation) const
{
	QString str;
	
	for (int side = White; side <= Black; side++)
	{
		for (int cside = KingSide; cside >= QueenSide; cside--)
		{
			int rs = m_castlingRights.rookSquare[side][cside];
			if (rs == 0)
				continue;
			
			int offset = (cside == QueenSide) ? -1: 1;
			Piece piece;
			int i = rs + offset;
			bool ambiguous = false;
			
			// If the castling rook is not the outernmost rook,
			// the castling square is ambiguous
			while (!(piece = m_squares[i]).isWall())
			{
				if (piece == Piece(Side(side), Piece::Rook))
				{
					ambiguous = true;
					break;
				}
				i += offset;
			}
			
			QChar c;
			// If the castling square is ambiguous, then we can't
			// use 'K' or 'Q'. Instead we'll use the square's file.
			if (ambiguous || notation == ShredderFen)
				c = QChar('a' + chessSquare(rs).file());
			else
			{
				if (cside == 0)
					c = 'q';
				else
					c = 'k';
			}
			if (side == White)
				c = c.toUpper();
			str += c;
		}
	}
	
	if (str.length() == 0)
		str = "-";
	return str;
}

QString Board::fenString(FenNotation notation) const
{
	QString fen;
	
	// Squares
	int i = m_arwidth * 2;
	for (int y = 0; y < m_height; y++)
	{
		int nempty = 0;
		i++;
		if (y > 0)
			fen += '/';
		for (int x = 0; x < m_width; x++)
		{
			Piece pc = m_squares[i];
			
			if (pc.isEmpty())
				nempty++;
			
			// Add the number of empty successive squares
			// to the FEN string.
			if (nempty > 0
			&&  (!pc.isEmpty() || x == m_width - 1))
			{
				fen += QString::number(nempty);
				nempty = 0;
			}
			
			if (pc.isValid())
				fen += pc.toChar();
			i++;
		}
		i++;
	}
	
	// Side to move
	if (m_side == White)
		fen += " w ";
	else if (m_side == Black)
		fen += " b ";
	
	// Castling rights
	fen += castlingRightsString(notation) + ' ';
	
	// En-passant square
	if (m_enpassantSquare != 0)
		fen += chessSquare(m_enpassantSquare).toString();
	else
		fen += '-';
	
	// Reversible halfmove count
	fen += ' ';
	fen += QString::number(m_reversibleMoveCount);
	
	// Full move number
	fen += ' ';
	fen += QString::number(m_history.size() / 2 + 1);
	
	return fen;
}
