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

#include <QFile>
#include <QTextStream>
#include <QDate>
#include <QtDebug>
#include "pgngame.h"
#include "chessgame.h"
#include "chessboard/chessboard.h"
#include "chessplayer.h"
#include "pgnfile.h"


PgnGame::PgnGame(const ChessGame* game)
	: m_hasTags(false)
{
	Q_ASSERT(game != 0);
	
	m_whitePlayer = game->whitePlayer()->name();
	m_blackPlayer = game->blackPlayer()->name();
	Chess::Board* board = game->chessboard();
	m_moves = board->moveHistory();
	m_fen = board->startingFen();
	m_variant = board->variant();
	m_isRandomVariant = board->isRandomVariant();
	m_result = game->result();
	
	m_hasTags = true;
}

static QString resultToString(Chess::Result result)
{
	switch (result)
	{
	case Chess::WhiteMates: case Chess::BlackResigns:
		return "1-0";
	case Chess::BlackMates: case Chess::WhiteResigns:
		return "0-1";
	case Chess::Stalemate: case Chess::DrawByMaterial:
	case Chess::DrawByRepetition: case Chess::DrawByFiftyMoves:
	case Chess::DrawByAgreement:
		return "1/2-1/2";
	default:
		return "*";
	};
}

static Chess::Result resultFromString(const QString& str)
{
	if (str == "*")
		return Chess::NoResult;
	else if (str == "1-0")
		return Chess::WhiteMates;
	else if (str == "0-1")
		return Chess::BlackMates;
	else if (str == "1/2-1/2")
		return Chess::DrawByAgreement;

	return Chess::ResultError;
}

PgnGame::PgnItem PgnGame::readItem(PgnFile& in)
{
	in.skipWhiteSpace();
	PgnItem itemType = PgnMove;

	QChar c;
	QChar openingBracket;
	QChar closingBracket;
	int bracketLevel = 0;
	QString str;
	Chess::Board* board = in.board();
	
	while (in.status() == QTextStream::Ok)
	{
		c = in.readChar();
		if (!m_hasTags && itemType != PgnTag && c != '[')
			continue;
		if ((c == '\n' || c == '\r') && itemType != PgnComment)
			break;
		
		if (openingBracket.isNull())
		{
			if (str.isEmpty())
			{
				// "Rest of the line" comment
				if (c == ';')
				{
					itemType = PgnComment;
					str = in.readLine();
					break;
				}
				// Escape mechanism (skip this line)
				if (c == '%')
				{
					in.readLine();
					continue;
				}
				// Skip leading periods
				if (c == '.')
				{
					in.skipWhiteSpace();
					continue;
				}
				// NAG (Numeric Annotation Glyph)
				if (c == '$')
				{
					itemType = PgnNag;
					continue;
				}
				// Move number
				if (c.isDigit() && itemType == PgnMove)
					itemType = PgnMoveNumber;
			}
			// Tag
			if (c == '[')
			{
				// Disallow tags after moves have been read
				if (m_moves.size() > 0)
				{
					// We may be reading the next game in
					// the stream, so rewind by one character.
					in.rewindChar();
					qDebug() << "No termination marker";
					return PgnError;
				}
				
				itemType = PgnTag;
				closingBracket = ']';
			}
			else if (c == '(')
			{
				itemType = PgnComment;
				closingBracket = ')';
			}
			else if (c == '{')
			{
				itemType = PgnComment;
				closingBracket = '}';
			}
			
			if (!closingBracket.isNull())
				openingBracket = c;
		}
		if (c == openingBracket)
			bracketLevel++;
		else if (c == closingBracket)
		{
			bracketLevel--;
			if (bracketLevel <= 0)
				break;
		}
		else if (itemType == PgnMove && c.isSpace())
			break;
		else if (itemType == PgnMoveNumber
		     &&  (c.isSpace() || c == '.'))
			break;
		else if (itemType == PgnNag && c.isSpace())
			break;
		else
			str += c;
	}
	
	str = str.trimmed();
	if (str.isEmpty())
		return PgnEmpty;
	
	if ((itemType == PgnMove || itemType == PgnMoveNumber)
	&&  (str == "*" || str == "1/2-1/2" || str == "1-0" || str == "0-1"))
	{
		Chess::Result result = resultFromString(str);
		if (result != m_result)
			qDebug() << "Line" << in.lineNumber() << "The termination "
			            "marker is different from the result tag";
		m_result = result;
		return PgnResult;
	}
	
	if (itemType == PgnTag)
	{
		QString tag = str.section(' ', 0, 0);
		QString param = str.section(' ', 1).replace('\"', "");
		
		if (tag == "White")
			m_whitePlayer = param;
		else if (tag == "Black")
			m_blackPlayer = param;
		else if (tag == "Result")
		{
			m_result = resultFromString(param);
			if (m_result == Chess::ResultError)
				qDebug() << "Invalid result:" << param;
		}
		else if (tag == "FEN")
		{
			m_fen = param;
			if (!board->setBoard(m_fen))
			{
				qDebug() << "Invalid FEN:" << m_fen;
				return PgnError;
			}
		}
	}
	else if (itemType == PgnMove)
	{
		if (!m_hasTags)
		{
			qDebug() << "No tags found";
			return PgnError;
		}
		
		// If the FEN string wasn't already set by the FEN tag,
		// set the board when we get the first move
		if (m_fen.isEmpty())
		{
			board->setBoard();
			m_fen = board->startingFen();
		}
		
		Chess::Move move = board->moveFromString(str);
		if (board->isLegalMove(move))
		{
			m_moves.append(move);
			board->makeMove(move);
		}
		else
		{
			qDebug() << "Illegal move:" << str;
			return PgnError;
		}
	}
	else if (itemType == PgnNag)
	{
		bool ok;
		int nag = str.toInt(&ok);
		if (!ok || nag < 0 || nag > 255)
		{
			qDebug() << "Invalid NAG:" << str;
			return PgnError;
		}
	}
	
	return itemType;
}

