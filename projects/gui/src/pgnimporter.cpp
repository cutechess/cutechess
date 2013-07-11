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

#include "pgnimporter.h"

#include <QFile>
#include <QFileInfo>

#include <pgnstream.h>
#include <pgngameentry.h>
#include "pgndatabase.h"

PgnImporter::PgnImporter(const QString& fileName, QObject* parent)
	: QThread(parent), m_fileName(fileName)
{
	m_abort = false;
}

QString PgnImporter::fileName() const
{
	return m_fileName;
}

void PgnImporter::abort()
{
	m_abort = true;
}

void PgnImporter::run()
{
	QFile file(m_fileName);
	QFileInfo fileInfo(m_fileName);
	const QTime startTime = QTime::currentTime();
	static const int updateInterval = 1024;
	int numReadGames = 0;

	if (!fileInfo.exists())
	{
		emit error(PgnImporter::FileDoesNotExist);
		return;
	}

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		emit error(PgnImporter::IoError);
		return;
	}

	PgnStream pgnStream(&file);
	QList<const PgnGameEntry*> games;

	forever
	{
		PgnGameEntry* game = new PgnGameEntry;
		if (m_abort || !game->read(pgnStream))
		{
			delete game;
			break;
		}

		games << game;
		numReadGames++;

		if (numReadGames % updateInterval == 0)
			emit databaseReadStatus(startTime, numReadGames,
			    pgnStream.pos());
	}
	PgnDatabase* db = new PgnDatabase(m_fileName);
	db->setEntries(games);
	db->setLastModified(fileInfo.lastModified());

	emit databaseRead(db);
}
