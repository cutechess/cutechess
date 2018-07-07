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

#ifndef KINGOFTHEHILLBOARD_H
#define KINGOFTHEHILLBOARD_H

#include "standardboard.h"

namespace Chess {

/*!
 * \brief A board for King of the Hill Chess
 *
 * King of the Hill chess is a variant of standard chess with an additional rule.
 * Moving the king to one of the four central squares wins.
 *
 * \note Rules: http://en.wikipedia.org/wiki/Chess_variant
 *
 * KingOfTheHillBoard uses Polyglot-compatible zobrist position keys,
 * so King of the Hill (and standard) opening books in Polyglot format can be used.
 *
 * \note Standard Chess Rules: http://www.fide.com/component/handbook/?id=124&view=article
 * \sa PolyglotBook
 */
class LIB_EXPORT KingOfTheHillBoard : public StandardBoard
{
	public:
		/*! Creates a new KingOfTheHillBoard object. */
		KingOfTheHillBoard();

		// Inherited from StandardBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual Result result();
	private:
		bool kingInCenter(Side side) const;
		const QList<int> m_centralSquares;
};

} // namespace Chess
#endif // KINGOFTHEHILLBOARD_H
