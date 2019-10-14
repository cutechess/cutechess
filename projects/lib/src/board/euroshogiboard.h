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

#ifndef EUROSHOGIBOARD_H
#define EUROSHOGIBOARD_H

#include "shogiboard.h"

namespace Chess {

/*!
 * \brief A board for EuroShogi
 *
 * EuroShogi is a variant of Shogi which is played on a 8x8 board.
 * In this variant either side begins with King, two (Gold) Generals,
 * two Bishops (on squares of different colours), two Knights, one Rook
 * (on 2nd rank), and eight Pawns on the third rank. Shogi rules apply.
 *
 * A Knight leaps like a chess Knight, but only in the two forward directions.
 * In contrast to Shogi, a Knight can also move one square sideways.
 * Therefore Knights can be dropped on any empty square of the board.
 *
 * The promotion zone consists of the three furthest ranks.
 * In case of a position occurring for the third time with the same player
 * to move and the same pieces in hands, the game is drawn. However,
 * if this situation arises from a series of checks, the side giving
 * perpetual check loses the game.
 *
 * EuroShogi was introduced in 2000 by Vladimír Pribylinec.
 *
 * \note Rules: http://en.wikipedia.org/wiki/Euroshogi
 *
 * \sa ShogiBoard
 */
class LIB_EXPORT EuroShogiBoard : public ShogiBoard
{
	public:
		/*! Creates a new EuroShogiBoard object. */
		EuroShogiBoard();

		// Inherited from ShogiBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual int height() const;
		virtual int width() const;
		virtual QString defaultFenString() const;
		virtual Result result();

	protected:
		// Inherited from ShogiBoard
		virtual int promotionRank() const;
		virtual void vInitialize();
		virtual void generateMovesForPiece(QVarLengthArray< Chess::Move >& moves,
					   int pieceType,
					   int square) const;
		virtual bool ranksAreAllowed() const;

	private:
		QVarLengthArray<int> m_knightOffsets[2];

};

} // namespace Chess
#endif // EUROSHOGIBOARD_H
