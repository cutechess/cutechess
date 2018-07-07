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

#ifndef HORDEBOARD_H
#define HORDEBOARD_H

#include "standardboard.h"

namespace Chess {

/*!
 * \brief A board for Horde Chess and Dunsany's Chess
 * (Defaults are set to lichess.org (SF) variant)
 *
 * Horde chess is a variant of standard chess with different setup.
 * It is similar to a variant introduced by Lord Dunsany in 1942.
 *
 * The black side plays with a standard set of pieces. However white has
 * 36 pawns and no other pieces.
 *
 * White has to mate the black king, black has to capture all pawns of the
 * white horde in order to win. Standard chess rules apply. White pawns on
 * the first and second ranks can make a double step.
 *
 * \note Rules: https://en.wikipedia.org/wiki/Dunsany%27s_chess#Horde_Chess
 *
 * HordeBoard uses Polyglot-compatible zobrist position keys,
 * so Horde Chess opening books in Polyglot format can be used.
 *
 * \note Standard Chess Rules: http://www.fide.com/component/handbook/?id=124&view=article
 * \sa PolyglotBook
 */
class LIB_EXPORT HordeBoard : public StandardBoard
{
	public:
		/*! Creates a new HordeBoard object. */
		HordeBoard();

		// Inherited from StandardBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
		virtual Result result();
	protected:
		virtual bool kingsCountAssertion(int whiteKings,
						 int blackKings) const;
		virtual bool vIsLegalMove(const Move& m);
	private:
		bool hasMaterial(Side side) const;
};

} // namespace Chess
#endif // HORDEBOARD_H
