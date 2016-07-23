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

#include "workerpool.h"

#include <QThread>
#include "worker.h"

WorkerPool::WorkerPool(QObject* parent)
	: QObject(parent)
{
}

WorkerPool::~WorkerPool()
{
	foreach (auto& task, m_tasks)
	{
		if (!task.thread->isFinished())
		{
			task.worker->cancel();
			task.thread->wait();
		}
		delete task.thread;
	}

	m_tasks.clear();
}

void WorkerPool::start(Worker* worker)
{
	auto thread = new QThread();
	worker->moveToThread(thread);

	connect(thread, &QThread::started, worker, &Worker::start);
	connect(worker, &Worker::finished, thread, &QThread::quit);

	connect(thread, &QThread::finished, worker, &QObject::deleteLater);

	WorkerTask task;
	task.worker = worker;
	task.thread = thread;
	m_tasks << task;
	thread->start();
}
