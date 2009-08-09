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
	: m_variant(Chess::Variant::NoVariant),
	  m_lineNumber(1),
	  m_rewind(false)
{
}

PgnStream::PgnStream(QIODevice* device)
	: m_variant(Chess::Variant::NoVariant),
	  m_lineNumber(1),
	  m_rewind(false)
{
	setDevice(device);
}

PgnStream::PgnStream(QString* string)
	: m_variant(Chess::Variant::NoVariant),
	  m_lineNumber(1),
	  m_rewind(false)
{
	m_in.setString(string, QIODevice::ReadOnly);
}

Chess::Board* PgnStream::board()
{
	return &m_board;
}

QIODevice* PgnStream::device() const
{
	return m_in.device();
}

void PgnStream::setDevice(QIODevice* device)
{
	m_in.setDevice(device);
}

QString* PgnStream::string() const
{
	return m_in.string();
}

void PgnStream::setString(QString* string)
{
	m_in.setString(string);
}

bool PgnStream::isOpen() const
{
	return (m_in.device() && m_in.device()->isOpen()) || m_in.string();
}

qint64 PgnStream::lineNumber() const
{
	return m_lineNumber;
}

QChar PgnStream::readChar()
{
	if (m_rewind)
	{
		m_rewind = false;
		return m_lastChar;
	}
	
	m_in >> m_lastChar;
	if (m_lastChar == '\n')
		m_lineNumber++;
	
	return m_lastChar;
}

QString PgnStream::readLine()
{
	m_lineNumber++;
	return m_in.readLine();
}

void PgnStream::rewind()
{
	m_in.resetStatus();
	m_lineNumber = 1;
	m_rewind = false;
	m_in.seek(0);
}

void PgnStream::rewindChar()
{
	m_rewind = true;
}

void PgnStream::skipWhiteSpace()
{
	do
	{
		readChar();
	}
	while (m_lastChar.isSpace());
	m_rewind = true;
}

QTextStream::Status PgnStream::status() const
{
	return m_in.status();
}

Chess::Variant PgnStream::variant() const
{
	return m_variant;
}

void PgnStream::setVariant(Chess::Variant variant)
{
	m_variant = variant;
	if (!m_variant.isNone())
		m_board.setVariant(m_variant);
}
