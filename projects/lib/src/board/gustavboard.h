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

#ifndef GUSTAVBOARD_H
#define GUSTAVBOARD_H

#include "westernboard.h"

namespace Chess {

/*!
 * \brief A board for Gustav III's Chess
 *
 * This variant of chess on a 10x8 board was attributed to King Gustav III
 * of Sweden. In addition to the standard chess arrangement each side has
 * two additional Adjutant-Generals (Amazon, Q+N). They start from extra
 * corner squares outside of the Rooks. The outermost files of the board only
 * consist of these corner squares, so there are 64 plus 4 squares in total.
 *
 * Standard chess rules apply. A Pawn can promote to Adjutant-General.
 *
 * \note Rules: http://mlwi.magix.net/bg/gustaviii.htm
 *
 * \sa AmazonBoard
 */
class LIB_EXPORT GustavBoard : public WesternBoard
{
	public:
		/*! Creates a new GustavBoard object. */
		GustavBoard();

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
		virtual int width() const;
		virtual int height() const;
		virtual bool variantHasWallSquares() const;
	protected:
		/*! Piece types for Gustav Chess */
		enum GustavPieceType
		{
			Adjutant = 7 //!< Adjutant-General (Queen + Knight)
		};
		// Inherited from WesternBoard
		virtual int castlingFile(CastlingSide castlingSide) const;
		virtual void addPromotions(int sourceSquare,
					   int targetSquare,
					   QVarLengthArray< Move >& moves) const;
};

} // namespace Chess
#endif // GUSTAVBOARD_H
