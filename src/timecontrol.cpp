/*
    This file is part of SloppyGUI.

    SloppyGUI is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SloppyGUI is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SloppyGUI.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QDebug>

#include "timecontrol.h"

TimeControl::TimeControl(int timePerTc, int movesPerTc, int increment, int timePerMove)
{
	setTimePerTc(timePerTc);
	setMovesPerTc(movesPerTc);
	setIncrement(increment);
	setTimePerMove(timePerMove);
}

int TimeControl::timePerTc() const
{
	return m_timePerTc;
}

int TimeControl::movesPerTc() const
{
	return m_movesPerTc;
}

int TimeControl::increment() const
{
	return m_increment;
}

int TimeControl::timePerMove() const
{
	return m_timePerMove;
}


void TimeControl::setTimePerTc(int timePerTc)
{
	if (timePerTc < 0) {
		qDebug("Negative time per time control: %d", timePerTc);
		return;
	}
	
	m_timePerTc = timePerTc;
	if (m_timePerTc > 0)
		m_timePerMove = 0;
}

void TimeControl::setMovesPerTc(int movesPerTc)
{
	if (movesPerTc < 0) {
		qDebug("Negative moves per time control: %d", movesPerTc);
		return;
	}
	
	m_movesPerTc = movesPerTc;
	if (m_movesPerTc > 0)
		m_timePerMove = 0;
}

void TimeControl::setIncrement(int increment)
{
	if (increment < 0) {
		qDebug("Negative time control increment: %d", increment);
		return;
	}
	
	m_increment = increment;
	if (m_increment > 0)
		m_timePerMove = 0;
}

void TimeControl::setTimePerMove(int timePerMove)
{
	if (timePerMove < 0) {
		qDebug("Negative time per move: %d", timePerMove);
		return;
	}
	
	m_timePerMove = timePerMove;
	if (m_timePerMove > 0) {
		m_timePerTc = 0;
		m_movesPerTc = 0;
		m_increment = 0;
	}
}

