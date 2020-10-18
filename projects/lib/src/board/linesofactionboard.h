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

#ifndef LINESOFACTIONBOARD_H
#define LINESOFACTIONBOARD_H

#include "westernboard.h"

namespace Chess {

/*!
 * \brief A board for Lines of Action (LOA)
 *
 * Lines of Action is an abstract strategy game on a 8 x 8 board.
 * It has been introduced by Claude Soucie, France in about 1960. It has
 * been described by Sid Sackson in 1969 in "Gamut of Games".
 *
 * At start both sides have 12 pieces lined up at the boundary squares of
 * the board. The corner squares are free. Black has their pieces on the
 * 1st and 8th rank of the board and starts the game. White occupies the
 * a- and the h-file and moves second. Both sides take turns moving.
 *
 * A piece can move horizontally, vertically, or diagonally. The step size
 * of a move equals the total number of pieces on the line of movement.
 * Opponent pieces can be captured but not be jumped over. Own pieces can
 * be jumped over but not be captured.
 *
 * The goal of the game is to connect all of a side's on-board pieces so
 * that they are touching horizontally, vertically, or diagonally.
 *
 * When all pieces of both sides get connected by the same move the game
 * is either drawn (Sackson's draw rule) or the side making the last move
 * wins (Soucie's rule - used here).
 *
 * \note Rules: https://en.wikipedia.org/wiki/Lines_of_Action
 *
 * \note If a side has no legal moves left this implementation adjudicates
 * a win for the opponent. When a position occurs for the third time the
 * game is drawn. When both sides finish at the same time the side making
 * the last move wins.
 *
 * \note There is a variant "Scrambled Eggs" where white and black pieces
 * are placed alternately on the outer squares.
 * FEN: 1PpPpPp1/p6P/P6p/p6P/P6p/p6P/P6p/1pPpPpP1 b - - 0 1
 * */
class LIB_EXPORT LinesOfActionBoard : public WesternBoard
{
	public:
		/*! Creates a new LinesOfActionBoard object. */
		LinesOfActionBoard();

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
		virtual Result result();

	protected:
		/*!
		 * Generates moves with a range equal to the number of pieces
		 * on the line and without jumping over opponent pieces.
		 *
		 * \param sourceSquare The source square of the piece
		 * \param offsets An array of offsets for the target square
		 * \note The generated \a moves include captures.
		 */
		void generateLineMoves(int sourceSquare,
				       const QVarLengthArray<int>& offsets,
				       QVarLengthArray<Move>& moves) const;

		// Inherited from WesternBoard
		virtual void vInitialize();
		virtual bool kingsCountAssertion(int whiteKings,
						 int blackKings) const;
		virtual bool isLegalPosition();
		virtual void generateMovesForPiece(QVarLengthArray<Move>& moves,
						   int pieceType,
						   int square) const;
		virtual QString sanMoveString(const Move& move);
		virtual Move moveFromSanString(const QString& str);

	private:
		int components(Side side) const;

		int m_minIndex;
		int m_maxIndex;
		int m_drawIfBothFinished;
		QVarLengthArray<int> m_offsets;
};

} // namespace Chess
#endif // LINESOFACTIONBOARD_H
