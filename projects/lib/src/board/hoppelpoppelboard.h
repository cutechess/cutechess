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

#ifndef HOPPELPOPPELBOARD_H
#define HOPPELPOPPELBOARD_H

#include "westernboard.h"

namespace Chess {

/*!
 * \brief A board for Hoppel-Poppel Chess
 *
 * Hoppel-Poppel follows the rules of standard chess, but
 * the Bishop moves like a Knight when capturing and the
 * Knight makes captures diagonally like a Bishop.
 *
 * This variant originates from Germany.
 *
 * \note Rules: https://www.chessvariants.com/diffmove.dir/hoppel-poppel.html
 */


class LIB_EXPORT HoppelPoppelBoard : public WesternBoard
{
	public:
		/*! Creates a new HoppelPoppelBoard object. */
		HoppelPoppelBoard();

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;

	protected:
		// Inherited from WesternBoard
		virtual void generateMovesForPiece(QVarLengthArray< Move >& moves,
						   int pieceType,
						   int square) const;
};

/*!
 * \brief A board for New Zealand Chess
 *
 * New Zealand Chess follows the rules of standard chess, but
 * has Rooks that move like Knights when capturing. The Knights
 * capture like Rooks in standard chess.
 *
 * This variant was published in the British Chess Magazine,
 * September 1903.
 *
 * \note Rules: D. B. Pritchard, The Classified Encyclopedia of
 *              Chess Variants, ed. John Beasley, 2007, ch. 14.4
 */
class LIB_EXPORT NewZealandBoard : public WesternBoard
{
	public:
		/*! Creates a new HoppelPoppelBoard object. */
		NewZealandBoard();

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;

	protected:
		// Inherited from WesternBoard
		virtual void generateMovesForPiece(QVarLengthArray< Move >& moves,
						   int pieceType,
						   int square) const;
};

} // namespace Chess
#endif // HOPPELPOPPELBOARD_H
