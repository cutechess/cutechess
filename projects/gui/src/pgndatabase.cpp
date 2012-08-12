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
	  m_fileName(fileName)
{
	QFileInfo fileInfo(m_fileName);
	m_displayName = fileInfo.fileName();
}

PgnDatabase::~PgnDatabase()
{
	qDeleteAll(m_entries);
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
	return m_fileName;
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

PgnDatabase::PgnDatabaseError PgnDatabase::game(const PgnGameEntry* entry, PgnGame* game)
{
	Q_ASSERT(entry != 0);
	Q_ASSERT(game != 0);

	QFile file(m_fileName);
	QFileInfo fileInfo(m_fileName);

	if (!fileInfo.exists())
		return DatabaseDoesNotExist;

	if (fileInfo.lastModified() != m_lastModified)
		return DatabaseModified;

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return IoDeviceError;

	PgnStream pgnStream(&file);

	if (!pgnStream.seek(entry->pos(), entry->lineNumber()))
		return StreamError;

	if (!game->read(pgnStream))
		return StreamError;

	return NoError;
}
