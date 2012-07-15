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

OpeningSuite::OpeningSuite(const QString& fileName, Format format)
	: m_format(format),
	  m_gamesRead(0),
	  m_epdStream(0),
	  m_pgnStream(0)
{
	QFile* file = new QFile(fileName);
	if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qWarning("Can't open file %s", qPrintable(fileName));
		delete file;
	}
	else if (format == EpdFormat)
		m_epdStream = new QTextStream(file);
	else if (format == PgnFormat)
		m_pgnStream = new PgnStream(file);
}

OpeningSuite::~OpeningSuite()
{
	if (m_epdStream != 0)
	{
		delete m_epdStream->device();
		delete m_epdStream;
	}
	if (m_pgnStream != 0)
	{
		delete m_pgnStream->device();
		delete m_pgnStream;
	}
}

OpeningSuite::Format OpeningSuite::format() const
{
	return m_format;
}

bool OpeningSuite::isNull() const
{
	return m_epdStream == 0 && m_pgnStream == 0;
}

PgnGame OpeningSuite::nextGame(int maxPlies)
{
	PgnGame game;
	if (isNull())
		return game;

	bool ok = false;
	if (m_format == EpdFormat)
	{
		EpdRecord epd;
		ok = epd.parse(*m_epdStream);

		// Rewind the EPD input file
		if (!ok && m_gamesRead > 0 && m_epdStream->atEnd())
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
		ok = game.read(*m_pgnStream, maxPlies);

		// Rewind the PGN input file
		if (!ok && m_gamesRead > 0)
		{
			m_pgnStream->rewind();
			ok = game.read(*m_pgnStream, maxPlies);
		}
	}

	if (ok)
		m_gamesRead++;
	return game;
}
