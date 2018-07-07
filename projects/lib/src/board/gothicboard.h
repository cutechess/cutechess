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

#ifndef GOTHICBOARD_H
#define GOTHICBOARD_H

#include "capablancaboard.h"

namespace Chess {

/*!
 * \brief A board for Gothic chess
 *
 * Gothic chess is a variant of Capablanca chess that uses a
 * different starting position.
 *
 * \note Rules: http://www.gothicchess.com/rules.html
 * \sa CapablancaBoard
 */
class LIB_EXPORT GothicBoard : public CapablancaBoard
{
	public:
		/*! Creates a new GothicBoard object. */
		GothicBoard();

		// Inherited from CapablancaBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
};

} // namespace Chess
#endif // GOTHICBOARD_H
