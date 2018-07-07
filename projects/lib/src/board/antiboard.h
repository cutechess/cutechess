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

#ifndef ANTIBOARD_H
#define ANTIBOARD_H

#include "standardboard.h"

namespace Chess {

/*!
 * \brief A board for Losing Chess, LC1 (main variant, international rules)
 * a.k.a Antichess
 *
 * Losing Chess is a variant of standard chess but has a different aim.
 *
 * The side that has no legal moves wins, either by losing all pieces or
 * by being stalemated.
 *
 * The king has no royal powers, so there is no check and the king can be
 * captured. Pawns can also promote to king. Castling is not allowed.
 * If a side can capture then they must make a capture move.
 *
 * \note Rules: https://en.wikipedia.org/wiki/Losing_chess#Rules_.28main_variant.29
 *
 * Losing Chess is believed to be several centuries old.
 * Giveaway Chess and Suicide Chess are variants of Losing Chess.
 * \sa GiveawayBoard
 * \sa SuicideBoard
 *
 * AntiBoard uses Polyglot-compatible zobrist position keys,
 * so adequate opening books in Polyglot format can be used.
 *
 * \note Standard Chess Rules: http://www.fide.com/component/handbook/?id=124&view=article
 * \sa PolyglotBook
 */
class LIB_EXPORT AntiBoard : public StandardBoard
{
	public:
		/*! Creates a new AntiBoard object. */
		AntiBoard();

		// Inherited from StandardBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
		virtual Result result();

	protected:
		// Inherited from StandardBoard
		virtual bool hasCastling() const;
		virtual bool kingsCountAssertion(int whiteKings,
						 int blackKings) const;
		virtual bool vSetFenString(const QStringList& fen);
		virtual bool inCheck(Side side, int square = 0) const;
		virtual bool vIsLegalMove(const Move& move);
		virtual void addPromotions(int sourceSquare,
					   int targetSquare,
					   QVarLengthArray<Move>& moves) const;

		/*! Rules stalemate outcome. */
		virtual Result vResultOfStalemate() const;

	private:
		quint64 m_testKey;
		bool m_canCapture;
}; // namespace Chess

}
#endif // ANTIBOARD_H
