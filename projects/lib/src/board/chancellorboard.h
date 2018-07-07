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

#ifndef CHANCELLORBOARD_H
#define CHANCELLORBOARD_H

#include "westernboard.h"

namespace Chess {

/*!
 * \brief A board for Chancellor Chess
 *
 * Chancellor Chess is a variant of chess on a 9x9 board. Each side has an
 * additional Pawn and a Chancellor piece with Rook and Knight movements.
 * The starting base line is RNBQKCNBR. The king side bishop and knight
 * traded places so that the bishops of a side operate on squares of different
 * colours.
 *
 * The King moves two squares towards the rook when castling. Pawns may also
 * promote to Chancellor.
 *
 * This variant was introduced in 1887 by Benjamin R. Foster, USA.
 *
 * \note Rules: https://en.wikipedia.org/wiki/Chancellor_Chess
 *
 * \sa CapablancaBoard
 * \sa ModernBoard
 */
class LIB_EXPORT ChancellorBoard : public WesternBoard
{
	public:
		/*! Creates a new ChancellorBoard object. */
		ChancellorBoard();

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
		virtual int width() const;
		virtual int height() const;
	protected:
		enum ChancellorPieceType
		{
			Chancellor = 8 //!< Chancellor (knight + rook)
		};
		// Inherited from WesternBoard
		virtual int castlingFile(CastlingSide castlingSide) const;
		virtual void addPromotions(int sourceSquare,
					   int targetSquare,
					   QVarLengthArray< Move >& moves) const;
};

} // namespace Chess
#endif // CHANCELLORBOARD_H
