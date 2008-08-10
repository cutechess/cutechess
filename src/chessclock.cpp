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

#include <QtGui>
#include "chessclock.h"


ChessClock::ChessClock(QWidget* parent)
	:QLCDNumber(parent)
{
	setFrameShape(QFrame::NoFrame);
	setSegmentStyle(QLCDNumber::Flat);
	
	m_totalTime = 0;
	m_timerId = -1;
}

void ChessClock::start(int totalTime)
{
	m_timer.start();
	m_totalTime = totalTime;
	m_timerId = startTimer(1000);
	updateDisplay();
}

void ChessClock::stop()
{
	killTimer(m_timerId);
}

void ChessClock::updateDisplay()
{
	QTime timeLeft = QTime().addMSecs(m_totalTime - m_timer.elapsed());
	QString format;
	if (timeLeft.hour() > 0)
		format = "hh:mm:ss";
	else
		format = "mm:ss";
	
	QString str = timeLeft.toString(format);
	setNumDigits(str.length());
	display(str);
}

void ChessClock::timerEvent(QTimerEvent* event)
{
	if (!event)
		return;
	
	if (event->timerId() == m_timerId)
		updateDisplay();
}

