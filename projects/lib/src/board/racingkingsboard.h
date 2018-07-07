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

#ifndef RACINGKINGSBOARD_H
#define RACINGKINGSBOARD_H

#include "westernboard.h"

namespace Chess {

/*!
 * \brief A board for Racing Kings Chess
 *
 * Racing Kings Chess (originally named Dodo Chess) is a variant of standard
 * chess by Vernon R. Parton, UK, 1961.
 *
 * This race game is played without pawns. The starting position has pieces on
 * ranks one and two. Moving the king to the eighth rank wins. However, the
 * game is drawn when the black king reaches the eighth rank immediately after
 * the white king. Giving check and entering check is forbidden.
 *
 * \note Rules: https://en.wikipedia.org/wiki/V._R._Parton#Racing_Kings
 * */
class LIB_EXPORT RacingKingsBoard : public WesternBoard
{
	public:
		/*! Creates a new RacingKingsBoard object. */
		RacingKingsBoard();

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual Result result();
		virtual QString defaultFenString() const;

	protected:
		virtual bool isLegalPosition();

	private:
		bool finished(Side side) const;
		bool canFinish(Side side);
};

} // namespace Chess
#endif // RACINGKINGSBOARD_H
