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

#ifndef MINISHOGIBOARD_H
#define MINISHOGIBOARD_H

#include "shogiboard.h"

namespace Chess {

/*!
 * \brief A board for Minishogi (5五将棋 gogo shōgi)
 *
 * Minishogi is a variant of Shogi which is played on a 5x5 board.
 * In this variant either side begins with one King, one Gold General,
 * one SilverGeneral, one Bishop, one Rook (all on their base rank),
 * and one Pawn in front of the King. Shogi rules apply.
 *
 * The promotion zone only consists of the opponent's base rank.
 * In case of a fourfold repetition of moves, the side giving perpetual
 * check loses, else the side starting the game (Sente) loses.
 *
 * Minishogi was introduced in 1970 by Shigenobu Kusumoto, Osaka, Japan.
 *
 * \note Rules: http://en.wikipedia.org/wiki/Minishogi
 *
 * \sa ShogiBoard
 */
class LIB_EXPORT MiniShogiBoard : public ShogiBoard
{
	public:
		/*! Creates a new MiniShogiBoard object. */
		MiniShogiBoard();

		// Inherited from ShogiBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual int height() const;
		virtual int width() const;
		virtual QString defaultFenString() const;
		virtual Result result();

	protected:

};

} // namespace Chess
#endif // MINISHOGIBOARD_H
