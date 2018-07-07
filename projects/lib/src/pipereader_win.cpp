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

#include "pipereader_win.h"
#include <QMutexLocker>


PipeReader::PipeReader(HANDLE pipe, QObject* parent)
	: QThread(parent),
	  m_pipe(pipe),
	  m_bufEnd(m_buf + BufSize),
	  m_start(m_buf),
	  m_end(m_buf),
	  m_freeBytes(BufSize),
	  m_usedBytes(0),
	  m_lastNewLine(1)
{
	Q_ASSERT(m_pipe != INVALID_HANDLE_VALUE);
}

qint64 PipeReader::bytesAvailable() const
{
	return qint64(m_usedBytes.available());
}

bool PipeReader::canReadLine() const
{
	QMutexLocker locker(&m_mutex);
	return m_lastNewLine <= m_usedBytes.available();
}

qint64 PipeReader::readData(char* data, qint64 maxSize)
{
	int n = qMin(int(maxSize), m_usedBytes.available());
	if (n <= 0)
		return -1;
	m_usedBytes.acquire(n);

	// Copy the first (possibly the only) block of data
	int size1 = qMin(int(m_bufEnd - m_start), n);
	memcpy(data, m_start, size_t(size1));
	m_start += size1;

	Q_ASSERT(m_start <= m_bufEnd);
	if (m_start == m_bufEnd)
		m_start = m_buf;

	// Copy the second block of data from the beginning
	int size2 = n - size1;
	if (size2 > 0)
	{
		memcpy(data + size1, m_start, size_t(size2));
		m_start += size2;

		Q_ASSERT(m_start <= m_bufEnd);
		if (m_start == m_bufEnd)
			m_start = m_buf;
	}
	Q_ASSERT(n == size1 + size2);

	m_freeBytes.release(n);
	return n;
}

void PipeReader::run()
{
	DWORD dwRead = 0;

	for (;;)
	{
		int maxSize = qMin(BufSize / 10, int(m_bufEnd - m_end));
		m_freeBytes.acquire(maxSize);

		BOOL ok = ReadFile(m_pipe, m_end, maxSize, &dwRead, 0);
		if (!ok || dwRead == 0)
		{
			DWORD err = GetLastError();
			if (err != ERROR_INVALID_HANDLE
			&&  err != ERROR_BROKEN_PIPE)
				qWarning("ReadFile failed with 0x%x", int(err));
			return;
		}

		m_end += dwRead;
		Q_ASSERT(m_end <= m_bufEnd);

		QMutexLocker locker(&m_mutex);

		m_lastNewLine += dwRead;
		for (int i = 1; i <= int(dwRead); i++)
		{
			if (*(m_end - i) == '\n')
			{
				m_lastNewLine = i;
				break;
			}
		}

		if (m_end == m_bufEnd)
			m_end = m_buf;

		m_freeBytes.release(maxSize - dwRead);
		m_usedBytes.release(dwRead);

		// To avoid signal spam, send the 'readyRead' signal only
		// if we have a whole line of new data
		if (m_lastNewLine <= int(dwRead))
			emit readyRead();
	}
}
