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

#ifndef THREEKINGSBOARD_H
#define THREEKINGSBOARD_H

#include "westernboard.h"

namespace Chess {

/*!
 * \brief A board for Three Kings Chess
 *
 * Three Kings Chess is a variant of standard chess with three kings
 * on both sides. There is no check. Capturing any king wins.
 *
 * Introduced by Adam Sobey at the Haslemere Chess Club, UK, December 1988.
 * \note Rules: http://en.wikipedia.org/wiki/Chess_variant
 *
 * \note Standard Chess Rules: http://www.fide.com/component/handbook/?id=124&view=article
 */
class LIB_EXPORT ThreeKingsBoard : public WesternBoard
{
	public:
		/*! Creates a new ThreeKingsBoard object. */
		ThreeKingsBoard();

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
		virtual Result result();
	protected:
		virtual bool kingsCountAssertion(int whiteKings,
						 int blackKings) const;
		virtual bool inCheck(Side side, int square = 0) const;
	private:
		int kingCount(Side side) const;
};

} // namespace Chess
#endif // THREEKINGSBOARD_H
