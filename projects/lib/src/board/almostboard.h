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

#ifndef ALMOSTBOARD_H
#define ALMOSTBOARD_H

#include "westernboard.h"

namespace Chess {

/*!
 * \brief A board for Almost Chess
 *
 * Almost Chess is a variant of standard chess where the Queens are
 * replaced by Chancellor pieces with Rook and Knight movements.
 *
 * This variant was introduced in 1977 by Ralph Betza, USA.
 *
 * \note Rules: https://en.wikipedia.org/wiki/Almost_Chess
 *
 * \sa ChancellorBoard
 * \sa CapablancaBoard
 */
class LIB_EXPORT AlmostBoard : public WesternBoard
{
	public:
		/*! Creates a new AlmostBoard object. */
		AlmostBoard();

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
	protected:
		enum AlmostPieceType
		{
			Chancellor = Queen //!< Chancellor (knight + rook) replaces Queen
		};
};

} // namespace Chess
#endif // ALMOSTBOARD_H
