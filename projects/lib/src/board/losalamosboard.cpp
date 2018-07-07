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

#include "losalamosboard.h"
#include "westernzobrist.h"

namespace Chess {

LosAlamosBoard::LosAlamosBoard()
	: WesternBoard(new WesternZobrist())
{
	setPieceType(Bishop, tr("no-bishop"), "", 0);
}

Board* LosAlamosBoard::copy() const
{
	return new LosAlamosBoard(*this);
}

QString LosAlamosBoard::variant() const
{
	return "losalamos";
}

int LosAlamosBoard::height() const
{
	return 6;
}

int LosAlamosBoard::width() const
{
	return 6;
}

QString LosAlamosBoard::defaultFenString() const
{
	return "rnqknr/pppppp/6/6/PPPPPP/RNQKNR w - - 0 1";
}

bool LosAlamosBoard::hasCastling() const
{
	return false;
}

bool LosAlamosBoard::pawnHasDoubleStep() const
{
	return false;
}

void LosAlamosBoard::addPromotions(int sourceSquare,
				int targetSquare,
				QVarLengthArray<Move>& moves) const
{
	moves.append(Move(sourceSquare, targetSquare, Knight));
	moves.append(Move(sourceSquare, targetSquare, Rook));
	moves.append(Move(sourceSquare, targetSquare, Queen));
}

} // namespace Chess
