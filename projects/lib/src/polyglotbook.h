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

#ifndef POLYGLOT_BOOK_H
#define POLYGLOT_BOOK_H

#include "openingbook.h"

/*!
 * \brief Opening book which uses the Polyglot book format.
 *
 * The Polyglot opening book format is used by chess engines like
 * Fruit, Toga, and Glaurung, and of course the UCI to Xboard adapter
 * Polyglot.
 *
 * Specs: http://alpha.uhasselt.be/Research/Algebra/Toga/book_format.html
 */
class LIB_EXPORT PolyglotBook: public OpeningBook
{
	public:
		/*! Creates a new PolyglotBook with access mode \a mode. */
		PolyglotBook(AccessMode mode = Ram);

	protected:
		// Inherited from OpeningBook
		virtual int entrySize() const;
		virtual Entry readEntry(QDataStream& in, quint64* key) const;
		virtual void writeEntry(const Map::const_iterator& it,
					QDataStream& out) const;
};

#endif // POLYGLOT_BOOK_H
