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

#ifndef PGNGAMEENTRY_H
#define PGNGAMEENTRY_H

#include <QDate>
#include "board/result.h"
class PgnStream;
class PgnGameFilter;
class QDataStream;


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
class LIB_EXPORT PgnGameEntry
{
	public:
		/*! A PGN tag's type. */
		enum TagType
		{
			EventTag,	//!< The name of the tournament or match event
			SiteTag,	//!< The location of the event
			DateTag,	//!< The starting date of the game
			RoundTag,	//!< The playing round ordinal of the game
			WhiteTag,	//!< The player of the white pieces
			BlackTag,	//!< The player of the black pieces
			ResultTag,	//!< The result of the game
			VariantTag	//!< The chess variant of the game
		};

		/*! Creates a new empty PgnGameEntry object. */
		PgnGameEntry();

		/*! Resets the entry to an empty default. */
		void clear();
		/*!
		 * Reads an entry from a PGN stream.
		 * Returns true if successful; otherwise returns false.
		 */
		bool read(PgnStream& in);

		/*!
		 * Reads an entry from data stream.
		 * Returns true if successful; otherwise returns false.
		 */
		bool read(QDataStream& in);

		/*!
		 * Writes an entry to data stream.
		 */
		void write(QDataStream& out) const;

		/*!
		 * Returns true if the PGN tags match \a filter.
		 * The matching is case insensitive.
		 */
		bool match(const PgnGameFilter& filter) const;

		/*! Returns the stream position where the game begins. */
		qint64 pos() const;
		/*! Returns the line number where the game begins. */
		qint64 lineNumber() const;

		/*! Returns the tag value corresponding to \a type. */
		QString tagValue(TagType type) const;

	private:
		void addTag(const QByteArray& tagValue);

		QByteArray m_data;

		qint64 m_pos;
		qint64 m_lineNumber;
};

/*! Reads a PGN game entry from a PGN stream. */
extern LIB_EXPORT PgnStream& operator>>(PgnStream& in, PgnGameEntry& entry);

/*! Reads a PGN game entry from a data stream. */
extern LIB_EXPORT QDataStream& operator>>(QDataStream& in, PgnGameEntry& entry);

/*! Writes a PGN game entry to a data stream. */
extern LIB_EXPORT QDataStream& operator<<(QDataStream& out,
                                          const PgnGameEntry& entry);

#endif // PGNGAMEENTRY_H
