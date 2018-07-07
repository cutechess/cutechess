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

#ifndef RESTRICTEDMOVEBOARD_H
#define RESTRICTEDMOVEBOARD_H

#include "westernboard.h"

namespace Chess {

/*!
 * \brief A base class for boards.
 * It is intended for chess variants of standard
 * chess with additional move restrictions
 *
 * \sa GridBoard
 */
class LIB_EXPORT RestrictedMoveBoard : public WesternBoard
{
	protected:
		/*! Creates a new RestrictedMoveBoard object. */
		RestrictedMoveBoard();

		// Inherited from WesternBoard
		virtual Board* copy() const = 0;
		virtual bool vIsLegalMove(const Move& move);
		virtual bool inCheck(Side side, int square = 0) const;

		/*!
		 * Returns true if \a move fulfills the additional game rules.
		 * Set \a reverse to true if this is a move of the opponent
		 * side (not the side to move). Default value is false.
		 */
		virtual bool restriction(const Move& move,
					 bool reverse = false) const = 0;
};

} // namespace Chess
#endif // RESTRICTEDMOVEBOARD_H
