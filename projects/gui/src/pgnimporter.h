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

#ifndef PGN_IMPORTER_H
#define PGN_IMPORTER_H

#include <QThread>
#include <QString>
#include <QMutex>
#include <QTime>

class PgnDatabase;

class PgnImporter : public QThread
{
	Q_OBJECT

	public:
		PgnImporter(const QString& fileName, QObject* parent = 0);
		QString fileName() const;
		void abort();

		// Inherited from QThread
		void run();

	signals:
		void databaseRead(PgnDatabase* database);
		void databaseReadStatus(const QTime& started, int numReadGames);

	private:
		QString m_fileName;
		bool m_abort;
		QMutex m_mutex;

};

#endif // PGN_IMPORTER_H

