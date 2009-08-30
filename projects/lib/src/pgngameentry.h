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

#ifndef PGNGAMEENTRY_H
#define PGNGAMEENTRY_H

#include "chessboard/chess.h"
#include "chessboard/variant.h"
#include "chessboard/result.h"
class PgnStream;


/*!
 * \brief An entry in a PGN collection.
 *
 * A PgnGameEntry object contains the tags of a PGN game, and
 * the position and line number in a PGN stream.
 * This class was designed for high-performance and low memory
 * consumption, which is useful for quickly loading large game
 * collections.
 *
 * \sa PgnGame, PgnStream
 */
class PgnGameEntry
{
	public:
		/*! Creates a new empty PgnGameEntry object. */
		explicit PgnGameEntry(Chess::Variant variant = Chess::Variant::Standard);

		/*! Resets the entry to an empty default. */
		void clear();
		/*!
		 * Reads an entry from a PGN stream.
		 * Returns true if successfull.
		 */
		bool read(PgnStream& in);

		/*! Returns the stream position where the game begins. */
		qint64 pos() const;
		/*! Returns the line number where the game begins. */
		qint64 lineNumber() const;

		/*! Returns the event/tournament name. */
		QString event() const;
		/*! Returns the site/location where the game was played. */
		QString site() const;
		/*! Returns the round number of a match or tournament. */
		int round() const;
		/*! Returns the name of the white player. */
		QString white() const;
		/*! Returns the name of the black player. */
		QString black() const;
		/*! Returns the game result. */
		Chess::Result result() const;
		/*! Returns the game's chess variant. */
		Chess::Variant variant() const;

	private:
		void addTag(const QString& tagName, const QString& tagValue);

		int m_round;
		qint64 m_pos;
		qint64 m_lineNumber;
		QString m_event;
		QString m_site;
		QString m_white;
		QString m_black;
		Chess::Result m_result;
		Chess::Variant m_variant;
};

#endif // PGNGAMEENTRY_H
