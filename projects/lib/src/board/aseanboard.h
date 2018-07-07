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

#ifndef ASEANBOARD_H
#define ASEANBOARD_H

#include "makrukboard.h"

namespace Chess {

/*!
 * \brief A board for ASEAN-Chess
 *
 * ASEAN-Chess is a standardised variant of chess defined by the ASEAN Chess
 * Confederation. It is based both on standard chess and Makruk.
 *
 * ASEAN-Chess is played on a standard chess board. The pieces are named as in
 * standard chess, but have the movements of Makruk. So the Queen has the
 * movement of the Met (Ferz, 1 square diagonally), and the Bishop moves like
 * the Khon in Makruk (or the Silver General in Shogi): 1 square diagonally or
 * 1 square straight forward. There is no castling and pawns have no initial
 * double steps.
 *
 * The starting position has the pawns on the third own rank, like in Makruk.
 * However, the pieces on the base rank are placed in the starting order of
 * standard chess. Pawns promote on the eighth rank to either Queen, Rook,
 * Bishop, or Knight.
 *
 * Like in standard chess giving mate wins, stalemate is a draw, insufficient
 * material is a draw, and after fifty consecutive moves without any pawn move
 * and any capture a draw can be claimed. However, three-fold repetition of a
 * move does not achieve a draw. A game is drawn when no side can be checkmated
 * at all by any sequence of legal moves.
 *
 * When a King is left without any pieces the material on the board is evalua-
 * ted at once. The stronger side must give mate within 16 moves if they have
 * a Rook. If they have a Bishop and a Queen the limit is 44 moves. If they
 * have a Knight and a Queen the limit is 64 moves.
 *
 * \note Rules: http://aseanchess.org/laws-of-asean-chess/
 *
 * \sa MakrukBoard
 *
 */
class LIB_EXPORT AseanBoard : public MakrukBoard
{
	public:
		/*! Creates a new AseanBoard object. */
		AseanBoard();

		// Inherited from MakrukBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;

	protected:
		// Inherited from MakrukBoard
		virtual QString vFenString(FenNotation notation) const;
		virtual bool vSetFenString(const QStringList& fen);
		virtual int promotionRank(int file = 0) const;
		virtual void addPromotions(int sourceSquare,
					   int targetSquare,
					   QVarLengthArray< Move >& moves) const;
		virtual int countingLimit() const;
		virtual CountingRules countingRules() const;
		virtual Result result();
};

} // namespace Chess
#endif // ASEANBOARD_H
