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


namespace Chess {

Result::Result(Type type, Side winner, const QString& description)
	: m_type(type),
	  m_winner(winner),
	  m_description(description)
{
}

Result::Result(const QString& str)
	: m_type(ResultError),
	  m_winner(NoSide)
{
	if (str.startsWith("1-0"))
	{
		m_type = Win;
		m_winner = White;
	}
	else if (str.startsWith("0-1"))
	{
		m_type = Win;
		m_winner = Black;
	}
	else if (str.startsWith("1/2-1/2"))
		m_type = Draw;
	else if (str.startsWith("*"))
		m_type = NoResult;

	int start = str.indexOf('{');
	int end = str.lastIndexOf('}');
	if (start != -1 && end != -1)
		m_description = str.mid(start + 1, end - start - 1);
}

bool Result::operator==(const Result& other) const
{
	return (m_type == other.m_type &&
		m_winner == other.m_winner &&
		m_description == other.m_description);
}

bool Result::operator!=(const Result& other) const
{
	return (m_type != other.m_type ||
		m_winner != other.m_winner ||
		m_description != other.m_description);
}

bool Result::isNone() const
{
	return m_type == NoResult;
}

bool Result::isDraw() const
{
	return (m_winner == NoSide &&
		m_type != NoResult &&
		m_type != ResultError);
}

Side Result::winner() const
{
	return m_winner;
}

Side Result::loser() const
{
	if (m_winner == NoSide)
		return NoSide;
	return otherSide(m_winner);
}

Result::Type Result::type() const
{
	return m_type;
}

QString Result::description() const
{
	QString w(sideString(winner()));
	QString l(sideString(loser()));
	QString str;

	if (m_type == Resignation)
		str = QObject::tr("%1 resigns").arg(l);
	else if (m_type == Timeout)
	{
		if (l.isEmpty())
			str = QObject::tr("Draw by timeout");
		else
			str = QObject::tr("%1 loses on time").arg(l);
	}
	else if (m_type == Adjudication)
	{
		if (w.isEmpty())
			str = QObject::tr("Draw by adjudication");
		else
			str = QObject::tr("%1 wins by adjudication").arg(w);
	}
	else if (m_type == IllegalMove)
		str = QObject::tr("%1 makes an illegal move").arg(l);
	else if (m_type == Disconnection)
	{
		if (l.isEmpty())
			str = QObject::tr("Draw by disconnection");
		else
			str = QObject::tr("%1 disconnects").arg(l);
	}
	else if (m_type == StalledConnection)
	{
		if (l.isEmpty())
			str = QObject::tr("Draw by stalled connection");
		else
			str = QObject::tr("%1's connection stalls").arg(l);
	}
	else if (m_type == Agreement)
	{
		if (w.isEmpty())
			str = QObject::tr("Draw by agreement");
		else
			str = QObject::tr("%1 wins by agreement").arg(w);
	}
	else if (m_type == NoResult)
		str = QObject::tr("No result");
	else if (m_type == ResultError)
		str = QObject::tr("Result error");

	if (m_description.isEmpty())
	{
		if (m_type == Win)
			return QObject::tr("%1 wins").arg(w);
		else if (m_type == Draw)
			return QObject::tr("Drawn game");
	}
	else
	{
		if (!str.isEmpty())
			str += ": ";
		str += m_description;
	}

	Q_ASSERT(!str.isEmpty());
	return str;
}

QString Result::toShortString() const
{
	if (m_type == NoResult || m_type == ResultError)
		return "*";
	if (m_winner == White)
		return "1-0";
	if (m_winner == Black)
		return "0-1";
	return "1/2-1/2";
}

QString Result::toVerboseString() const
{
	return toShortString() + QString(" {") + description() + "}";
}

} // namespace Chess
