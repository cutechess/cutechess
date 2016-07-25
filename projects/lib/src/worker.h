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
#include <QRunnable>
#include <QTime>
#include <QString>

/*!
 * An abstraction of a long-running task.
 */
class LIB_EXPORT Worker : public QObject, public QRunnable
{
	Q_OBJECT

	public:
		/*! Creates a new Worker object with the given \a title.
		 *
		 * The title describes the purpose of this Worker so that it can be
		 * identified.
		 */
		explicit Worker(const QString& title);
		/*! Destructs the Worker object. */
		virtual ~Worker();
		/*! Returns the time when the worker was started. */
		QTime startTime() const;
		/*! Returns the title of the worker. */
		QString title() const;

		// Inherited from QRunnable
		void run() override;

	signals:
		/*!
		 * Emitted after the worker has been started but before
		 * any tasks are performed.
		 *
		 * This signal is emitted automatically should not manually emitted.
		 */
		void started();
		/*! Emitted when the worker has made progress.
		 *
		 * The value should be in the range 0 - 100.
		 * */
		void progressChanged(int value);
		/*! Emitted in when the status of the worker has changed.
		 *
		 * The purpose of this signal is to give a textual description on
		 * what tasks are currently performed.
		 *
		 * In case of an error this signal can be used to give a textual
		 * description of the error.
		 * */
		void statusChanged(const QString& statusMessage);
		/*! Emitted if the user requested to cancel the worker.
		 *
		 * This signal is emitted automatically just before the
		 * finished() signal and should not manually emitted.
		 */
		void cancelled();
		/*! Emitted after the worker has finished its tasks.
		 *
		 * This signal is automatically emitted and
		 * should not manually emitted.
		 */
		void finished();
		/*!
		 * Signals that an error \a error has occured during the
		 * execution of the task. The meaning of \a error depends
		 * on the implementation.
		 */
		void error(int error);

	public slots:
		/*! Request cancellation of the worker.
		 *
		 * After this slot has been called the worker should try to
		 * cancel its tasks as quickly as possible. The finished()
		 * signal is called after the cancellation is complete.
		 */
		void cancel();

	protected:
		/*! Perform the long running task.
		 *
		 * Periodically check for cancelRequested()
		 * to see if the work should be cancelled.
		 *
		 * Optionally emit the progressChanged and
		 * statusChanged to reflect the current status of
		 * the work.
		 *
		 * If any error occurs emit the error() and statusChanged()
		 * signals and return from the function.
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
