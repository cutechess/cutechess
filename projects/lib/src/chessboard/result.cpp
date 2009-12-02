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

#include "result.h"
#include <QObject>
using namespace Chess;

Result::Result(Code code, Side winner, const QString& move)
	: m_code(code),
	  m_winner(winner),
	  m_move(move)
{
}

Result::Result(const QString& str)
	: m_code(ResultError),
	  m_winner(NoSide)
{
	if (str.startsWith("1-0"))
	{
		m_code = Win;
		m_winner = White;
	}
	else if (str.startsWith("0-1"))
	{
		m_code = Win;
		m_winner = Black;
	}
	else if (str.startsWith("1/2-1/2"))
		m_code = Draw;
	else if (str.startsWith("*"))
		m_code = NoResult;
}

bool Result::operator==(const Result& other) const
{
	return (m_code == other.m_code && m_winner == other.m_winner);
}

bool Result::operator!=(const Result& other) const
{
	return (m_code != other.m_code || m_winner != other.m_winner);
}

Side Result::winner() const
{
	return m_winner;
}

Side Result::loser() const
{
	switch (m_winner)
	{
	case White:
		return Black;
	case Black:
		return White;
	default:
		return NoSide;
	}
}

bool Result::isDraw() const
{
	return (m_winner == NoSide && m_code != NoResult && m_code != ResultError);
}

bool Result::isNone() const
{
	return (m_code == NoResult || m_code == ResultError);
}

Result::Code Result::code() const
{
	return m_code;
}

Result::Code Result::simpleCode() const
{
	if (m_code == NoResult || m_code == ResultError)
		return m_code;
	if (m_winner != NoSide)
		return Win;
	return Draw;
}

QString Result::description() const
{
	QString str;
	QString w = QObject::tr("Player");
	QString l = w;

	if (m_winner == White)
	{
		w = QObject::tr("White");
		l = QObject::tr("Black");
	}
	else if (m_winner == Black)
	{
		w = QObject::tr("Black");
		l = QObject::tr("White");
	}

	switch (m_code)
	{
	case Win:
		str = QObject::tr("%1 wins").arg(w);
		break;
	case WinByMate:
		str = QObject::tr("%1 mates").arg(w);
		break;
	case WinByResignation:
		str = QObject::tr("%1 resigns").arg(l);
		break;
	case WinByTimeout:
		str = QObject::tr("%1 loses on time").arg(l);
		break;
	case WinByAdjudication:
		str = QObject::tr("%1 wins by adjudication").arg(w);
		break;
	case WinByIllegalMove:
		str = QObject::tr("%1 made an illegal move").arg(l);
		if (!m_move.isEmpty())
			str += QString(": ") + m_move;
		break;
	case WinByDisconnection:
		str = QObject::tr("%1 disconnects").arg(l);
		break;
	case WinByStalledConnection:
		str = QObject::tr("%1's connection stalled").arg(l);
		break;
	case Draw:
		str = QObject::tr("Draw");
		break;
	case DrawByStalemate:
		str = QObject::tr("Draw by stalemate");
		break;
	case DrawByMaterial:
		str = QObject::tr("Draw by insufficient mating material");
		break;
	case DrawByRepetition:
		str = QObject::tr("Draw by 3-fold repetition");
		break;
	case DrawByFiftyMoves:
		str = QObject::tr("Draw by fifty moves rule");
		break;
	case DrawByAgreement:
		str = QObject::tr("Draw by agreement");
		break;
	case DrawByTimeout:
		str = QObject::tr("Draw by timeout");
		break;
	case DrawByAdjudication:
		str = QObject::tr("Draw by adjudication");
		break;
	case NoResult:
		str = QObject::tr("Unfinished");
		break;
	default:
		str = QObject::tr("Result error");
		break;
	}

	return str;
}

QString Result::toString() const
{
	return toSimpleString() + QString(" {") + description() + "}";
}

QString Result::toSimpleString() const
{
	if (m_code == NoResult || m_code == ResultError)
		return "*";
	if (m_winner == White)
		return "1-0";
	if (m_winner == Black)
		return "0-1";
	return "1/2-1/2";
}
