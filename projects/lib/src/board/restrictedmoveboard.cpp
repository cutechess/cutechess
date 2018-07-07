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

#include "restrictedmoveboard.h"
#include "westernzobrist.h"

namespace Chess {

RestrictedMoveBoard::RestrictedMoveBoard()
	: WesternBoard(new WesternZobrist())
{
}

bool RestrictedMoveBoard::vIsLegalMove(const Move& move)
{
	if (!restriction(move))
		return false;

	return WesternBoard::vIsLegalMove(move);
}

bool RestrictedMoveBoard::inCheck(Side side, int square) const
{
	if (!WesternBoard::inCheck(side, square))
		return false;

	if (square == 0)
		square = kingSquare(side);

	QVarLengthArray <Move> moves;
	if (sideToMove() == side)
		// needs symmetry of piece movement of both sides
		for (int type = Pawn; type <= King; type++)
		{
			generateMovesForPiece(moves, type, square);
			for (const auto m: moves)
			{
				if (captureType(m) == type
				&&  restriction(m, true))
					return true;
			}
			moves.clear();
		}
	else
	{
		generateMoves(moves);
		for (const auto m: moves)
		{
			if (m.targetSquare() == square
			&&  restriction(m))
				return true;
		}
	}
	return false;
}

} // namespace Chess
