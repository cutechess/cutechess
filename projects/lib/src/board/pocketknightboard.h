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

#ifndef POCKETKNIGHTBOARD_H
#define POCKETKNIGHTBOARD_H

#include "westernboard.h"

namespace Chess {

/*!
 * \brief A board for Pocket Knight Chess
 *
 * Pocket Knight Chess is a variant of standard chess.
 * Each side has an additional Knight in their reserve that can be
 * dropped onto an empty square during the game instead of making a
 * normal move.
 *
 * This variant originates from the early 20th century and is also known
 * under the name Tombola Chess.
 *
 * \note Rules: http://www.chessvariants.com/other.dir/pocket.html
 *
 */
class LIB_EXPORT PocketKnightBoard : public WesternBoard
{
	public:
		/*! Creates a new PocketKnightBoard object. */
		PocketKnightBoard();

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QList< Piece > reservePieceTypes() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
		virtual bool variantHasDrops() const;

	protected:
		// Inherited from WesternBoard
		virtual int reserveType(int pieceType) const;
		virtual void vMakeMove(const Move& move,
				       BoardTransition* transition);
		virtual void vUndoMove(const Move& move);
		virtual void generateMovesForPiece(QVarLengthArray< Move >& moves,
						   int pieceType,
						   int square) const;
};

} // namespace Chess
#endif // POCKETKNIGHTBOARD_H
