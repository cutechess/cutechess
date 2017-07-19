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

#include "threadedtask.h"
#include <QProgressDialog>

ThreadedTask::ThreadedTask(const QString& title,
			   const QString& labelText,
			   int minimum,
			   int maximum,
			   QWidget* parent)
	: QThread(parent),
	  m_cancel(false),
	  m_statusMessage(labelText),
	  m_taskStart(QTime::currentTime())
{
	m_dlg = new QProgressDialog(tr("%1 - Undefined time remaining").arg(labelText),
						   tr("Cancel"),
						   minimum,
						   maximum,
						   parent);
	m_dlg->setWindowModality(Qt::WindowModal);
	m_dlg->setWindowTitle(title);
	m_dlg->setMinimumDuration(1000);
	m_dlg->setValue(0);

	m_lastUpdate = 0;

	connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
	connect(this, SIGNAL(destroyed()), m_dlg, SLOT(deleteLater()));
	connect(this, SIGNAL(progressValueChanged(int)),
		this, SLOT(updateProgress(int)));
	connect(this, SIGNAL(statusMessageChanged(QString)),
		this, SLOT(setStatusMessage(QString)));
	connect(m_dlg, SIGNAL(canceled()), this, SLOT(cancel()));
}

ThreadedTask::~ThreadedTask()
{
}

void ThreadedTask::cancel()
{
	m_cancel = true;
}

bool ThreadedTask::cancelRequested() const
{
	return m_cancel;
}

void ThreadedTask::updateProgress(int value)
{
	int elapsed = m_taskStart.secsTo(QTime::currentTime());

	if (elapsed > m_lastUpdate)
	{
		m_lastUpdate = elapsed;
		int remainingSecs = (m_dlg->maximum() - value) / (value / elapsed);

		m_dlg->setLabelText(QString("%1 - %2").arg(m_statusMessage,
			humaniseTime(remainingSecs)));
	}

	m_dlg->setValue(value);
}

void ThreadedTask::setStatusMessage(const QString& msg)
{
	m_statusMessage = msg;
}

QString ThreadedTask::humaniseTime(int sec) const
{
	if (sec <= 5)
		return QString(tr("About 5 seconds"));

	if (sec <= 10)
		return QString(tr("About 10 seconds"));

	if (sec <= 60)
		return QString(tr("Less than a minute"));

	if (sec <= 120)
		return QString(tr("About a minute"));

	return QString(tr("About %1 minutes").arg(sec / 60));
}
