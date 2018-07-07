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

#ifndef CAPABLANCABOARD_H
#define CAPABLANCABOARD_H

#include "westernboard.h"

namespace Chess {

/*!
 * \brief A board for Capablanca chess
 *
 * Capablanca chess is a variant of standard chess which adds two
 * piece types and is played on a 10x8 board.
 *
 * \note Rules: http://en.wikipedia.org/wiki/Capablanca_chess
 */
class LIB_EXPORT CapablancaBoard : public WesternBoard
{
	public:
		/*! Creates a new CapablancaBoard object. */
		CapablancaBoard();

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual int width() const;
		virtual QString defaultFenString() const;

	protected:
		/*! Special piece types for Capablanca variants. */
		enum CapablancaPieceType
		{
			Archbishop = 7,	//!< Archbishop (knight + bishop)
			Chancellor	//!< Chancellor (knight + rook)
		};

		// Inherited from WesternBoard
		virtual void addPromotions(int sourceSquare,
					   int targetSquare,
					   QVarLengthArray<Move>& moves) const;
};

} // namespace Chess
#endif // CAPABLANCABOARD_H
