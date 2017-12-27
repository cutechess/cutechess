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

#ifndef SHATRANJBOARD_H
#define SHATRANJBOARD_H

#include "standardboard.h"

namespace Chess {

/*!
 * \brief A board for Shatranj
 *
 * Shatranj evolved from the Indian game Chaturanga and is the immediate
 * ancestor of standard chess.
 *
 * This game spread from India to Persia (named Chatrang, Shatrang) and
 * then to Arabia as Shatranj. Shatranj rules, theory and problems (mansubat)
 * are well documented. The game had stable rules and was popular for nine
 * centuries in Arabia (about AD 650-1500). There were Arabian professional
 * master players, player grades and handicap play.
 *
 * Spanish and Italian players changed some rules of the game during the
 * 15th and 16th century resulting in the much faster modern chess.
 *
 * Focussing on the differences to standard chess:
 *
 * Instead of the Queen Shatranj has the Ferz (Counselor) which only moves
 * one square diagonally. There are no Bishops but Alfils (War Elephants),
 * which leap two squares diagonally. The kings may start either on the d-file
 * or e-file, facing each other. Shatranj has no pawn double steps and
 * no castling. A Pawn can only promote to Ferz.
 *
 * A side wins if their opponent cannot make a legal move (mate or stalemate),
 * or has no pieces left besides their "bare" King. However a bare King is
 * given one chance to get level by the next move: Two bare kings is a draw.
 *
 * \note Rules: http://en.wikipedia.org/wiki/Shatranj
 */
class LIB_EXPORT ShatranjBoard : public StandardBoard
{
	public:
		/*! Creates a new ShatranjBoard object. */
		ShatranjBoard();

		// Inherited from StandardBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
		virtual Result result();

	protected:
		/*! Special piece types for Shatranj variants. */
		enum ShatranjPieceType
		{
			//!< Ferz (leaps 1 square diag.) overrides Queen
			Ferz = Queen,
			//!< Alfil (leaps 2 squares diag.) overrides Bishop
			Alfil = Bishop
		};

		/*! Movement mask for Ferz moves. */
		static const unsigned FerzMovement = 16;
		/*! Movement mask for Alfil moves. */
		static const unsigned AlfilMovement = 32;

		// Inherited from StandardBoard
		virtual bool hasCastling() const;
		virtual bool pawnHasDoubleStep() const;
		virtual void vInitialize();
		virtual bool inCheck(Side side, int square = 0) const;
		virtual void generateMovesForPiece(QVarLengthArray<Move>& moves,
						   int pieceType,
						   int square) const;
		virtual void addPromotions(int sourceSquare,
					   int targetSquare,
					   QVarLengthArray<Move>& moves) const;

		/*!
		 * Returns true if the side to move can bare the opponent king with this move
		 * and the rules allow a counter bare king move else false.
		 */
		virtual bool canBareOpponentKing();
	private:
		int m_arwidth;
		QVarLengthArray<int> m_ferzOffsets;
		QVarLengthArray<int> m_alfilOffsets;
		int pieceCount(Side side) const;
		bool bareKing(Side side, int count = 0) const;
};

} // namespace Chess
#endif // SHATRANJBOARD_H
