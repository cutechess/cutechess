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

#ifndef LOOPBOARD_H
#define LOOPBOARD_H

#include "crazyhouseboard.h"

namespace Chess {

/*!
 * \brief A board for Loop Chess
 *
 * Loop Chess is a variant of standard chess where captured
 * pieces can be brought back ("dropped") into the game by
 * the capturing side, similar to Shogi and Crazyhouse.
 * Promoted pieces keep their ranks when captured. Pawns must
 * not be dropped on the first and the eighth rank.
 *
 * \sa CrazyhouseBoard
 * \sa ChessgiBoard
 *
 * \note Rules: http://en.wikipedia.org/wiki/Crazyhouse#Variations
 */
class LIB_EXPORT LoopBoard : public CrazyhouseBoard
{
	public:
		/*! Creates a new LoopBoard object. */
		LoopBoard();

		// Inherited from CrazyhouseBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;

	protected:
		// Inherited from CrazyhouseBoard
		virtual int promotedPieceType(int type) const;
		virtual int maxPieceSymbolLength() const;
};

} // namespace Chess
#endif // LOOPBOARD_H
