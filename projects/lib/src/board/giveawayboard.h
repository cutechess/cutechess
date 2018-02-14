/*
    This file is part of Cute Chess.

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

#ifndef GIVEAWAYBOARD_H
#define GIVEAWAYBOARD_H

#include "antiboard.h"

namespace Chess {

/*!
 * \brief A board for Giveaway Chess, a Losing Chess variant (ICC wild 26)
 *
 * The side that has no legal moves wins, either by losing all pieces or
 * by being stalemated.
 *
 * The king has no royal powers, so there is no check and the king can be
 * captured. Pawns can also promote to king. Castling is allowed.
 * If a side can capture then they must make a capture move.
 *
 * \note Rules: https://en.wikipedia.org/wiki/Losing_chess
 *
 * \sa AntiBoard
 * \sa SuicideBoard
 *
 * GiveawayBoard uses Polyglot-compatible zobrist position keys,
 * so adequate opening books in Polyglot format can be used.
 *
 * \note Standard Chess Rules: http://www.fide.com/component/handbook/?id=124&view=article
 * \sa PolyglotBook
 */
class LIB_EXPORT GiveawayBoard : public AntiBoard
{
	public:
		/*! Creates a new GiveawayBoard object. */
		GiveawayBoard();

		// Inherited from AntiBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
		virtual void vMakeMove(const Move& move, BoardTransition* transition);

	protected:
		// Inherited from AntiBoard
		virtual bool hasCastling() const;

}; // namespace Chess

}
#endif // GIVEAWAYBOARD_H
