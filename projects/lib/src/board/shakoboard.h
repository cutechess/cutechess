/*
    This file is part of Cute Chess.
    Copyright (C) 2008-2020 Cute Chess authors

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

#ifndef SHAKOBOARD_H
#define SHAKOBOARD_H

#include "westernboard.h"

namespace Chess {

/*!
 * \brief A board for Shako Chess
 *
 * Shako Chess is a variant of standard chess on a 10 x 10 board with
 * two more pieces, Cannons and Elephants. This game was invented by
 * Jean-Louis Cazaux(1997).
 *
 * Each player commands 22 pieces which are the standard set with two
 * more pieces, Cannons and Elephants.
 * The Cannon piece, when not capturing, it moves just like the Rook.
 * But to capture a piece, there must be one additional piece in its path
 * which the Cannon jumps over before landing on the piece being captured.
 * This intervening piece may belong to either player. The cannon may not
 * jump over more than one piece to make its capture, and may not jump
 * over pieces when not capturing.
 * The Elephant steps diagonally one or two squares, leaping over the
 * intermediate square if it is occupied.
 *
 * All other pieces move like in orthodox chess; also castling is as in
 * usual chess. Pawns promote on the tenth row of the board to Queen,
 * Rook, Knight, Bishop, Elephant, or Cannon, to the owning players choice.
 *
 * Initially each side has *ten* pawns on their third ranks. Most of the
 * other pieces start on their second ranks. The Cannons are placed at
 * the corners.Pawns can make an initial double step and can be captured
 * en passant.
 *
 * \note Rules: https://musketeerchess.net/games/shako/rules/rules.php
 *
 * \sa WesternBoard
 */
class LIB_EXPORT ShakoBoard : public WesternBoard
{
	public:
        /*! Creates a new ShakoBoard object. */
        ShakoBoard();

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual int height() const;
		virtual int width() const;
		virtual QString defaultFenString() const;
		virtual void generateMovesForPiece(QVarLengthArray< Move >& moves,
						   int pieceType,
						   int square) const;
    protected:
		/*! Special piece types for Shako variants. */
		enum ShakoPieceType
		{
			Cannon = King + 1, //!< Cannon (Xiangqi Cannon)
			Elephant	 //!< Elephant
		};
		/*! Movement mask for Cannon moves. */
		static const unsigned CannonMovement = 16;
		/*! Movement mask for BiSquare Diagonal Jump moves. */
		static const unsigned BiSqDiagMovement = 32;

		// Inherited from Board
		virtual void vInitialize();
		virtual bool inCheck(Side side, int square = 0) const;

        // Inherited from WesternBoard
		virtual void addPromotions(int sourceSquare,
						int targetSquare,
						QVarLengthArray< Move >& moves) const;
		virtual void generatePawnMoves(int sourceSquare,
						QVarLengthArray<Move>& moves) const;
		virtual int castlingFile(CastlingSide castlingSide) const;
	private:
		void generateCannonMoves(int sourceSquare,
						QVarLengthArray<Move>& moves) const;
		/*! Helper method for Pawn moves. Returns square offset for
		 *  the given \a step with orientation \a sign. */
		inline int pawnPushOffset(const PawnStep& ps,
						int sign) const;

		QVarLengthArray<int> m_rookOffsets;
		QVarLengthArray<int> m_biSquareDiagOffsets;
};

} // namespace Chess
#endif // SHAKOBOARD_H
