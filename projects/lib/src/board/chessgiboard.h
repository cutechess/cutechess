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

#ifndef CHESSGIBOARD_H
#define CHESSGIBOARD_H

#include "loopboard.h"

namespace Chess {

/*!
 * \brief A board for Chessgi
 *
 * Chessgi differs from Loop Chess only in one rule: a side may
 * also drop pawns onto their first rank. The name Chessgi was
 * introduced by Ralph Betza in order to stress its similarities
 * to both Chess and Shogi. Alias names are Drop Chess, Mad Mate,
 * Neo-Chess, among others.
 *
 * \sa LoopBoard
 * \sa CrazyhouseBoard
 *
 * \note Rules: http://www.chessvariants.com/other.dir/chessgi.html
 *
 */
class LIB_EXPORT ChessgiBoard : public LoopBoard
{
	public:
		/*! Creates a new ChessgiBoard object. */
		ChessgiBoard();

		// Inherited from LoopBoard
		virtual Board* copy() const;
		virtual QString variant() const;

	protected:
		// Inherited from LoopBoard (override)
		virtual bool pawnDropOkOnRank(int rank) const;
};

} // namespace Chess
#endif // CHESSGIBOARD_H
