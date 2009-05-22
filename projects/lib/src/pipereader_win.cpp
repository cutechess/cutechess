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

#include "pipereader_win.h"


PipeReader::PipeReader(HANDLE pipe, QObject* parent)
	: QThread(parent),
	  m_pipe(pipe),
	  m_start(0),
	  m_end(0),
	  m_bytesLeft(0),
	  m_lastLineBreak(0)
{
	Q_ASSERT(m_pipe != INVALID_HANDLE_VALUE);
}

qint64 PipeReader::bytesAvailable() const
{
	return m_bytesLeft;
}

bool PipeReader::canReadLine() const
{
	return m_lastLineBreak > 0;
}

qint64 PipeReader::readData(char* data, qint64 maxSize)
{
	QMutexLocker locker(&m_mutex);
	if (m_bytesLeft <= 0)
		return -1;

	// Bytes to read
	qint64 n = qMin(maxSize, m_bytesLeft);
	// Maximum size of the first (possibly the only) block of data
	qint64 size1 = (qint64)(BufSize - (m_start - m_data));

	// The data is in one contiguous block
	if (size1 >= n || m_end > m_start)
	{
		memcpy(data, m_start, (size_t)n);
		if (n == size1)
			m_start = m_data;
		else
			m_start += n;
	}
	// The first part of the data is at the end of the buffer,
	// and the second part at the beginning
	else
	{
		memcpy(data, m_start, (size_t)size1);
		qint64 size2 = n - size1;
		memcpy(data + size1, m_data, (size_t)size2);
		m_start = m_data + size2;
	}

	m_bytesLeft -= n;
	m_lastLineBreak -= n;

	if (m_bytesLeft < BufSize / 2)
		m_cond.wakeOne();

	return n;
}

// Finds the last newline character in a block of 'size' bytes which ends
// at 'end'. Keeping track of the last newline allows us to quickly tell if
// a whole line can be read.
//
// Sets 'm_lastLineBreak' to a new value and returns true if successfull.
bool PipeReader::findLastNewline(const char* end, int size)
{
	for (int i = 0; i < size; i++)
	{
		if (*end == '\n')
		{
			m_lastLineBreak = m_bytesLeft - i;
			return true;
		}
		end--;
	}

	return false;
}

void PipeReader::run()
{
	const char* bufEnd = m_data + BufSize;
	int chunkSize = 0;
	DWORD dwRead = 0;
	m_start = m_data;
	m_end = m_start;
	m_bytesLeft = 0;

	forever
	{
		QMutexLocker locker(&m_mutex);

		// Use a chunk size as large as possible, but still limited
		// to half of the buffer.
		if (m_end >= m_start)
			chunkSize = bufEnd - m_end;
		else
			chunkSize = m_start - m_end;
		chunkSize = qMin(chunkSize, BufSize / 2);
		Q_ASSERT(chunkSize > 0 && chunkSize <= BufSize);

		// Wait until more than half of the buffer is free
		if (m_bytesLeft >= BufSize / 2)
			m_cond.wait(&m_mutex);

		locker.unlock();
		BOOL ok = ReadFile(m_pipe, m_end, chunkSize, &dwRead, 0);
		if (!ok || dwRead == 0)
		{
			DWORD err = GetLastError();
			if (err != ERROR_INVALID_HANDLE
			&&  err != ERROR_BROKEN_PIPE)
				qWarning("ReadFile failed with 0x%x", (int)err);
			return;
		}

		locker.relock();
		m_bytesLeft += dwRead;
		Q_ASSERT(m_bytesLeft <= BufSize);
		m_end += dwRead;
		bool sendReady = findLastNewline(m_end - 1, dwRead);
		if (m_end >= bufEnd)
			m_end = m_data;

		locker.unlock();
		// To avoid signal spam, send the 'readyRead' signal only
		// if we have a whole line of new data
		if (sendReady)
			emit readyRead();
	}
}
