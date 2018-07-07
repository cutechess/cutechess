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

#ifndef COURIERBOARD_H
#define COURIERBOARD_H

#include "shatranjboard.h"

namespace Chess {

/*!
 * \brief A board for Courier Chess
 *
 * Courier Chess is a 12x8 board variant that has been popular in medieval
 * Europe, especially in the countries of the Holy Roman Empire (esp. Germany,
 * Netherlands) from the 12th to the 18th century alongside Shatranj and
 * then Chess (influencing the transformation from the former to the latter).
 *
 * The Queen and the Bishop in Courier Chess have the old Shatranj movements
 * of the Ferz (Counselor, one square diagonally) and the Alfil (War Elephant,
 * leaps two squares diagonally). King, Rook, and Knight have normal moves.
 * There are additional pieces: Every side has one Mann who moves like a King,
 * and one Schleich with Wazir movement (one square forward, backward, left
 * or right). Every side has two Currier or Courier pieces with modern Bishop
 * movement. Courier Chess has no Pawn double step option and no castling. A
 * Pawn can only promote to Queen.
 *
 * The effective starting position has the white Queen and Rook Pawns on
 * ranks 4 (introducing ceremonial double steps from rank 2) and the Queen
 * moves to g4 from g2. The black pieces do mirrorwise.
 *
 * A side wins if their opponent cannot make a legal move (mate or stalemate),
 * or has no pieces left besides their "bare" King. However, a bare King is
 * given one chance to get level by the next move: Two bare kings is a draw.
 *
 * \sa ShatranjBoard
 *
 * \note Rules: http://en.wikipedia.org/wiki/Courier_chess
 */
class LIB_EXPORT CourierBoard : public ShatranjBoard
{
	public:
		/*! Creates a new CourierBoard object. */
		CourierBoard();

		// Inherited from ShatranjBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual int width() const;
		virtual QString defaultFenString() const;

	protected:
		/*! Special piece types for Courier variants. */
		enum CourierPieceType
		{
			//!< Currier, Courier (diagonal slider) is modern Bishop
			Courier = King + 1,
			//!< Mann moves like King
			Mann,
			//!< Schleich moves like Wazir (1 square orthog.)
			Schleich
		};

		/*! Movement mask for Wazir moves. (Schleich) */
		static const unsigned WazirMovement = 64;

		// Inherited from ShatranjBoard
		virtual void vInitialize();
		virtual bool inCheck(Side side, int square = 0) const;
		virtual void generateMovesForPiece(QVarLengthArray<Move>& moves,
						   int pieceType,
						   int square) const;
	private:
		QVarLengthArray<int> m_wazirOffsets;
};

} // namespace Chess
#endif // COURIERBOARD_H
