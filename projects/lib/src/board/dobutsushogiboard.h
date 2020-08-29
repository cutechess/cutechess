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

#ifndef DOBUTSUSHOGIBOARD_H
#define DOBUTSUSHOGIBOARD_H

#include "shogiboard.h"

namespace Chess {

/*!
 * \brief A board for Dōbutsu shōgi (どうぶつしょうぎ, Animal Chess)
 *
 * Dōbutsu shōgi is a simplified variant of shōgi for young children which
 * is played on a board with 3 files and 4 ranks. Many shōgi rules apply in
 * this variant. The pieces move like in standard shōgi but moves are limited
 * to the range of one square. There are captures by displacement but no
 * concepts of check nor mate. Captured pieces switch sides and can be dropped
 * onto empty squares.
 *
 * Either side begins with one Lion (King) in the center of their base rank,
 * one Giraffe (Rook) to its right, one Elephant (Bishop) to its left,
 * and one Chick (Pawn) in front if it.
 *
 * The Chick promotes to a Hen (Tokin) after moving forward to highest rank.
 * The Hen moves like a Gold General in shōgi. A captured Hen can only be
 * dropped as a Chick but without further restrictions.
 *
 * The game is drawn if the players play the same position three turns in a
 * row. Perpetual checks are allowed.
 *
 * Capturing the opponent's Lion wins. The Lion reaching the opponent's base
 * rank wins if the Lion cannot be captured immediately.
 *
 * Dōbutsu shōgi was introduced in 2009 by Madoka Kitao, Japan
 *
 * \note Rules: http://en.wikipedia.org/wiki/Dōbutsu_shōgi
 *
 * \sa ShogiBoard
 */
class LIB_EXPORT DobutsuShogiBoard : public ShogiBoard
{
	public:
		/*! Creates a new DobutsuShogiBoard object. */
		DobutsuShogiBoard();

		// Inherited from ShogiBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual int height() const;
		virtual int width() const;
		virtual QString defaultFenString() const;
		virtual void generateMovesForPiece(QVarLengthArray<Move> & moves,
						   int pieceType,
						   int square) const;
		virtual Result result();

	protected:
		virtual int promotedPieceType(int type) const;
		virtual bool isLegalPosition();


};

} // namespace Chess
#endif // DOBUTSUSHOGIBOARD_H
