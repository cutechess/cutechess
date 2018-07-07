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

#include "suicideboard.h"

namespace Chess {

SuicideBoard::SuicideBoard()
	: AntiBoard()
{
}

Board* SuicideBoard::copy() const
{
	return new SuicideBoard(*this);
}

QString SuicideBoard::variant() const
{
	return "suicide";
}

Result SuicideBoard::vResultOfStalemate() const
{
	int pcwhite = pieceCount(Side::White);
	int pcblack = pieceCount(Side::Black);

	if (pcwhite == pcblack)
		return Result(Result::Draw, Side::NoSide, tr("Draw"));

	Side winner = pcwhite < pcblack ? Side::White : Side::Black;
	QString str { tr("%1 wins").arg(winner.toString()) };

	return Result(Result::Win, winner, str);
}

int SuicideBoard::pieceCount(Side side, int ptype) const
{
	int count = 0;
	for (int i = 0; i < arraySize(); i++)
	{
		if ((ptype == Piece::NoPiece || ptype == pieceAt(i).type())
	        &&  (side == Side::NoSide || side == pieceAt(i).side()))
			++count;
	}
	return count;
}

} // namespace Chess
