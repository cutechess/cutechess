/*
    This file is part of Cute Chess.
    Copyright (C) 2008-2018 Cute Chess authors

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

#include "pgngamefilter.h"
#include "pgngameentry.h"


PgnGameFilter::PgnGameFilter()
	: m_type(Advanced),
	  m_minRound(0),
	  m_maxRound(0),
	  m_result(AnyResult),
	  m_resultInverted(false)
{
}

PgnGameFilter::PgnGameFilter(const QString& pattern)
	: m_type(FixedString),
	  m_pattern(pattern.toLatin1()),
	  m_minRound(0),
	  m_maxRound(0),
	  m_result(AnyResult),
	  m_resultInverted(false)
{
}

void PgnGameFilter::setPattern(const QString& pattern)
{
	m_type = FixedString;
	m_pattern = pattern.toLatin1();
}

void PgnGameFilter::setEvent(const QString& event)
{
	m_event = event.toLatin1();
}

void PgnGameFilter::setSite(const QString& site)
{
	m_site = site.toLatin1();
}

void PgnGameFilter::setPlayer(const QString& name, Chess::Side side)
{
	m_player = name.toLatin1();
	m_playerSide = side;
}

void PgnGameFilter::setOpponent(const QString& name)
{
	m_opponent = name.toLatin1();
}

void PgnGameFilter::setMinDate(const QDate& date)
{
	m_minDate = date;
}

void PgnGameFilter::setMaxDate(const QDate& date)
{
	m_maxDate = date;
}

void PgnGameFilter::setMinRound(int round)
{
	m_minRound = round;
}

void PgnGameFilter::setMaxRound(int round)
{
	m_maxRound = round;
}

void PgnGameFilter::setResult(Result result)
{
	m_result = result;
}

void PgnGameFilter::setResultInverted(bool invert)
{
	m_resultInverted = invert;
}
