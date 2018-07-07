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

#ifndef OPENINGSUITE_H
#define OPENINGSUITE_H

#include <QVector>
#include "pgngame.h"
class QString;
class QFile;
class QTextStream;
class PgnStream;

/*!
 * \brief A suite of chess openings
 *
 * This class acts as an abstract interface for accessing a suite
 * of chess openings in EPD or PGN format. An OpeningSuite object
 * reads positions and games from a text stream (eg. a text file)
 * and returns the opening as a PgnGame object.
 *
 * \sa EpdRecord
 * \sa PgnGame
 */
class LIB_EXPORT OpeningSuite
{
	public:
		/*! The format of the opening data. */
		enum Format
		{
			EpdFormat,	//!< EPD format
			PgnFormat	//!< PGN format
		};

		/*! The order in which openings are picked. */
		enum Order
		{
			SequentialOrder,	//!< Sequential order
			RandomOrder		//!< Random order
		};

		/*!
		 * Creates a new opening suite that starts every game at \a fen.
		 */
		OpeningSuite(const QString& fen);
		/*!
		 * Creates a new opening suite that reads the openings
		 * from \a fileName in \a format format.
		 *
		 * Openings will be picked according to \a order.
		 *
		 * If \a order is \a SequentialOrder, \a startIndex will
		 * be the index of the first opening. If \a order is
		 * \a RandomOrder, then setting a start index does nothing.
		 *
		 * \note The created opening suite is null until
		 * initialize() is called.
		 */
		OpeningSuite(const QString& fileName,
			     Format format,
			     Order order = SequentialOrder,
			     int startIndex = 0);
		/*! Destroys the opening suite. */
		~OpeningSuite();

		/*! Returns the format of the source data. */
		Format format() const;
		/*! Returns the order in which openings are picked. */
		Order order() const;
		/*!
		 * Returns true if the suite contains no data; otherwise
		 * returns false.
		 */
		bool isNull() const;

		/*!
		 * Initializes the opening suite.
		 *
		 * If \a order is SequentialOrder, this function just opens
		 * the opening suite file and gets ready to read data. If
		 * \a order is RandomOrder, the file positions of all the
		 * openings are parsed from the file, which could take some
		 * time if the file is large.
		 *
		 * Returns true if successful; otherwise returns false.
		 */
		bool initialize();
		/*!
		 * Reads a new opening from the suite and returns it.
		 * A maximum of \a maxPlies plies (halfmoves) are read.
		 */
		PgnGame nextGame(int maxPlies);

	private:
		struct FilePosition
		{
			qint64 pos;
			qint64 lineNumber;
		};

		FilePosition getPgnPos();
		FilePosition getEpdPos();

		Format m_format;
		Order m_order;
		int m_gamesRead;
		int m_gameIndex;
		int m_startIndex;
		QString m_fileName;
		QString m_fen;
		QFile* m_file;
		QTextStream* m_epdStream;
		PgnStream* m_pgnStream;
		QVector<FilePosition> m_filePositions;
};

#endif // OPENINGSUITE_H
