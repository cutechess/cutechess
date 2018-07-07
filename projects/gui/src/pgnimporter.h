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

#ifndef PGN_IMPORTER_H
#define PGN_IMPORTER_H

#include <worker.h>

class PgnDatabase;

/*!
 * \brief Reads PGN database in a separate thread.
 *
 * \sa PgnDatabase
 */
class PgnImporter : public Worker
{
	Q_OBJECT

	public:
		/*! Import error. */
		enum Error
		{
			FileDoesNotExist, //!< The PGN file was not found
			IoError           //!< A generic I/O error
		};

		/*!
		 * Constructs a PgnImporter with \a fileName as
		 * database to be imported.
		 */
		PgnImporter(const QString& fileName);
		/*! Returns the file name of the database to be imported. */
		QString fileName() const;

	protected:
		void work() override;

	signals:
		/*! Emitted when \a database is read. */
		void databaseRead(PgnDatabase* database);
		/*!
		 * Emitted periodically to give progress information about the import.
		 *
		 * The import was initiated at \a started and so far \a numReadGames games
		 * and \a numReadBytes bytes have been read.
		 */
		void databaseReadStatus(const QTime& started, int numReadGames, qint64 numReadBytes);

	private:
		QString m_fileName;

};

#endif // PGN_IMPORTER_H

