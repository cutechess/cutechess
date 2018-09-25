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

#ifndef CODRUSBOARD_H
#define CODRUSBOARD_H

#include "giveawayboard.h"

namespace Chess {

/*!
 * \brief A board for the Game of Codrus, a Losing Chess variant
 *
 * The Game of Codrus is a predecessor of Losing Chess.
 *
 * The King has no royal powers, so there is no check and the King can be
 * captured. Castling is allowed. If a side can capture then they must make
 * a capture move.
 *
 * The side whose King is captured wins.
 *
 * The Game of Codrus has been described in J. Brede's Alamanach, Denmark 1844.
 * It is named after the Athenian King Codrus who sacrificed himself to save
 * his people (1068 BC).
 *
 * \note Rules: J. Brede (1844): Almanach für Freunde vom Schachspiel,
 * Altona, (then) Denmark: J. F. Hammreich. p. 82f (in German language).
 * Retrieved from: http://www.binnewirtz.com/Brede.htm.
 *
 * \note Also see Pritchard, D. B. (2007). Beasley, John, ed. The Classified
 * Encyclopedia of Chess Variants. Harpenden, UK: John Beasley. ch. 10.9.
 * Retrieved from https://www.jsbeasley.co.uk/encyc.htm.
 *
 * \sa AntiBoard
 * \sa GiveawayBoard
 */

class LIB_EXPORT CodrusBoard : public GiveawayBoard
{
	public:
		/*! Creates a new CodrusBoard object. */
		CodrusBoard();

		// Inherited from GiveawayBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual Result result();

	protected:
		// Inherited from GiveawayBoard
		virtual bool kingsCountAssertion(int whiteKings,
						 int blackKings) const;
		virtual void addPromotions(int sourceSquare,
					   int targetSquare,
					   QVarLengthArray<Move>& moves) const;
		virtual bool vIsLegalMove(const Move& move);

		/*! Rules outcome of stalemate */
		virtual Result vResultOfStalemate() const;

	private:
		int pieceCount(Side side,
			       int ptype = Piece::NoPiece) const;

}; // namespace Chess
}
#endif // CODRUSBOARD_H
