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

#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QTime>
#include <QString>

/*!
 * \brief An abstraction of a long-running task.
 *
 * The worker class is used to run long running task in different
 * threads of execution. The Worker class itself has no knowledge of
 * threads. Instead, WorkerPool is used to run the task in another thread.
 */
class Worker : public QObject
{
	Q_OBJECT

	public:
		/*! Creates a new Worker object with the given \a title.
		 *
		 * The title describes the purpose of this Worker so that it can be
		 * identified.
		 */
		explicit Worker(const QString& title);
		/*! Destructs the Worker object.
		 *
		 * \note WorkerPool should be responsible for destructing the Worker and
		 * freeing its resources.
		 */
		virtual ~Worker();
		/*! Returns the time when the worker was started. */
		QTime startTime() const;
		/*! Returns the title of the worker. */
		QString title() const;

	signals:
		/*! Emitted after the worker has been started but before any tasks are performed. */
		void started();
		/*! Emitted optionally in sub-classes when the worker has made progress. */
		void progressChanged(int value);
		/*! Emitted optionally in sub-classes when the status of the worker has changed. */
		void statusChanged(const QString& statusMessage);
		/*! Emitted if the user requested to cancel the worker.
		 *
		 * \note Emitted just before the finished() signal.
		 */
		void cancelled();
		/*! Emitted after the worker has finished its tasks. */
		void finished();
		/*!
		 * Emitted optionally in sub-classes when an error is encountered.
		 *
		 * The meaning of the \a error parameter depends on the
		 * sub-class.
		 */
		void error(int error);

	public slots:
		void start();
		/*! Request cancellation of the worker.
		 *
		 * After this slot has been called the worker tries to cancel its tasks
		 * as quickly as possible. The finished() signal is called after the
		 * cancellation is complete.
		 */
		void cancel();

	protected:
		/*! Perform the long running task.
		 *
		 * Periodically check for cancelRequested() to see if the work should be cancelled.
		 *
		 * Emit the progressChanged and statusChanged to reflect the current status of the work.
		 *
		 * If any error occurs emit the error and statusChanged signals and return from the function.
		 */
		virtual void work() = 0;
		/*! Returns true if the user requested cancellation of the task with the cancel() slot. */
		bool cancelRequested() const;

	private:
		bool m_cancel;
		QString m_title;
		QTime m_startTime;
};

#endif // WORKER_H
