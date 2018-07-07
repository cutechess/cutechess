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

#ifndef CHESSCLOCK_H
#define CHESSCLOCK_H

#include <QWidget>
#include <QTime>

class QTimerEvent;
class QLabel;

class ChessClock: public QWidget
{
	Q_OBJECT
	
	public:
		ChessClock(QWidget* parent = nullptr);
	
	public slots:
		void setPlayerName(const QString& name);
		void setInfiniteTime(bool infinite);
		void setTime(int totalTime);
		void start(int totalTime);
		void stop();
	
	protected:
		virtual void timerEvent(QTimerEvent* event);
	
	private:
		void stopTimer();

		int m_totalTime;
		int m_timerId;
		bool m_infiniteTime;
		QTime m_time;
		QLabel* m_nameLabel;
		QLabel* m_timeLabel;
		QPalette m_defaultPalette;
};

#endif // CHESSCLOCK
