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
	: QLabel(parent),
	  m_infiniteTime(false),
	  m_totalTime(0),
	  m_timerId(-1)
{
	setTextFormat(Qt::RichText);
	setAlignment(Qt::AlignHCenter);
}

void ChessClock::setPlayerName(const QString& name)
{
	m_playerName = name;
	setTime(m_totalTime);
}

void ChessClock::setInfiniteTime(bool infinite)
{
	killTimer(m_timerId);
	m_infiniteTime = infinite;
	setText(QString("<h2>%1</h2>").arg(m_playerName));
}

void ChessClock::setTime(int totalTime)
{
	QTime timeLeft = QTime().addMSecs(abs(totalTime + 500));

	QString format;
	if (timeLeft.hour() > 0)
		format = "hh:mm:ss";
	else
		format = "mm:ss";

	QString str;
	if (!m_playerName.isEmpty())
		str.append(m_playerName).append(": ");
	if (totalTime <= -500)
		str.append("-");
	str.append(timeLeft.toString(format));

	setText(QString("<h2>%1</h2>").arg(str));
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
