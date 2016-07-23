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

#ifndef WORKER_POOL_H
#define WORKER_POOL_H

#include <QObject>
#include <QList>

class Worker;
class QThread;

/*
 * \brief A manager for Worker classes
 *
 * The WorkerPool manages a collection of Worker classes.
 */
class WorkerPool : public QObject
{
	Q_OBJECT

	public:
	    /*! Creates a new WorkerPool object with the given \a parent. */
	    explicit WorkerPool(QObject* parent = nullptr);
	    ~WorkerPool();
	    /*! Start \a worker in a separate thread of execution. WorkerPool will
	     * take the ownership of the Worker object and delete it after it has finished. */
	    void start(Worker* worker);

	private:
	    struct WorkerTask
	    {
		Worker* worker;
		QThread* thread;
	    };
	    QList<WorkerTask> m_tasks;
};

#endif // WORKER_POOL_H
