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

#include "chigorinboard.h"
#include "westernzobrist.h"


namespace Chess {

ChigorinBoard::ChigorinBoard()
	: WesternBoard(new WesternZobrist())
{
	setPieceType(Chancellor, tr("chancellor"), "C", KnightMovement | RookMovement);
}

Board* ChigorinBoard::copy() const
{
	return new ChigorinBoard(*this);
}

QString ChigorinBoard::variant() const
{
	return "chigorin";
}

QString ChigorinBoard::defaultFenString() const
{
	return "rbbqkbbr/pppppppp/8/8/8/8/PPPPPPPP/RNNCKNNR w KQkq - 0 1";
}

void ChigorinBoard::addPromotions(int sourceSquare, int targetSquare, QVarLengthArray< Move >& moves) const
{
	Side side = sideToMove();

	if (side == Side::White)
	{
		moves.append(Move(sourceSquare, targetSquare, Knight));
		moves.append(Move(sourceSquare, targetSquare, Rook));
		moves.append(Move(sourceSquare, targetSquare, Chancellor));
	}
	if (side == Side::Black)
	{
		moves.append(Move(sourceSquare, targetSquare, Bishop));
		moves.append(Move(sourceSquare, targetSquare, Rook));
		moves.append(Move(sourceSquare, targetSquare, Queen));
	}
}

} // namespace Chess
