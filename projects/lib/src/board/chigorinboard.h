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

#ifndef CHIGORINBOARD_H
#define CHIGORINBOARD_H

#include "westernboard.h"

namespace Chess {

/*!
 * \brief A board for Chigorin Chess
 *
 * Chigorin Chess is a variant of standard chess where the
 * white side has no pieces with Bishop movement but four
 * Knights and one Chancellor with Rook and Knight movements.
 * Black has no pieces with Knight movement but four Bishops
 * and one Queen. Pawn promotion is only allowed to the piece
 * types that a side had at the start of the game (with the
 * exceptions of Pawn and King).
 *
 * This variant was introduced by Ralph Betza in 2002
 *
 * \note Rules: https://en.wikipedia.org/wiki/Chigorin_Chess
 *
 * \sa ChancellorBoard
 */
class LIB_EXPORT ChigorinBoard : public WesternBoard
{
	public:
		/*! Creates a new ChigorinBoard object. */
		ChigorinBoard();

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
	protected:
		enum ChigorinPieceType
		{
			Chancellor = 8 //!< Chancellor (knight + rook)
		};
		// Inherited from WesternBoard
		virtual void addPromotions(int sourceSquare,
					   int targetSquare,
			     QVarLengthArray< Move >& moves) const;
};

} // namespace Chess
#endif // CHIGORINBOARD_H
