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

#include "gamedatabasemanager.h"

#include <QFileInfo>
#include <QDataStream>
#include <QThreadPool>

#include <pgngameentry.h>

#include "pgndatabase.h"
#include "pgnimporter.h"
#include "importprogressdlg.h"
#include "cutechessapp.h"

#define GAME_DATABASE_STATE_MAGIC   0xDEADD00D
#define GAME_DATABASE_STATE_VERSION 1

GameDatabaseManager::GameDatabaseManager(QObject* parent)
	: QObject(parent),
	  m_modified(false)
{
}

GameDatabaseManager::~GameDatabaseManager()
{
	qDeleteAll(m_databases);
}

QList<PgnDatabase*> GameDatabaseManager::databases() const
{
	return m_databases;
}

bool GameDatabaseManager::writeState(const QString& fileName)
{
	QFile stateFile(fileName);

	if (!stateFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return false;

	QDataStream out(&stateFile);
	out.setVersion(QDataStream::Qt_4_6); // don't change

	// Write magic number and version
	out << (quint32)GAME_DATABASE_STATE_MAGIC;
	out << (quint32)GAME_DATABASE_STATE_VERSION;

	// Write the number of databases
	out << (qint32)m_databases.count();

	// Write the contents of the databases
	for (const PgnDatabase* db : qAsConst(m_databases))
	{
		out << db->fileName();
		out << db->lastModified();
		out << db->displayName();
		out << (qint32)db->entries().count();

		const auto entries = db->entries();
		for (const PgnGameEntry* entry : entries)
			entry->write(out);
	}

	m_modified = false;

	return true;
}

bool GameDatabaseManager::readState(const QString& fileName)
{
	QFile stateFile(fileName);

	if (!stateFile.open(QIODevice::ReadOnly))
		return false;

	QDataStream in(&stateFile);
	in.setVersion(QDataStream::Qt_4_6); // don't change

	// Read and verify the magic value
	quint32 magic;
	in >> magic;

	if (magic != GAME_DATABASE_STATE_MAGIC)
	{
		qWarning("GameDatabaseManager: bad magic value in state file");
		return false;
	}

	// Read and verify the version number
	quint32 version;
	in >> version;

	if (version < GAME_DATABASE_STATE_VERSION ||
	    version > GAME_DATABASE_STATE_VERSION)
	{
		// TODO: Add backward compatibility
		qWarning("GameDatabaseManager: state file version mismatch");
		return false;
	}

	// Read the number of databases
	qint32 dbCount;
	in >> dbCount;

	// Read the contents of the databases
	QString dbFileName;
	QDateTime dbLastModified;
	QString dbDisplayName;
	QList<PgnDatabase*> readDatabases;

	for (int i = 0; i < dbCount; i++)
	{
		in >> dbFileName;
		in >> dbLastModified;
		in >> dbDisplayName;

		// Check if the database exists
		QFileInfo fileInfo(dbFileName);
		if (!fileInfo.exists())
		{
			m_modified = true;
			continue;
		}

		// Check if the database has been modified
		if (fileInfo.lastModified() > dbLastModified)
		{
			m_modified = true;
			importPgnFile(dbFileName);
			continue;
		}

		qint32 dbEntryCount;
		in >> dbEntryCount;

		// Read the entries
		QList<const PgnGameEntry*> entries;
		for (int j = 0; j < dbEntryCount; j++)
		{
			PgnGameEntry* entry = new PgnGameEntry;
			entry->read(in);
			entries << entry;
		}

		PgnDatabase* db = new PgnDatabase(dbFileName);
		db->setEntries(entries);
		db->setLastModified(dbLastModified);
		db->setDisplayName(dbDisplayName);

		readDatabases << db;
	}

	m_modified = false;

	m_databases = readDatabases;
	emit databasesReset();

	return true;
}

void GameDatabaseManager::importPgnFile(const QString& fileName)
{
	PgnImporter* pgnImporter = new PgnImporter(fileName);
	connect(pgnImporter, SIGNAL(databaseRead(PgnDatabase*)),
		this, SLOT(addDatabase(PgnDatabase*)));

	auto dlg = new ImportProgressDialog(pgnImporter);
	dlg->show();
	dlg->raise();
	dlg->activateWindow();

	QThreadPool::globalInstance()->start(pgnImporter);
}

void GameDatabaseManager::addDatabase(PgnDatabase* database)
{
	m_databases << database;
	m_modified = true;
	emit databaseAdded(m_databases.count() - 1);
}

void GameDatabaseManager::removeDatabase(int index)
{
	emit databaseAboutToBeRemoved(index);
	m_databases.removeAt(index);
	m_modified = true;
}

void GameDatabaseManager::importDatabaseAgain(int index)
{
	const QString fileName = m_databases.at(index)->fileName();

	removeDatabase(index);
	importPgnFile(fileName);
}

bool GameDatabaseManager::isModified() const
{
	return m_modified;
}

void GameDatabaseManager::setModified(bool modified)
{
	m_modified = modified;
}
