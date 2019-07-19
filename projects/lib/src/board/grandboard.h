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

#ifndef GRANDBOARD_H
#define GRANDBOARD_H

#include "capablancaboard.h"

namespace Chess {

/*!
 * \brief A board for Grand Chess
 *
 * Grand Chess is a variant of standard chess on a 10 x 10 board with
 * Capablanca chess pieces. This game was introduced in 1984 by Christian
 * Freeling, The Netherlands.
 *
 * Each side has one Cardinal (Archbishop, Bishop+Knight) and one Marshal
 * (Chancellor, Rook+Knight) in addition to the standard pieces.
 * GrandBoard uses the Capablanca names Archbishop and Chancellor.
 *
 * Initially each side has *ten* pawns on their third ranks. Most of the
 * other pieces start on their second ranks. The rooks are placed at
 * the corners. Castling is not allowed in Grand Chess.
 *
 * Pawns can make an initial double step and can be captured en passant.
 * When reaching their eigth or ninth ranks pawns can optionally be
 * promoted. Pawns can only promote to an own captured piece. On the tenth
 * rank a pawn *must* be promoted. If promotion is impossible, then the
 * pawn cannot advance to the tenth rank. It can give check to a king on
 * the end rank nevertheless.
 *
 * \note Rules: https://en.wikipedia.org/wiki/Grand_Chess
 *
 * \sa CapablancaBoard
 * \sa EmbassyBoard
 */
class LIB_EXPORT GrandBoard : public CapablancaBoard
{
	public:
		/*! Creates a new GrandBoard object. */
		GrandBoard();

		// Inherited from CapablancaBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual int height() const;
		virtual QString defaultFenString() const;
		virtual bool hasCastling() const;
		virtual void generateMovesForPiece(QVarLengthArray< Move >& moves,
						   int pieceType,
						   int square) const;
		virtual bool vIsLegalMove(const Move& move);
	private:
		/*! Helper method for Pawn moves. Returns square offset for
		 *  the given \a step with orientation \a sign. */
		inline int pawnMoveOffset(const PawnStep& ps,
					  int sign) const;

};

} // namespace Chess
#endif // GRANDBOARD_H
