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

Result::Result(Code code)
	: m_code(code)
{
}

Result::Result(const QString& str)
{
	if (str.startsWith("1-0"))
		m_code = WhiteWins;
	else if (str.startsWith("0-1"))
		m_code = BlackWins;
	else if (str.startsWith("1/2-1/2"))
		m_code = Draw;
	else if (str.startsWith("*"))
		m_code = NoResult;
	else
		m_code = ResultError;
}

bool Result::operator==(const Result& other) const
{
	return (m_code == other.m_code);
}

bool Result::operator==(Code otherCode) const
{
	return (m_code == otherCode);
}

bool Result::operator!=(const Result& other) const
{
	return (m_code != other.m_code);
}

bool Result::operator!=(Code otherCode) const
{
	return (m_code != otherCode);
}

Side Result::winner() const
{
	switch (m_code)
	{
	case WhiteWins:
	case WhiteMates:
	case BlackResigns:
	case BlackTimeout:
	case BlackTerminates:
		return White;
	case BlackWins:
	case BlackMates:
	case WhiteResigns:
	case WhiteTimeout:
	case WhiteTerminates:
		return Black;
	default:
		return NoSide;
	}
}

Side Result::loser() const
{
	switch (winner())
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
	switch (m_code)
	{
	case Draw: case DrawByMaterial:
	case DrawByRepetition: case DrawByFiftyMoves:
	case DrawByAgreement: case DrawByTimeout:
		return true;
	default:
		return false;
	}
}

bool Result::isNone() const
{
	return (m_code == NoResult);
}

Result::Code Result::code() const
{
	return m_code;
}

Result::Code Result::simpleCode() const
{
	switch (winner())
	{
	case White:
		return WhiteWins;
	case Black:
		return BlackWins;
	default:
		if (isDraw())
			return Draw;
		return NoResult;
	}
}

QString Result::toString() const
{
	switch (m_code)
	{
	case WhiteWins:
		return QObject::tr("1-0 {White wins}");
	case BlackWins:
		return QObject::tr("0-1 {Black wins}");
	case WhiteMates:
		return QObject::tr("1-0 {White mates}");
	case BlackMates:
		return QObject::tr("0-1 {Black mates}");
	case WhiteResigns:
		return QObject::tr("0-1 {White resigns}");
	case BlackResigns:
		return QObject::tr("1-0 {Black resigns}");
	case WhiteTimeout:
		return QObject::tr("0-1 {White loses on time}");
	case BlackTimeout:
		return QObject::tr("1-0 {Black loses on time}");
	case WhiteTerminates:
		return QObject::tr("0-1 {White quits/terminates}");
	case BlackTerminates:
		return QObject::tr("1-0 {Black quits/terminates}");
	case Stalemate:
		return QObject::tr("1/2-1/2 {Stalemate}");
	case Draw:
		return QObject::tr("1/2-1/2 {Draw}");
	case DrawByMaterial:
		return QObject::tr("1/2-1/2 {Draw by insufficient material}");
	case DrawByRepetition:
		return QObject::tr("1/2-1/2 {Draw by 3-fold repetition}");
	case DrawByFiftyMoves:
		return QObject::tr("1/2-1/2 {Draw by fifty moves rule}");
	case DrawByAgreement:
		return QObject::tr("1/2-1/2 {Draw by agreement}");
	case DrawByTimeout:
		return QObject::tr("1/2-1/2 {Draw by timeout}");
	case NoResult:
		return QObject::tr("* {Unfinished}");
	default:
		return QObject::tr("* {Result error}");
	}
}

QString Result::toSimpleString() const
{
	switch (simpleCode())
	{
	case WhiteWins:
		return "1-0";
	case BlackWins:
		return "0-1";
	case Draw:
		return "1/2-1/2";
	default:
		return "*";
	}
}
