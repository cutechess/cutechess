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

#ifndef CAPARANDOMBOARD_H
#define CAPARANDOMBOARD_H

#include "capablancaboard.h"

namespace Chess {

/*!
 * \brief A board for Capablanca Random chess
 *
 * Capablanca Random is like Capablanca chess, but it uses randomized
 * starting positions similarly to Fischer Random chess.
 *
 * \note Rules: http://en.wikipedia.org/wiki/Capablanca_random_chess
 * \sa CapablancaBoard
 * \sa FrcBoard
 */
class LIB_EXPORT CaparandomBoard : public CapablancaBoard
{
	public:
		/*! Creates a new CaparandomBoard object. */
		CaparandomBoard();

		// Inherited from CapablancaBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual bool isRandomVariant() const;
		/*!
		 * Returns a randomized starting FEN string.
		 *
		 * \note Mersenne::random() is used for the randomization,
		 * so Mersenne::initialize() should be called before calling
		 * this function.
		 */
		virtual QString defaultFenString() const;

	private:
		bool pawnsAreSafe(const QVector<int>& pieces) const;
};

} // namespace Chess
#endif // CAPARANDOMBOARD_H
