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

#include "chessgiboard.h"

namespace Chess {

ChessgiBoard::ChessgiBoard()
	: LoopBoard()
{
}

Board* ChessgiBoard::copy() const
{
	return new ChessgiBoard(*this);
}

QString ChessgiBoard::variant() const
{
	return "chessgi";
}

bool ChessgiBoard::pawnDropOkOnRank(int rank) const
{
	if (sideToMove() == Side::Black)
		rank--;
	return rank >= 0 && rank < height() - 1;
}

} // namespace Chess
