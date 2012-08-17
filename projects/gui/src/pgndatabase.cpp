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

#include "pgndatabase.h"
#include <pgnstream.h>
#include <QFileInfo>

PgnDatabase::PgnDatabase(const QString& fileName, QObject* parent)
	: QObject(parent),
	  m_file(fileName),
	  m_stream(0)
{
	QFileInfo fileInfo(fileName);
	m_displayName = fileInfo.fileName();
}

PgnDatabase::~PgnDatabase()
{
	qDeleteAll(m_entries);
	delete m_stream;
}

void PgnDatabase::setEntries(const QList<const PgnGameEntry*>& entries)
{
	qDeleteAll(m_entries);
	m_entries = entries;
}

QList<const PgnGameEntry*> PgnDatabase::entries() const
{
	return m_entries;
}

QString PgnDatabase::fileName() const
{
	return m_file.fileName();
}

void PgnDatabase::closeFile()
{
	delete m_stream;
	m_stream = 0;
	m_file.close();
}

QDateTime PgnDatabase::lastModified() const
{
	return m_lastModified;
}

void PgnDatabase::setLastModified(const QDateTime& lastModified)
{
	m_lastModified = lastModified;
}

QString PgnDatabase::displayName() const
{
	return m_displayName;
}

void PgnDatabase::setDisplayName(const QString& displayName)
{
	m_displayName = displayName;
}

PgnDatabase::PgnDatabaseError PgnDatabase::game(const PgnGameEntry* entry,
						PgnGame* game,
						int maxPlies,
						bool leaveFileOpen)
{
	Q_ASSERT(entry != 0);
	Q_ASSERT(game != 0);

	if (!m_stream)
	{
		QFileInfo fileInfo(m_file.fileName());

		if (!fileInfo.exists())
			return DatabaseDoesNotExist;

		if (fileInfo.lastModified() != m_lastModified)
			return DatabaseModified;

		if (!m_file.open(QIODevice::ReadOnly | QIODevice::Text))
			return IoDeviceError;

		m_stream = new PgnStream(&m_file);
	}

	PgnDatabaseError error = NoError;

	if (!m_stream->seek(entry->pos(), entry->lineNumber())
	||  !game->read(*m_stream, maxPlies))
		error = StreamError;

	if (!leaveFileOpen)
		closeFile();

	return error;
}