PgnGame::PgnGame(PgnFile& in, int maxMoves)
	: m_variant(Chess::StandardChess),
	  m_isRandomVariant(false),
	  m_hasTags(false),
	  m_result(Chess::NoResult),
	  m_round(0)
{
	Chess::Board* board = in.board();
	if (in.variant() != Chess::NoVariant)
		m_variant = in.variant();
	else
		board->setVariant(m_variant);
	
	while (in.status() == QTextStream::Ok
	   &&  m_moves.size() < maxMoves)
	{
		PgnItem item = readItem(in);
		if (item == PgnError)
		{
			qDebug() << "PGN error on line" << in.lineNumber();
			break;
		}
		else if (item == PgnTag)
			m_hasTags = true;
		else if (item == PgnResult)
			break;
		else if (item == PgnEmpty)
			break;
	}
}

void PgnGame::write(const QString& filename) const
{
	if (!m_hasTags)
		return;
	
	bool useFen = false;
	QString variantString;
	if (m_variant == Chess::StandardChess)
	{
		if (m_fen != Chess::standardFen)
			useFen = true;
		if (m_isRandomVariant)
			variantString = "Fischerandom";
	}
	else if (m_variant == Chess::CapablancaChess)
	{
		if (m_fen == Chess::capablancaFen)
			variantString = "Capablanca";
		else if (m_fen == Chess::gothicFen)
			variantString = "Gothic";
		else
			useFen = true;
		if (m_isRandomVariant)
			variantString = "Caparandom";
	}
	
	QString resultString = resultToString(m_result);
	QString date = QDate::currentDate().toString("yyyy.MM.dd");
	
	QFile file(filename);
	if (file.open(QIODevice::Append))
	{
		QTextStream out(&file);
		
		out << "[Date \"" << date << "\"]\n";
		out << "[White \"" << m_whitePlayer << "\"]\n";
		out << "[Black \"" << m_blackPlayer << "\"]\n";
		out << "[Result \"" << resultString << "\"]\n";
		if (!variantString.isEmpty())
			out << "[Variant \"" << variantString << "\"]\n";
		if (useFen)
			out << "[FEN \"" << m_fen << "\"]\n";
		
		Chess::Board board(m_variant);
		board.setBoard(m_fen);
		for (int i = 0; i < m_moves.size(); i++)
		{
			if ((i % 8) == 0)
				out << "\n";
			if ((i % 2) == 0)
				out << QString::number(i / 2 + 1) << ". ";
			out << board.moveString(m_moves[i], Chess::StandardAlgebraic) << " ";
			board.makeMove(m_moves[i]);
		}
		out << resultString << "\n\n";
	}
}

bool PgnGame::isEmpty() const
{
	return m_moves.isEmpty();
}

