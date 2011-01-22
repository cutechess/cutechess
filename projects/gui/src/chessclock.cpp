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

#include "chessclock.h"
#include <QTimerEvent>


ChessClock::ChessClock(QWidget* parent)
	: QLCDNumber(parent),
	  m_totalTime(0),
	  m_timerId(-1)
{
	setFrameShape(QFrame::NoFrame);
	setSegmentStyle(QLCDNumber::Flat);
}

void ChessClock::setTime(int totalTime)
{
	totalTime -= 500;
	QTime timeLeft = QTime().addMSecs(abs(totalTime));

	QString format;
	if (timeLeft.hour() > 0)
		format = "hh:mm:ss";
	else
		format = "mm:ss";

	QString str;
	if (totalTime < 0)
		str += "-";
	str += timeLeft.toString(format);
	setNumDigits(str.length());
	display(str);
}

void ChessClock::start(int totalTime)
{
	m_time.start();
	m_totalTime = totalTime;
	m_timerId = startTimer(1000);
	setTime(totalTime);
}

void ChessClock::stop()
{
	killTimer(m_timerId);
	setTime(m_totalTime - m_time.elapsed());
}

void ChessClock::timerEvent(QTimerEvent* event)
{
	if (!event)
		return;
	
	if (event->timerId() == m_timerId)
		setTime(m_totalTime - m_time.elapsed());
}
