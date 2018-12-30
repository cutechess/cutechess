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

#include "gustavboard.h"
#include "westernzobrist.h"


namespace Chess {

GustavBoard::GustavBoard()
	: WesternBoard(new WesternZobrist())
{
	setPieceType(Adjutant, tr("adjutant-general"), "A",
		     KnightMovement | BishopMovement | RookMovement, "Q~");
}

Board* GustavBoard::copy() const
{
	return new GustavBoard(*this);
}

QString GustavBoard::variant() const
{
	return "gustav3";
}

QString GustavBoard::defaultFenString() const
{
	return "arnbqkbnra/*pppppppp*/*8*/*8*/*8*/*8*/*PPPPPPPP*/ARNBQKBNRA w KQkq - 0 1";
}

int GustavBoard::width() const
{
	return 10;
}

int GustavBoard::height() const
{
	return 8;
}

bool GustavBoard::variantHasWallSquares() const
{
	return true;
}

int GustavBoard::castlingFile(WesternBoard::CastlingSide castlingSide) const
{
	Q_ASSERT(castlingSide != NoCastlingSide);
	// QueenSide denotes lower file side, towards a-rook
	return castlingSide == QueenSide ? 3 : 7; // d-file and h-file
}

void GustavBoard::addPromotions(int sourceSquare, int targetSquare, QVarLengthArray< Move >& moves) const
{
	WesternBoard::addPromotions(sourceSquare, targetSquare, moves);
	moves.append(Move(sourceSquare, targetSquare, Adjutant));
}

} // namespace Chess
