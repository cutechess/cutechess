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

#include "pgndatabase.h"
#include <pgnstream.h>
#include <QFileInfo>

PgnDatabase::PgnDatabase(const QString& fileName, QObject* parent)
	: QObject(parent),
	  m_fileName(fileName),
	  m_displayName(QFileInfo(fileName).completeBaseName())
{
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

PgnDatabase::Status PgnDatabase::status() const
{
	QFileInfo info(m_fileName);

	if (!info.exists())
		return DoesNotExist;
	if (!info.isReadable())
		return Unreadable;
	if (info.lastModified() != m_lastModified)
		return Modified;

	return Ok;
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

PgnDatabase::Status PgnDatabase::game(const PgnGameEntry* entry,
				      PgnGame* game)
{
	Q_ASSERT(entry != nullptr);
	Q_ASSERT(game != nullptr);

	Status status = this->status();
	if (status != Ok)
		return status;

	QFile file(m_fileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return Unreadable;

	PgnStream in(&file);
	if (!in.seek(entry->pos(), entry->lineNumber()) || !game->read(in))
		return Corrupted;

	return Ok;
}
