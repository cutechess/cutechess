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

#include "timecontrol.h"
#include <QStringList>
#include <QSettings>

namespace {

QString s_timeString(int ms)
{
	if (ms == 0 || ms % 60000 != 0)
		return TimeControl::tr("%1 sec").arg(double(ms) / 1000.0);
	if (ms % 3600000 != 0)
		return TimeControl::tr("%1 min").arg(ms / 60000);
	return TimeControl::tr("%1 h").arg(ms / 3600000);
}

QString s_nodeString(int nodes)
{
	if (nodes == 0 || nodes % 1000 != 0)
		return QString::number(nodes);
	else if (nodes % 1000000 != 0)
		return TimeControl::tr("%1 k").arg(nodes / 1000);
	return TimeControl::tr("%1 M").arg(nodes / 1000000);
}

} // anonymous namespace

TimeControl::TimeControl()
	: m_movesPerTc(0),
	  m_timePerTc(0),
	  m_timePerMove(0),
	  m_increment(0),
	  m_timeLeft(0),
	  m_movesLeft(0),
	  m_plyLimit(0),
	  m_nodeLimit(0),
	  m_lastMoveTime(0),
	  m_expiryMargin(0),
	  m_expired(false),
	  m_infinite(false)
{
}

TimeControl::TimeControl(const QString& str)
	: m_movesPerTc(0),
	  m_timePerTc(0),
	  m_timePerMove(0),
	  m_increment(0),
	  m_timeLeft(0),
	  m_movesLeft(0),
	  m_plyLimit(0),
	  m_nodeLimit(0),
	  m_lastMoveTime(0),
	  m_expiryMargin(0),
	  m_expired(false),
	  m_infinite(false)
{
	if (str == "inf")
	{
		setInfinity(true);
		return;
	}

	QStringList list = str.split('+');

	// increment
	if (list.size() == 2)
	{
		int inc = (int)(list.at(1).toDouble() * 1000);
		if (inc >= 0)
			setTimeIncrement(inc);
	}

	list = list.at(0).split('/');
	QString strTime;

	// moves per tc
	if (list.size() == 2)
	{
		int nmoves = list.at(0).toInt();
		if (nmoves >= 0)
			setMovesPerTc(nmoves);
		strTime = list.at(1);
	}
	else
		strTime = list.at(0);

	// time per tc
	int ms = 0;
	list = strTime.split(':');
	if (list.size() == 2)
		ms = (int)(list.at(0).toDouble() * 60000 + list.at(1).toDouble() * 1000);
	else
		ms = (int)(list.at(0).toDouble() * 1000);

	if (ms > 0)
		setTimePerTc(ms);
}

bool TimeControl::operator==(const TimeControl& other) const
{
	if (m_movesPerTc == other.m_movesPerTc
	&&  m_timePerTc == other.m_timePerTc
	&&  m_timePerMove == other.m_timePerMove
	&&  m_increment == other.m_increment
	&&  m_plyLimit == other.m_plyLimit
	&&  m_nodeLimit == other.m_nodeLimit
	&&  m_infinite == other.m_infinite)
		return true;
	return false;
}

bool TimeControl::isValid() const
{
	if (m_movesPerTc < 0
	||  m_timePerTc < 0
	||  m_timePerMove < 0
	||  m_increment < 0
	||  m_plyLimit < 0
	||  m_nodeLimit < 0
	||  m_expiryMargin < 0
	||  (m_timePerTc == m_timePerMove && !m_infinite))
		return false;
	return true;
}

QString TimeControl::toString() const
{
	if (!isValid())
		return QString();

	if (m_infinite)
		return QString("inf");

	if (m_timePerMove != 0)
		return QString("%1/move").arg((double)m_timePerMove / 1000);

	QString str;
	if (m_movesPerTc > 0)
		str += QString::number(m_movesPerTc) + "/";
	str += QString::number((double)m_timePerTc / 1000);

	if (m_increment > 0)
		str += QString("+") + QString::number((double)m_increment / 1000);
	return str;
}

QString TimeControl::toVerboseString() const
{
	if (!isValid())
		return QString();

	QString str;

	if (m_infinite)
		str = tr("infinite time");
	else if (m_timePerMove != 0)
		str = tr("%1 per move")
			.arg(s_timeString(m_timePerMove));
	else if (m_movesPerTc != 0)
		str = tr("%1 moves in %2")
			.arg(m_movesPerTc)
			.arg(s_timeString(m_timePerTc));
	else
		str = s_timeString(m_timePerTc);

	if (m_timePerTc != 0 && m_increment != 0)
		str += tr(", %1 increment")
			.arg(s_timeString(m_increment));
	if (m_nodeLimit != 0)
		str += tr(", %1 nodes")
			.arg(s_nodeString(m_nodeLimit));
	if (m_plyLimit != 0)
		str += tr(", %1 plies").arg(m_plyLimit);
	if (m_expiryMargin != 0)
		str += tr(", %1 msec margin").arg(m_expiryMargin);

	return str;
}

