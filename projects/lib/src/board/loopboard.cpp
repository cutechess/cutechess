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

#include "loopboard.h"

namespace Chess {

LoopBoard::LoopBoard()
	: CrazyhouseBoard()
{
}

Board* LoopBoard::copy() const
{
	return new LoopBoard(*this);
}

QString LoopBoard::variant() const
{
	return "loop";
}

QString LoopBoard::defaultFenString() const
{
	return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR[-] w KQkq - 0 1";
}

int LoopBoard::promotedPieceType(int type) const
{
	return type;
}

int LoopBoard::maxPieceSymbolLength() const
{
	// prevent reading of Crazyhouse's promoted piece types
	return 1;
}

} // namespace Chess
