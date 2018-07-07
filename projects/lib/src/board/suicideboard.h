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

#ifndef SUICIDEBOARD_H
#define SUICIDEBOARD_H

#include "antiboard.h"

namespace Chess {

/*!
 * \brief A board for Suicide Chess, a Losing Chess variant
 *
 * Suicide Chess is a variant of Losing Chess (FICS Suicide).
 *
 * The king has no royal powers, so there is no check and the king can be
 * captured. Pawns can also promote to king. Castling is not allowed.
 * If a side can capture then they must make a capture move.
 *
 * The side that has lost all pieces wins. If a side cannot move (stalemate)
 * then the side with less pieces wins and else the game is drawn.
 *
 * \note Rules: https://en.wikipedia.org/wiki/Losing_chess
 *
 * \sa AntiBoard
 * \sa GiveawayBoard
 *
 * SuicideBoard uses Polyglot-compatible zobrist position keys,
 * so adequate opening books in Polyglot format can be used.
 *
 * \note Standard Chess Rules: http://www.fide.com/component/handbook/?id=124&view=article
 * \sa PolyglotBook
 */
class LIB_EXPORT SuicideBoard : public AntiBoard
{
	public:
		/*! Creates a new SuicideBoard object. */
		SuicideBoard();

		// Inherited from AntiBoard
		virtual Board* copy() const;
		virtual QString variant() const;

	protected:
		// Inherited from AntiBoard
		virtual Result vResultOfStalemate() const;

	private:
		int pieceCount(Side side,
			       int ptype = Piece::NoPiece) const;

}; // namespace Chess

}
#endif // SUICIDEBOARD_H
