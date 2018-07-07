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

#include "threekingsboard.h"
#include "westernzobrist.h"

namespace Chess {

ThreeKingsBoard::ThreeKingsBoard()
	: WesternBoard(new WesternZobrist())
{
}

Board* ThreeKingsBoard::copy() const
{
	return new ThreeKingsBoard(*this);
}

QString ThreeKingsBoard::variant() const
{
	return "threekings";
}

QString ThreeKingsBoard::defaultFenString() const
{
	return "knbqkbnk/pppppppp/8/8/8/8/PPPPPPPP/KNBQKBNK w - - 0 1";
}

bool ThreeKingsBoard::kingsCountAssertion(int whiteKings, int blackKings) const
{
	return whiteKings == 3 && blackKings == 3;
}

bool ThreeKingsBoard::inCheck(Side, int) const
{
	return false;
}

Result ThreeKingsBoard::result()
{
	if (kingCount(Side::White) > kingCount(Side::Black))
		return Result(Result::Win, Side::White,
			      tr("White wins"));
	if (kingCount(Side::Black) > kingCount(Side::White))
		return Result(Result::Win, Side::Black,
			      tr("Black wins"));
	return WesternBoard::result();
}

/*! Returns number of kings of \a side */
int ThreeKingsBoard::kingCount(Side side) const
{
	// Find the kings
	int kingCount = 0;
	for (int sq = 0; sq < arraySize(); sq++)
	{
		Piece tmp = pieceAt(sq);
		if (tmp.type() == King && tmp.side() == side)
			kingCount++;
	}
	return kingCount;
}

} // namespace Chess
