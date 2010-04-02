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

#include "pgnstream.h"
#include <QIODevice>


PgnStream::PgnStream()
	: m_board(0),
	  m_pos(0),
	  m_lineNumber(1),
	  m_device(0),
	  m_string(0),
	  m_status(Ok)
{
}

PgnStream::PgnStream(Chess::Board* board, QIODevice* device)
	: m_board(board)
{
	setDevice(device);
}

PgnStream::PgnStream(Chess::Board* board, const QString* string)
	: m_board(board)
{
	setString(string);
}

void PgnStream::reset()
{
	m_pos = 0;
	m_lineNumber = 1;
	m_device = 0;
	m_string = 0;
	m_status = Ok;
}

Chess::Board* PgnStream::board()
{
	return m_board;
}

QIODevice* PgnStream::device() const
{
	return m_device;
}

void PgnStream::setDevice(QIODevice* device)
{
	Q_ASSERT(device != 0);
	reset();
	m_pos = device->pos();
	m_device = device;
}

const QString* PgnStream::string() const
{
	return m_string;
}

void PgnStream::setString(const QString* string)
{
	Q_ASSERT(string != 0);
	reset();
	m_string = string;
}

bool PgnStream::isOpen() const
{
	return (m_device && m_device->isOpen()) || m_string;
}

qint64 PgnStream::pos() const
{
	return m_pos;
}

qint64 PgnStream::lineNumber() const
{
	return m_lineNumber;
}

QChar PgnStream::readChar()
{
	QChar c;
	if (m_device)
	{
		if (!m_device->getChar(&m_lastChar))
		{
			m_status = ReadPastEnd;
			return QChar();
		}
		c = QChar(m_lastChar);
	}
	else if (m_string && m_pos < m_string->size())
	{
		c = m_string->at(m_pos);
	}
	else
	{
		m_status = ReadPastEnd;
		return QChar();
	}

	m_pos++;
	if (c == '\n')
		m_lineNumber++;

	return c;
}

QString PgnStream::readLine()
{
	QString str;
	QChar c;
	while (!(c = readChar()).isNull() && c != '\n')
		str += c;

	if (c == '\n')
		m_lineNumber++;
	return str;
}

void PgnStream::rewind()
{
	seek(0);
}

void PgnStream::rewindChar()
{
	if (m_pos <= 0)
		return;

	QChar c;
	if (m_device)
	{
		c = QChar(m_lastChar);
		m_device->ungetChar(m_lastChar);
		m_lastChar = 0;
	}
	else if (m_string)
		c = m_string->at(m_pos);
	else
		return;

	if (c == '\n')
		m_lineNumber--;
	m_pos--;
}

bool PgnStream::seek(qint64 pos, qint64 lineNumber)
{
	if (pos < 0)
		return false;

	bool ok = false;
	if (m_device)
		ok = m_device->seek(pos);
	else if (m_string)
		ok = pos < m_string->size();
	if (!ok)
		return false;

	m_status = Ok;
	m_pos = pos;
	m_lineNumber = lineNumber;
	m_lastChar = 0;

	return true;
}

void PgnStream::skipWhiteSpace()
{
	QChar c;
	do
	{
		c = readChar();
	}
	while (c.isSpace());
	rewindChar();
}

PgnStream::Status PgnStream::status() const
{
	return m_status;
}
