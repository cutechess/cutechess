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

#ifndef MAKPONGBOARD_H
#define MAKPONGBOARD_H

#include "makrukboard.h"

namespace Chess {

/*!
 * \brief A board for Makpong
 *
 * Makpong is a variant of Makruk where the King may not move out of check.
 * The King may capture an attacking piece if in range, but cannot capture
 * out of double check.
 *
 * This variant is designed to reduce draws. It is used as tie breaker in
 * Makruk single elimination tournaments.
 *
 * \sa MakrukBoard
 */
class LIB_EXPORT MakpongBoard : public MakrukBoard
{
	public:
		/*! Creates a new MakpongBoard object. */
		MakpongBoard();

		// Inherited from MakrukBoard
		virtual Board* copy() const;
		virtual QString variant() const;

	protected:
		// Inherited from MakrukBoard
		virtual bool insufficientMaterial() const;
		virtual void generateMovesForPiece(QVarLengthArray<Move>& moves,
						   int pieceType,
						   int square) const;
};

} // namespace Chess
#endif // MAKPONGBOARD_H
