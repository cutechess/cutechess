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

#ifndef THREADEDTASK_H
#define THREADEDTASK_H

#include <QThread>
#include <QTime>
class QWidget;
class QProgressDialog;

/*!
 * \brief A long task that is executed in its own thread.
 *
 * ThreadedTask is the base class for tasks that can take a long
 * time and should be executed in a separate thread. ThreadedTask
 * automatically creates a progress dialog with a "cancel" button
 * for the task.
 *
 * The ThreadedTask class should be extended by reimplementing
 * QThread::run() and checking for cancellation by calling
 * cancelRequested() periodically. The subclass should also notify
 * the progress dialog by emitting the progressValueChanged() signal.
 *
 * ThreadedTask destroys itself and the progress dialog automatically
 * after the task is finished or cancelled.
 */
class ThreadedTask : public QThread
{
	Q_OBJECT

	public:
		/*!
		 * Creates a new ThreadedTask object and initializes the
		 * progress dialog.
		 *
		 * The progress dialog is initialized with the window title
		 * \a title, label text \a labelText, and a range from
		 * \a minimum to \a maximum. The dialog is window modal and
		 * its parent is set to \a parent.
		 */
		explicit ThreadedTask(const QString& title,
				      const QString& labelText,
				      int minimum,
				      int maximum,
				      QWidget* parent);
		/*! Destroys the task and its progress dialog. */
		virtual ~ThreadedTask();

	signals:
		/*!
		 * The reimplementation of QThread::run() should emit this
		 * signal periodically to keep the progress dialog informed
		 * of progress.
		 */
		void progressValueChanged(int value);
		/*!
		 * Subclasses should emit this signal when the status
		 * message (ie. the label text) is changed.
		 */
		void statusMessageChanged(const QString& message);

	protected:
		/*!
		 * Returns true if the user had pressed the "cancel" button
		 * on the progress dialog; otherwise returns false.
		 *
		 * The reimplementation of QThread::run() should check this
		 * value periodically and return if cancel was requested.
		 */
		bool cancelRequested() const;

		/*!
		 * Returns human-readable version of the given time \a
		 * sec.
		 */
		QString humaniseTime(int sec) const;
	
	private slots:
		// Yes, QThread subclasses shouldn't normally have slots
		// because they're executed in the wrong thread. But in this
		// case it doesn't matter because we're just setting a flag.
		void cancel();

		void updateProgress(int value);
		void setStatusMessage(const QString& msg);

	private:
		bool m_cancel;
		QString m_statusMessage;
		QTime m_taskStart;
		int m_lastUpdate;
		QProgressDialog* m_dlg;
};

#endif // THREADEDTASK_H
