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

#ifndef OPENINGSUITE_H
#define OPENINGSUITE_H

#include "pgngame.h"
class QString;
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

		/*!
		 * Creates a new opening suite that reads the openings
		 * from \a fileName in \a format format.
		 * If the file can't be read a null object is created.
		 */
		OpeningSuite(const QString& fileName, Format format);
		/*! Destroys the opening suite. */
		~OpeningSuite();

		/*! Returns the format of the source data. */
		Format format() const;
		/*!
		 * Returns true if the suite contains no data; otherwise
		 * returns false.
		 */
		bool isNull() const;

		/*!
		 * Reads a new opening from the suite and returns it.
		 * A maximum of \a maxPlies plies (halfmoves) are read.
		 */
		PgnGame nextGame(int maxPlies);

	private:
		Format m_format;
		int m_gamesRead;
		QTextStream* m_epdStream;
		PgnStream* m_pgnStream;
};

#endif // OPENINGSUITE_H
