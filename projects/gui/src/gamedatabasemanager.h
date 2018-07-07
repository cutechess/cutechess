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

#ifndef GAME_DATABASE_MANAGER_H
#define GAME_DATABASE_MANAGER_H

#include <QObject>
#include <QList>

class PgnImporter;
class PgnDatabase;

/*!
 * \brief Manages chess game databases.
 *
 * \sa PgnImporter
 * \sa PgnDatabase
 */
class GameDatabaseManager : public QObject
{
	Q_OBJECT

	public:
		/*! Constructs an empty GameDatabaseManager with \a parent. */
		GameDatabaseManager(QObject* parent = nullptr);
		virtual ~GameDatabaseManager();

		/*!
		 * Returns the list of currently managed databases.
		 */
		QList<PgnDatabase*> databases() const;

		/*!
		 * Writes the state to a file pointed by \a fileName.
		 *
		 * \sa readState
		 */
		bool writeState(const QString& fileName);
		/*!
		 * Reads the state from a file pointed by \a fileName.
		 *
		 * \sa writeState
		 */
		bool readState(const QString& fileName);

		/*! Returns true if the current state has been modified. */
		bool isModified() const;

		/*! Sets the state modified flag to \a modified. */
		void setModified(bool modified);

	public slots:
		/*! Adds \a database to the list of managed databases. */
		void addDatabase(PgnDatabase* database);
		/*!
		 * Remove database at \a index from the list of managed
		 * databases.
		 */
		void removeDatabase(int index);
		/*!
		 * Re-imports database at \a index from the list of managed
		 * databases.
		 */
		void importDatabaseAgain(int index);
		/*!
		 * Imports a game database pointed by \a fileName in PGN format.
		 *
		 * This function is asynchronous and thus returns immediately.
		 *
		 * \sa importStarted
		 */
		void importPgnFile(const QString& fileName);
	signals:
		/*!
		 * Emitted when database is added at \a index.
		 *
		 * \sa databases()
		 */
		void databaseAdded(int index);
		/*!
		 * Emitted when a database is about to be removed at \a index.
		 *
		 * \note The index value is valid when this signal is emitted.
		 *
		 * \sa databases()
		 */
		void databaseAboutToBeRemoved(int index);
		/*!
		 * Emitted when all previously queried database information is now
		 * invalid and must be queried again.
		 *
		 * \sa databases()
		 */
		void databasesReset();

	private:
		QList<PgnDatabase*> m_databases;
		bool m_modified;

};

#endif // GAME_DATABASE_MANAGER_H
