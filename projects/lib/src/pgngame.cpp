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
#include "pgnstream.h"


QTextStream& operator<<(QTextStream& out, const PgnGame* game)
{
	game->write(out);
	return out;
}


PgnGame::PgnGame(Chess::Variant variant)
	: m_hasTags(false),
	  m_round(0),
	  m_startingSide(Chess::White),
	  m_variant(variant)
{
}

bool PgnGame::addMove(const Chess::Move& move,
		      Chess::Board* board,
		      const QString& comment)
{
	Q_ASSERT(board != 0);
	if (!board->isLegalMove(move))
		return false;

	MoveData md;
	md.move = move;
	md.sanMove = board->moveString(move, Chess::StandardAlgebraic);
	md.comment = comment;

	m_moves.append(md);
	return true;
}

bool PgnGame::addMove(const QString& moveString,
		      Chess::Board* board,
		      const QString& comment)
{
	Q_ASSERT(board != 0);

	MoveData md;
	md.move = board->moveFromString(moveString);
	if (md.move.isNull())
		return false;

	Q_ASSERT(board->isLegalMove(md.move));
	md.sanMove = moveString;
	md.comment = comment;

	m_moves.append(md);
	return true;
}

PgnGame::PgnItem PgnGame::readItem(PgnStream& in, PgnMode mode)
{
	in.skipWhiteSpace();
	PgnItem itemType = PgnMove;

	QChar c;
	QChar openingBracket;
	QChar closingBracket;
	int bracketLevel = 0;
	QString str;
	Chess::Board* board = in.board();
	
	while (in.status() == PgnStream::Ok)
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
		Chess::Result result(str);
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
		
		if (tag == "Event")
			m_event = param;
		else if (tag == "Site")
			m_site = param;
		else if (tag == "Round")
			m_round = param.toInt();
		else if (tag == "White")
			m_playerName[Chess::White] = param;
		else if (tag == "Black")
			m_playerName[Chess::Black] = param;
		else if (tag == "Result")
		{
			m_result = Chess::Result(param);
			if (m_result.code() == Chess::Result::ResultError)
				qDebug() << "Invalid result:" << param;
		}
		else if (tag == "Variant")
		{
			m_variant = param;
			if (m_variant.isNone())
			{
				qDebug() << "Invalid variant:" << param;
				return PgnError;
			}
			board->setVariant(m_variant);
		}
		else if (tag == "FEN")
		{
			if (!board->setFenString(param))
			{
				qDebug() << "Invalid FEN:" << param;
				return PgnError;
			}
			setStartingFen(param);
		}
		else if (mode == Verbose)
		{
			if (tag == "TimeControl")
			{
				m_timeControl[0] = TimeControl(param);
				m_timeControl[1] = m_timeControl[0];
			}
			else if (tag == "WhiteTimeControl")
				m_timeControl[Chess::White] = TimeControl(param);
			else if (tag == "BlackTimeControl")
				m_timeControl[Chess::Black] = TimeControl(param);
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
			setStartingFen(board->variant().startingFen());
			board->setFenString(m_fen);
		}
		
		if (addMove(str, board))
			board->makeMove(m_moves.last().move);
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

bool PgnGame::read(PgnStream& in, PgnMode mode, int maxMoves)
{
	m_hasTags = false;
	m_round = 0;
	setStartingFen(QString());
	m_moves.clear();

	Chess::Board* board = in.board();
	if (!in.variant().isNone())
		m_variant = in.variant();
	else
		board->setVariant(m_variant);
	
	while (in.status() == PgnStream::Ok
	   &&  m_moves.size() < maxMoves)
	{
		PgnItem item = readItem(in, mode);
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

	return !m_fen.isEmpty();
}

static void writeTag(QTextStream& out, const QString& name, const QString& value)
{
	if (!value.isEmpty())
		out << "[" << name << " \"" << value << "\"]\n";
	else
		out << "[" << name << " \"?\"]\n";
}

bool PgnGame::write(QTextStream& out, PgnMode mode) const
{
	if (!m_hasTags)
		return false;
	
	QString date = QDate::currentDate().toString("yyyy.MM.dd");
	
	// The seven tag roster
	writeTag(out, "Event", m_event);
	writeTag(out, "Site", m_site);
	writeTag(out, "Date", date);
	if (m_round > 0)
		writeTag(out, "Round", QString::number(m_round));
	else
		writeTag(out, "Round", "?");
	writeTag(out, "White", m_playerName[Chess::White]);
	writeTag(out, "Black", m_playerName[Chess::Black]);
	writeTag(out, "Result", m_result.toSimpleString());

	if (mode == Verbose)
	{
		writeTag(out, "PlyCount", QString::number(m_moves.size()));

		const TimeControl& wtc = m_timeControl[Chess::White];
		const TimeControl& btc = m_timeControl[Chess::Black];
		if (wtc == btc && wtc.isValid())
			writeTag(out, "TimeControl", wtc.toString());
		else
		{
			if (wtc.isValid())
				writeTag(out, "WhiteTimeControl", wtc.toString());
			if (btc.isValid())
				writeTag(out, "BlackTimeControl", btc.toString());
		}
	}

	if (m_variant != Chess::Variant::Standard)
		writeTag(out, "Variant", m_variant.toString());
	if (!m_fen.isEmpty()
	&&  (m_variant.isRandom() || m_fen != m_variant.startingFen()))
	{
		writeTag(out, "SetUp", "1");
		writeTag(out, "FEN", m_fen);
	}

	QString str;
	int lineLength = 0;
	int movenum = 0;
	int side = m_startingSide;

	for (int i = 0; i < m_moves.size(); i++)
	{
		const MoveData& md = m_moves.at(i);

		str.clear();
		if (side == Chess::White || i == 0)
			str = QString::number(++movenum) + ". ";

		str += md.sanMove;
		if (mode == Verbose && !md.comment.isEmpty())
			str += QString(" {%1}").arg(md.comment);

		// Limit the lines to 80 characters
		if (lineLength == 0 || lineLength + str.size() >= 80)
		{
			out << "\n" << str;
			lineLength = str.size();
		}
		else
		{
			out << " " << str;
			lineLength += str.size() + 1;
		}

		side = !side;
	}
	str = QString("{%1} %2")
	      .arg(m_result.description())
	      .arg(m_result.toSimpleString());
	if (lineLength + str.size() >= 80)
		out << "\n" << str << "\n\n";
	else
		out << " " << str << "\n\n";

	return true;
}

bool PgnGame::write(const QString& filename, PgnMode mode) const
{
	QFile file(filename);
	if (!file.open(QIODevice::Append))
		return false;

	QTextStream out(&file);
	return write(out, mode);
}

bool PgnGame::isEmpty() const
{
	return m_moves.isEmpty();
}

QString PgnGame::event() const
{
	return m_event;
}

QString PgnGame::site() const
{
	return m_site;
}

int PgnGame::round() const
{
	return m_round;
}

QString PgnGame::startingFen() const
{
	return m_fen;
}

Chess::Result PgnGame::result() const
{
	return m_result;
}

const TimeControl& PgnGame::timeControl(Chess::Side side) const
{
	Q_ASSERT(side != Chess::NoSide);
	return m_timeControl[side];
}

const QVector<PgnGame::MoveData>& PgnGame::moves() const
{
	return m_moves;
}

QString PgnGame::playerName(Chess::Side side) const
{
	return m_playerName[side];
}

void PgnGame::setPlayerName(Chess::Side side, const QString& name)
{
	m_playerName[side] = name;
	m_hasTags = true;
}

void PgnGame::setEvent(const QString& event)
{
	m_event = event;
}

void PgnGame::setSite(const QString& site)
{
	m_site = site;
}

void PgnGame::setRound(int round)
{
	m_round = round;
}

void PgnGame::setResult(const Chess::Result& result)
{
	m_result = result;
}

void PgnGame::setStartingFen(const QString& fen)
{
	if (!fen.isEmpty())
	{
		QString sideStr = fen.section(' ', 1, 1);
		Q_ASSERT(sideStr == "w" || sideStr == "b");

		if (sideStr == "b")
			m_startingSide = Chess::Black;
		else
			m_startingSide = Chess::White;
	}
	else
		m_startingSide = Chess::White;

	m_fen = fen;
}

void PgnGame::setTimeControl(const TimeControl& timeControl, Chess::Side side)
{
	if (side != Chess::NoSide)
		m_timeControl[side] = timeControl;
	else
	{
		m_timeControl[Chess::White] = timeControl;
		m_timeControl[Chess::Black] = timeControl;
	}
}