void TimeControl::initialize()
{
	m_expired = false;
	m_lastMoveTime = 0;

	if (m_timePerTc != 0)
	{
		m_timeLeft = m_timePerTc;
		m_movesLeft = m_movesPerTc;
	}
	else if (m_timePerMove != 0)
		m_timeLeft = m_timePerMove;
}

bool TimeControl::isInfinite() const
{
	return m_infinite;
}

int TimeControl::timePerTc() const
{
	return m_timePerTc;
}

int TimeControl::movesPerTc() const
{
	return m_movesPerTc;
}

int TimeControl::timeIncrement() const
{
	return m_increment;
}

int TimeControl::timePerMove() const
{
	return m_timePerMove;
}

int TimeControl::timeLeft() const
{
	return m_timeLeft;
}

int TimeControl::movesLeft() const
{
	return m_movesLeft;
}

int TimeControl::plyLimit() const
{
	return m_plyLimit;
}

int TimeControl::nodeLimit() const
{
	return m_nodeLimit;
}

int TimeControl::expiryMargin() const
{
	return m_expiryMargin;
}

void TimeControl::setInfinity(bool enabled)
{
	m_infinite = enabled;
}

void TimeControl::setTimePerTc(int timePerTc)
{
	Q_ASSERT(timePerTc >= 0);
	m_timePerTc = timePerTc;
}

void TimeControl::setMovesPerTc(int movesPerTc)
{
	Q_ASSERT(movesPerTc >= 0);
	m_movesPerTc = movesPerTc;
}

void TimeControl::setTimeIncrement(int increment)
{
	Q_ASSERT(increment >= 0);
	m_increment = increment;
}

void TimeControl::setTimePerMove(int timePerMove)
{
	Q_ASSERT(timePerMove >= 0);
	m_timePerMove = timePerMove;
}

void TimeControl::setTimeLeft(int timeLeft)
{
	m_timeLeft = timeLeft;
}

void TimeControl::setMovesLeft(int movesLeft)
{
	Q_ASSERT(movesLeft >= 0);
	m_movesLeft = movesLeft;
}

void TimeControl::setPlyLimit(int plies)
{
	Q_ASSERT(plies >= 0);
	m_plyLimit = plies;
}

void TimeControl::setNodeLimit(int nodes)
{
	Q_ASSERT(nodes >= 0);
	m_nodeLimit = nodes;
}

void TimeControl::setExpiryMargin(int expiryMargin)
{
	Q_ASSERT(expiryMargin >= 0);
	m_expiryMargin = expiryMargin;
}

void TimeControl::startTimer()
{
	m_time.start();
}

void TimeControl::update(bool applyIncrement)
{
	/*
	 * This will overflow after roughly 49 days however it's unlikely
	 * we'll ever hit that limit.
	 */
	if (m_time.isValid())
		m_lastMoveTime = (int)m_time.elapsed();
	else
		m_lastMoveTime = 0;

	if (!m_infinite && m_lastMoveTime > m_timeLeft + m_expiryMargin)
		m_expired = true;

	if (m_timePerMove != 0)
		setTimeLeft(m_timePerMove);
	else
	{
	        int newTimeLeft = m_timeLeft - m_lastMoveTime;
		if (applyIncrement)
			newTimeLeft += m_increment;
		setTimeLeft(newTimeLeft);
		
		if (m_movesPerTc > 0)
		{
			setMovesLeft(m_movesLeft - 1);
			
			// Restart the time control
			if (m_movesLeft == 0)
			{
				setMovesLeft(m_movesPerTc);
				setTimeLeft(m_timePerTc + m_timeLeft);
			}
		}
	}
}

int TimeControl::lastMoveTime() const
{
	return m_lastMoveTime;
}

bool TimeControl::expired() const
{
	return m_expired;
}

int TimeControl::activeTimeLeft() const
{
	if (m_time.isValid())
		return m_timeLeft - m_time.elapsed();
	return m_timeLeft;
}

void TimeControl::readSettings(QSettings* settings)
{
	settings->beginGroup("time_control");

	m_movesPerTc = settings->value("moves_per_tc", m_movesPerTc).toInt();
	m_timePerTc = settings->value("time_per_tc", m_timePerTc).toInt();
	m_timePerMove = settings->value("time_per_move", m_timePerMove).toInt();
	m_increment = settings->value("increment", m_increment).toInt();
	m_plyLimit = settings->value("ply_limit", m_plyLimit).toInt();
	m_nodeLimit = settings->value("node_limit", m_nodeLimit).toInt();
	m_expiryMargin = settings->value("expiry_margin", m_expiryMargin).toInt();
	m_infinite = settings->value("infinite", m_infinite).toBool();

	settings->endGroup();
}

void TimeControl::writeSettings(QSettings* settings)
{
	settings->beginGroup("time_control");

	settings->setValue("moves_per_tc", m_movesPerTc);
	settings->setValue("time_per_tc", m_timePerTc);
	settings->setValue("time_per_move", m_timePerMove);
	settings->setValue("increment", m_increment);
	settings->setValue("ply_limit", m_plyLimit);
	settings->setValue("node_limit", m_nodeLimit);
	settings->setValue("expiry_margin", m_expiryMargin);
	settings->setValue("infinite", m_infinite);
}
