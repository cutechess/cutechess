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

#ifndef PIPEREADER_WIN_H
#define PIPEREADER_WIN_H

#include <windows.h>
#include <QThread>
#include <QMutex>
#include <QSemaphore>


/*!
 * \brief A thread class for reading input from a child process
 *
 * PipeReader is intended for reading input from chess engines in Windows.
 * It uses blocking read calls to read from a WinAPI pipe, and
 * sends the readyRead() signal when a new line of text data is available.
 *
 * No event loops are used. The child process has to terminate or be
 * terminated before the pipe reader can exit cleanly.
 *
 * \note This class is for Windows only
 * \sa EngineProcess
 */
class LIB_EXPORT PipeReader : public QThread
{
	Q_OBJECT

	public:
		/*! Creates a new PipeReader and starts the read loop. */
		PipeReader(HANDLE pipe, QObject* parent = nullptr);

		/*!
		 * Read up to \a maxSize bytes into \a data.
		 * \return number of bytes read or -1 if an error occurred.
		 */
		qint64 readData(char* data, qint64 maxSize);

		/*! Returns the number of bytes available for reading. */
		qint64 bytesAvailable() const;

		/*! Returns true if a complete line of data can be read. */
		bool canReadLine() const;

	signals:
		/*! There's a new line of data available. */
		void readyRead();
		
	protected:
		virtual void run();

	private:
		static const int BufSize = 0x8000;

		HANDLE m_pipe;
		char m_buf[BufSize];
		const char* const m_bufEnd;
		char* m_start;
		char* m_end;
		mutable QMutex m_mutex;
		QSemaphore m_freeBytes;
		QSemaphore m_usedBytes;
		int m_lastNewLine;
};

#endif // PIPEREADER_WIN_H
