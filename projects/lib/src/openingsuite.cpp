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

#include "openingsuite.h"
#include <QFile>
#include <QTextStream>
#include "pgnstream.h"
#include "epdrecord.h"
#include "mersenne.h"

OpeningSuite::OpeningSuite(const QString& fen)
	: m_format(EpdFormat),
	  m_order(SequentialOrder),
	  m_gamesRead(0),
	  m_gameIndex(0),
	  m_startIndex(0),
	  m_fen(fen),
	  m_file(nullptr),
	  m_epdStream(nullptr),
	  m_pgnStream(nullptr)
{
}

OpeningSuite::OpeningSuite(const QString& fileName,
			   Format format,
			   Order order,
			   int startIndex)
	: m_format(format),
	  m_order(order),
	  m_gamesRead(0),
	  m_gameIndex(0),
	  m_startIndex(startIndex),
	  m_fileName(fileName),
	  m_file(nullptr),
	  m_epdStream(nullptr),
	  m_pgnStream(nullptr)
{
}

OpeningSuite::~OpeningSuite()
{
	if (m_epdStream != nullptr)
	{
		delete m_epdStream->device();
		delete m_epdStream;
	}
	if (m_pgnStream != nullptr)
	{
		delete m_pgnStream->device();
		delete m_pgnStream;
	}
}

OpeningSuite::Format OpeningSuite::format() const
{
	return m_format;
}

OpeningSuite::Order OpeningSuite::order() const
{
	return m_order;
}

bool OpeningSuite::isNull() const
{
	return m_epdStream == nullptr && m_pgnStream == nullptr;
}

bool OpeningSuite::initialize()
{
	if (!m_fen.isEmpty())
		return true;

	m_gamesRead = 0;
	m_gameIndex = 0;
	m_filePositions.clear();

	if (m_epdStream != nullptr)
	{
		delete m_epdStream->device();
		delete m_epdStream;
		m_epdStream = nullptr;
	}
	if (m_pgnStream != nullptr)
	{
		delete m_pgnStream->device();
		delete m_pgnStream;
		m_pgnStream = nullptr;
	}

	m_file = new QFile(m_fileName);
	if (!m_file->open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qWarning("Can't open opening suite %s",
			 qUtf8Printable(m_fileName));
		delete m_file;
		return false;
	}

	if (m_format == PgnFormat)
		m_pgnStream = new PgnStream(m_file);

	if (m_order == RandomOrder)
	{
		// Create a shuffled vector of file positions
		forever
		{
			FilePosition pos;
			if (m_format == EpdFormat)
				pos = getEpdPos();
			else if (m_format == PgnFormat)
				pos = getPgnPos();

			if (pos.pos == -1)
				break;

			int i = Mersenne::random() % (m_filePositions.size() + 1);
			if (i == m_filePositions.size())
				m_filePositions.append(pos);
			else
			{
				m_filePositions.append(m_filePositions.at(i));
				m_filePositions[i] = pos;
			}
		}
	}
	else if (m_order == SequentialOrder)
	{
		for (int i = 0; i < m_startIndex; i++)
		{
			FilePosition pos;
			if (m_format == EpdFormat)
				pos = getEpdPos();
			else if (m_format == PgnFormat)
				pos = getPgnPos();

			if (pos.pos == -1)
				break;
		}
	}

	if (m_format == EpdFormat)
	{
		m_file->reset();
		m_epdStream = new QTextStream(m_file);
	}

	return true;
}

PgnGame OpeningSuite::nextGame(int maxPlies)
{
	PgnGame game;

	if (!m_fen.isEmpty())
	{
		Chess::Side side(m_fen.section(' ', 1, 1));
		game.setStartingFenString(side, m_fen);
	}
	if (isNull())
		return game;

	FilePosition pos = { -1, -1 };
	if (m_order == RandomOrder)
	{
		pos = m_filePositions.at(m_gameIndex++);
		if (m_gameIndex >= m_filePositions.size())
			m_gameIndex = 0;
	}

	bool ok = false;
	if (m_format == EpdFormat)
	{
		if (pos.pos != -1)
		{
			m_epdStream->seek(pos.pos);
			m_epdStream->resetStatus();
		}

		EpdRecord epd;
		ok = epd.parse(*m_epdStream);

		// Rewind the EPD input file
		if (m_order == SequentialOrder
		&&  !ok && m_gamesRead > 0 && m_epdStream->atEnd())
		{
			m_epdStream->seek(0);
			m_epdStream->resetStatus();
			ok = epd.parse(*m_epdStream);
		}

		Chess::Side side(epd.fen().section(' ', 1, 1));
		game.setStartingFenString(side, epd.fen());
	}
	else if (m_format == PgnFormat)
	{
		if (pos.pos != -1)
			m_pgnStream->seek(pos.pos, pos.lineNumber);

		ok = game.read(*m_pgnStream, maxPlies);

		// Rewind the PGN input file
		if (m_order == SequentialOrder
		&&  !ok && m_gamesRead > 0)
		{
			m_pgnStream->rewind();
			ok = game.read(*m_pgnStream, maxPlies);
		}
	}

	if (ok)
		m_gamesRead++;
	return game;
}

OpeningSuite::FilePosition OpeningSuite::getPgnPos()
{
	FilePosition pos = { -1, -1 };
	if (!m_pgnStream->nextGame())
		return pos;

	pos.pos = m_pgnStream->pos();
	pos.lineNumber = m_pgnStream->lineNumber();

	char c;
	bool inTag = false;
	bool inQuotes = false;

	while ((c = m_pgnStream->readChar()) != 0)
	{
		if (!inTag)
		{
			if (c == '[')
				inTag = true;
			else if (!isspace(c))
			{
				m_pgnStream->rewindChar();
				break;
			}

			continue;
		}

		if ((c == ']' && !inQuotes) || c == '\n' || c == '\r')
		{
			inTag = false;
			inQuotes = false;
		}
		else if (c == '\"')
			inQuotes = !inQuotes;
	}

	return pos;
}

OpeningSuite::FilePosition OpeningSuite::getEpdPos()
{
	FilePosition pos = { m_file->pos(), -1 };

	while (m_file->readLine().isEmpty())
	{
		if (m_file->atEnd())
		{
			pos.pos = -1;
			break;
		}
		else
			pos.pos = m_file->pos();
	}

	return pos;
}
