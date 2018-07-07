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

#ifndef BEROLINABOARD_H
#define BEROLINABOARD_H

#include "standardboard.h"

namespace Chess {

/*!
 * \brief A board for Berolina Chess
 *
 * Berolina chess is a variant of standard chess with modified pawn moves.
 * Pawns move one square diagonally forward and they capture one square straight ahead.
 * Introduced by Edmund Nebermann, Berlin 1926.
 *
 * \note Rules: http://en.wikipedia.org/wiki/Berolina_chess
 *
 * BerolinaBoard uses Polyglot-compatible zobrist position keys,
 * so Berolina opening books in Polyglot format could be used.
 *
 * \note Standard Chess Rules: http://www.fide.com/component/handbook/?id=124&view=article
 * \sa PolyglotBook
 */
class LIB_EXPORT BerolinaBoard : public StandardBoard
{
	public:
		/*! Creates a new BerolinaBoard object. */
		BerolinaBoard();

		// Inherited from StandardBoard
		virtual Board* copy() const;
		virtual QString variant() const;
};

} // namespace Chess
#endif // BEROLINABOARD_H
