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

#ifndef GAME_DATABASE_MANAGER_H
#define GAME_DATABASE_MANAGER_H

#include <QObject>
#include <QList>

class PgnImporter;
class PgnDatabase;

class GameDatabaseManager : public QObject
{
	Q_OBJECT

	public:
		GameDatabaseManager(QObject* parent = 0);
		~GameDatabaseManager();

		QList<PgnDatabase*> databases() const;

		bool writeState(const QString& fileName);
		bool readState(const QString& fileName);
		void importPgnFile(const QString& fileName);

		bool isModified() const;
		void setModified(bool modified);

	public slots:
		void addDatabase(PgnDatabase* database);

	signals:
		void databaseAdded(int index);
		void databaseAboutToBeRemoved(int index);
		void databasesReset();
		void importStarted(PgnImporter* importer);

	private:
		QList<PgnImporter*> m_pgnImporters;
		QList<PgnDatabase*> m_databases;
		bool m_modified;

};

#endif // GAME_DATABASE_MANAGER_H
