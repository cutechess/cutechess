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

#ifndef EMBASSYBOARD_H
#define EMBASSYBOARD_H

#include "capablancaboard.h"

namespace Chess {

/*!
 * \brief A board for Embassy chess
 *
 * Embassy chess is a variant of Capablanca chess that uses a
 * different starting position. It is similar to the line-up
 * of Grand Chess. The king goes to the b- or i-file when castling.
 *
 * This variant was introduced in 2005 by Kevin Hill.
 *
 * \note Rules: https://en.wikipedia.org/wiki/Embassy_Chess
 * \sa CapablancaBoard
 * \sa GothicBoard
 */
class LIB_EXPORT EmbassyBoard : public CapablancaBoard
{
	public:
		/*! Creates a new EmbassyBoard object. */
		EmbassyBoard();

		// Inherited from CapablancaBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
		virtual int castlingFile(CastlingSide castlingSide) const;
};

} // namespace Chess
#endif // EMBASSYBOARD_H
