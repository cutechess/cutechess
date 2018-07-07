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

#ifndef EXTINCTIONBOARD_H
#define EXTINCTIONBOARD_H

#include "standardboard.h"

namespace Chess {

/*!
 * \brief A board for Extinction Chess
 *
 * Extinction Chess (Survival of the Species by R. W. Schmittberger, USA,
 * 1985) is a variant of standard chess but has a different objective.
 *
 * A side wins if their opponent loses all pieces of any kind.
 *
 * The king has no royal obligation, so there is no check and the king
 * can be captured. Pawns can also promote to king. Castling is allowed.
 *
 * \note Rules: https://en.wikipedia.org/wiki/Extinction_chess
 *
 * ExtinctionBoard uses Polyglot-compatible zobrist position keys,
 * so adequate opening books in Polyglot format can be used.
 *
 * \note Standard Chess Rules: http://www.fide.com/component/handbook/?id=124&view=article
 * \sa PolyglotBook
 */
class LIB_EXPORT ExtinctionBoard : public StandardBoard
{
	public:
		/*! Creates a new ExtinctionBoard object.
		 * The \a set defines piece types that must not become extinct.
		 * If \a allPromotions is set (default) then all promotion
		 * types of standard chess are allowed (king anyhow).
		 */
		ExtinctionBoard(const QSet<int>& set
				= QSet<int>{King, Queen, Rook, Bishop, Knight, Pawn},
				bool allPromotions = true);

		// Inherited from StandardBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
		virtual Result result();
	protected:
		// Inherited from StandardBoard
		virtual bool kingsCountAssertion(int whiteKings,
						 int blackKings) const;
		virtual bool inCheck(Side side, int square = 0) const;
		virtual void addPromotions(int sourceSquare,
					   int targetSquare,
					   QVarLengthArray<Move>& moves) const;

	private:
		bool m_allPromotions;
		QVarLengthArray<int> m_pieceSet;
		Piece extinctPiece(Side side) const;
};


/*!
 * \brief A board for Kinglet Chess
 *
 * Kinglet (V. R. Parton, UK 1953) is a standard chess variant similar to
 * Extinction Chess. There are two differences:
 *
 * A side wins if their opponent loses all pawns (by capture or promotion).
 * Pawns can only promote to king.
 *
 * The king has no royal obligation, so there is no check and the king can
 * be captured. Castling is allowed. Stalemate is a draw.
 *
 * \note Rules: https://en.wikipedia.org/wiki/V._R._Parton#Kinglet_Chess
 * \sa Extinction Chess
 */
class LIB_EXPORT KingletBoard : public ExtinctionBoard
{
	public:
		KingletBoard();
		virtual Board *copy() const;
		virtual QString variant() const;
};

} // namespace Chess
#endif // EXTINCTIONBOARD_H
