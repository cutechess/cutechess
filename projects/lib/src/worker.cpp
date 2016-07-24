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

#include "worker.h"

Worker::Worker(const QString& title)
	: QObject(nullptr), QRunnable(), m_cancel(false), m_title(title)
{
}

Worker::~Worker()
{
}

void Worker::cancel()
{
	m_cancel = true;
}

bool Worker::cancelRequested() const
{
	return m_cancel;
}

QTime Worker::startTime() const
{
	return m_startTime;
}

QString Worker::title() const
{
	return m_title;
}

void Worker::run()
{
	m_startTime = QTime::currentTime();
	emit started();

	work();
	if (m_cancel)
		emit cancelled();

	emit finished();
}
