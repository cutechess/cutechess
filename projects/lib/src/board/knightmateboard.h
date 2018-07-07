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

#ifndef KNIGHTMATEBOARD_H
#define KNIGHTMATEBOARD_H
#include "westernboard.h"

namespace Chess {

/*!
 * \brief A board for Knight Mate Chess
 *
 * Knight Mate chess is a variant of standard chess but with king and knight
 * movements interchanged. Introduced by Bruce Zymov, 1972.
 *
 * \note Rules: http://en.wikipedia.org/wiki/Chess_variant
 *
 * \note Standard Chess Rules: http://www.fide.com/component/handbook/?id=124&view=article
 */
class LIB_EXPORT KnightMateBoard : public WesternBoard
{
	public:
		/*! Creates a new KnightMateBoard object. */
		KnightMateBoard();

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;

	protected:
		/*! Special piece types for Knightmate */
		enum KnightMatePieceType
		{
			Mann = King + 1	//!< Mann (Commoner, moves like a chess king)
		};
		// Inherited from WesternBoard
		void virtual generateMovesForPiece(QVarLengthArray<Move>& moves,
						   int pieceType,
						   int square) const;
		virtual bool inCheck(Side side, int square = 0) const;
		virtual void addPromotions(int sourceSquare,
					   int targetSquare,
					   QVarLengthArray<Move>& moves) const;
		virtual Move moveFromSanString(const QString& str);
};

} // namespace Chess
#endif // KNIGHTMATEBOARD_H
