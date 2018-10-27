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

#ifndef JESONMORBOARD_H
#define JESONMORBOARD_H

#include "westernboard.h"

namespace Chess {

/*!
 * \brief A board for Jeson Mor
 *
 * Jeson Mor is a board game from Mongolia. There are nine knights per
 * side and no other pieces. Initially all knights are lined up on their
 * sides' first rank of a 9x9 board.
 *
 * Leaving the central square e5 or capturing all opponent pieces wins.
 *
 * \note Rules: http://en.wikipedia.org/wiki/Jeson_Mor
 */
class LIB_EXPORT JesonMorBoard : public WesternBoard
{
	public:
		/*! Creates a new JesonMorBoard object. */
		JesonMorBoard();

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual int width() const;
		virtual int height() const;
		virtual QString defaultFenString() const;
		virtual Result result();
	protected:
		// Inherited from WesternBoard
		virtual bool kingsCountAssertion(int whiteKings,
						 int blackKings) const;
		virtual bool inCheck(Side side, int square = 0) const;
	private:
		const int m_centralSquare;
};

} // namespace Chess
#endif // JESONMORBOARD_H
