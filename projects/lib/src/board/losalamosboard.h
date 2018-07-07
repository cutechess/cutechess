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

#ifndef LOSALAMOSBOARD_H
#define LOSALAMOSBOARD_H

#include "westernboard.h"

namespace Chess {

/*!
 * \brief A board for Los Alamos chess
 *
 * Los Alamos chess is a variant of standard chess which is played on a
 * 6x6 board. This variant has no bishops, no pawn double steps, and no
 * castling. It was introduced in 1956 by Paul Stein, Mark Wells, and
 * others with the first chess-like program on the MANIAC I computer of
 * Los Alamos Scientific Laboratory.
 *
 * \note Rules: http://en.wikipedia.org/wiki/Los_Alamos_chess
 */
class LIB_EXPORT LosAlamosBoard : public WesternBoard
{
	public:
		/*! Creates a new LosAlamosBoard object. */
		LosAlamosBoard();

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual int height() const;
		virtual int width() const;
		virtual QString defaultFenString() const;

	protected:
		// Inherited from WesternBoard
		virtual bool hasCastling() const;
		virtual bool pawnHasDoubleStep() const;
		virtual void addPromotions(int sourceSquare,
					   int targetSquare,
					   QVarLengthArray<Move>& moves) const;
};

} // namespace Chess
#endif // LOSALAMOSBOARD_H
