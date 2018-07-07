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

#ifndef FRCBOARD_H
#define FRCBOARD_H

#include "standardboard.h"

namespace Chess {

/*!
 * \brief A board for Fischer Random chess (or Chess 960)
 *
 * Fischer Random is like standard chess, but it uses randomized
 * starting positions, one of which is the starting position of
 * standard chess. FrcBoard uses the same zobrist position keys
 * as StandardBoard, so Polyglot opening books can be used.
 *
 * \note Rules: http://en.wikipedia.org/wiki/Chess960
 * \sa StandardBoard
 * \sa CaparandomBoard
 */
class LIB_EXPORT FrcBoard : public StandardBoard
{
	public:
		/*! Creates a new FrcBoard object. */
		FrcBoard();

		// Inherited from StandardBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual bool isRandomVariant() const;
		/*!
		 * Returns a randomized starting FEN string.
		 *
		 * \note Mersenne::random() is used for the randomization,
		 * so Mersenne::initialize() should be called before calling
		 * this function.
		 */
		virtual QString defaultFenString() const;
};

} // namespace Chess
#endif // FRCBOARD_H
