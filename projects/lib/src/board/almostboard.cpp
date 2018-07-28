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

#include "almostboard.h"
#include "westernzobrist.h"


namespace Chess {

AlmostBoard::AlmostBoard()
	: WesternBoard(new WesternZobrist())
{
	setPieceType(Chancellor, tr("chancellor"), "C", KnightMovement | RookMovement);
}

Board* AlmostBoard::copy() const
{
	return new AlmostBoard(*this);
}

QString AlmostBoard::variant() const
{
	return "almost";
}

QString AlmostBoard::defaultFenString() const
{
	return "rnbckbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBCKBNR w KQkq - 0 1";
}

} // namespace Chess
