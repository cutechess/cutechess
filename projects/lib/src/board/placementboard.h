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

#ifndef PLACEMENTBOARD_H
#define PLACEMENTBOARD_H

#include "westernboard.h"

namespace Chess {

/*!
 * \brief A board for Placement Chess, a Shuffle-Chess variant
 * (a.k.a. Pre-Chess, Meta-Chess, Bronstein Chess, Benko Chess)
 *
 * This game was proposed by several authors like David Bronstein,
 * USSR ~1948, and Pal Benkö, USA 1978.
 *
 * Placement Chess follows the rules of standard chess but each side begins
 * only with their Pawns on their second rank. Then the players take turns
 * to place their pieces freely on the base rank. There is one restriction:
 * any side must place their Bishops on squares with different colours.
 *
 * After all pieces have been placed White makes the first move.
 * There are 2880*2880 opening positions.
 *
 * Castling rights are established only for Kings on the e-file and for
 * Rooks on corner squares.
 *
 * \note: Set-up phase with alternating white and black piece drops.
 *
 * \sa StandardBoard
 * \sa FrcBoard
 *
 */
class LIB_EXPORT PlacementBoard : public WesternBoard
{
	public:
		/*! Creates a new PlacementBoard object. */
		PlacementBoard();

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
		virtual bool variantHasDrops() const;
		virtual Result result();

	protected:
		virtual void setCastlingRights();

		// Inherited from WesternBoard
		virtual QList< Piece > reservePieceTypes() const;
		virtual bool kingsCountAssertion(int whiteKings, int blackKings) const;
		virtual void generateMovesForPiece(QVarLengthArray<Move>& moves,
						   int pieceType,
						   int square) const;
		virtual bool vSetFenString(const QStringList& fen);
		virtual void vMakeMove(const Move& move,
				       BoardTransition* transition);
		virtual void vUndoMove(const Move& move);
		virtual bool isLegalPosition();

	private:
		bool m_inSetUp;
		bool m_previouslyInSetUp;
		bool inSetup() const;
};

} // namespace Chess
#endif // PLACEMENTBOARD_H
