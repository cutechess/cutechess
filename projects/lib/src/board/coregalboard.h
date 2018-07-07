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

#ifndef COREGALBOARD_H
#define COREGALBOARD_H

#include "westernboard.h"

namespace Chess {

/*!
 * \brief A board for Co-regal Chess
 *
 * Co-regal Chess follows the rules of standard chess, but
 * the Queen like the King is subject to check and mate.
 *
 * Introduced by Vernon R. Parton, UK, 1970.
 *
 * \note This implements the original variant without Queen castling.
 *
 * \note Rules: https://en.wikipedia.org/wiki/V._R._Parton#Co-Regal_Chess
 */


class LIB_EXPORT CoRegalBoard : public WesternBoard
{
	public:
		/*! Creates a new CoregalBoard object. */
		CoRegalBoard(const QSet<int>& setOfRoyalTypes
			     = QSet<int>{King, Queen});

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;

	protected:
		virtual bool inCheck(Side side, int square = 0) const;

	private:
		const QSet<int> m_royalPieceTypes;

};

} // namespace Chess
#endif // COREGALBOARD_H
