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

#ifndef CRAZYHOUSEBOARD_H
#define CRAZYHOUSEBOARD_H

#include "westernboard.h"

namespace Chess {

/*!
 * \brief A board for Crazyhouse chess
 *
 * Crazyhouse is a variant of standard chess where captured
 * pieces can be brought back ("dropped") into the game,
 * similar to Shogi.
 *
 * \note Rules: http://en.wikipedia.org/wiki/Crazyhouse
 * \sa ShogiBoard
 */
class CrazyhouseBoard : public WesternBoard
{
	Q_OBJECT

	public:
		/*! Creates a new CrazyhouseBoard object. */
		CrazyhouseBoard(QObject* parent = 0);

		// Inherited from WesternBoard
		virtual QString variant() const;
		virtual bool variantHasDrops() const;
		virtual QString defaultFenString() const;

	protected:
		/*!
		 * Promoted piece types for Crazyhouse.
		 *
		 * All of these pieces where promoted from pawns, and
		 * when they're captured they get demoted back to pawns.
		 */
		enum CrazyhousePieceType
		{
			PromotedKnight = 7,	//!< Promoted Knight
			PromotedBishop,		//!< Promoted Bishop
			PromotedRook,		//!< Promoted Rook
			PromotedQueen,		//!< Promoted Queen
		};

		// Inherited from WesternBoard
		virtual int handPieceType(int pieceType) const;
		virtual QString sanMoveString(const Move& move);
		virtual Move moveFromSanString(const QString& str);
		virtual void vMakeMove(const Move& move,
				       QVarLengthArray<int>& changedSquares);
		virtual void vUndoMove(const Move& move);
		virtual void generateMovesForPiece(QVarLengthArray<Move>& moves,
						   int pieceType,
						   int square) const;

	private:
		static int normalPieceType(int type);
		static int promotedPieceType(int type);
		void normalizePieces(Piece piece, QVarLengthArray<int>& squares);
		void restorePieces(Piece piece, const QVarLengthArray<int>& squares);
};

} // namespace Chess
#endif // CRAZYHOUSEBOARD_H
