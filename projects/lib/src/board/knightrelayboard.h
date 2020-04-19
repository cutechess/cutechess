/*
	This file is part of Cute Chess.
	Copyright (C) 2008-2020 Cute Chess authors

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

#ifndef KNIGHTRELAYBOARD_H
#define KNIGHTRELAYBOARD_H

#include "standardboard.h"

namespace Chess {

/*!
 * \brief A board for Knight-Relay Chess
 *
 * Knight-Relay is a variant of standard chess where pieces
 * defended by a friendly knight gain the power of a knight.
 * Knights cannot capture (or check) or be captured; pawns
 * cannot en passant, or be relayed to the first or last ranks.
 *
 * \note Rules: https://en.wikipedia.org/wiki/Knight_relay_chess
 */
class LIB_EXPORT KnightRelayBoard : public StandardBoard
{
	public:
		/*! Creates a new KnightRelayBoard object. */
		KnightRelayBoard();

		// Inherited from StandardBoard
		virtual Board* copy() const;
		virtual QString variant() const;

	protected:
		// Inherited from StandardBoard
		virtual bool pieceHasMovement(Piece piece, int square, unsigned movement) const;
		virtual bool pieceHasCaptureMovement(Piece piece, int square, unsigned movement) const;
		virtual bool vIsLegalMove(const Move& move);

		// Inherited from WesternBoard
		virtual bool hasEnPassantCaptures() const;
};

} // namespace Chess
#endif // KNIGHTRELAYBOARD_H
