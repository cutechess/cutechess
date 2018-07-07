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

#include "coregalboard.h"
#include "westernzobrist.h"

namespace Chess {

CoRegalBoard::CoRegalBoard(const QSet< int >& setOfRoyalTypes)
	: WesternBoard(new WesternZobrist()),
	  m_royalPieceTypes(setOfRoyalTypes)
{
}

Board* CoRegalBoard::copy() const
{
	return new CoRegalBoard(*this);
}

QString CoRegalBoard::variant() const
{
	return "coregal";
}

QString CoRegalBoard::defaultFenString() const
{
	return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}

bool CoRegalBoard::inCheck(Side side, int square) const
{
	if (square != 0)
		return WesternBoard::inCheck(side, square);

	Piece piece;

	for (int i = 0; i < arraySize(); i++)
	{
		piece = pieceAt(i);
		if (piece.side() == side
		&&  m_royalPieceTypes.contains(piece.type())
		&&  WesternBoard::inCheck(side, i))
			return true;
	}
	return false;
}

} // namespace Chess
