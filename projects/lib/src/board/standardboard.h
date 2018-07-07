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

#ifndef STANDARDBOARD_H
#define STANDARDBOARD_H

#include "westernboard.h"

namespace Chess {

/*!
 * \brief A board for standard chess
 *
 * This is the most common chess variant, and one that is
 * supported by almost all chess engines.
 *
 * StandardBoard uses Polyglot-compatible zobrist position keys,
 * so Polyglot opening books can be used easily.
 *
 * \note Rules: http://www.fide.com/component/handbook/?id=124&view=article
 * \sa PolyglotBook
 */
class LIB_EXPORT StandardBoard : public WesternBoard
{
	public:
		/*! Creates a new StandardBoard object. */
		StandardBoard();

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
		virtual Result tablebaseResult(unsigned int* dtm = nullptr) const;
};

} // namespace Chess
#endif // STANDARDBOARD_H
