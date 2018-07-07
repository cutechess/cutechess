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

#ifndef CHECKLESSBOARD_H
#define CHECKLESSBOARD_H

#include "westernboard.h"

namespace Chess {

/*!
 * \brief A board for Checkless Chess
 *
 * Checkless Chess is a variant of standard chess that already
 * has been played in the early 19th century. It is also
 * known as Prohibition Chess.
 *
 * The kings must not be checked unless for mate.
 * Entering check is forbidden.
 *
 * \note Rules: https://en.wikipedia.org/wiki/Checkless_chess
 * */
class LIB_EXPORT ChecklessBoard : public WesternBoard
{
	public:
		/*! Creates a new ChecklessBoard object. */
		ChecklessBoard();

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;

	protected:
		virtual bool isLegalPosition();
};

} // namespace Chess
#endif // CHECKLESSBOARD_H
