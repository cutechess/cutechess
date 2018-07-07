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

#include "gridboard.h"

namespace Chess {

GridBoard::GridBoard() : RestrictedMoveBoard() {}

Board* GridBoard::copy() const
{
	return new GridBoard(*this);
}

QString GridBoard::variant() const
{
	return "grid";
}

QString GridBoard::defaultFenString() const
{
	return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}

bool GridBoard::leavesRegion(const Move& move, int fs, int rs) const
{
	Square sourceSq = chessSquare(move.sourceSquare());
	Square targetSq = chessSquare(move.targetSquare());

	return ((sourceSq.file() - fs) / 2 != (targetSq.file() - fs) / 2)
	||      (sourceSq.rank() - rs) / 2 != (targetSq.rank() - rs) / 2;
}

bool GridBoard::restriction(const Move& move, bool) const
{
	return leavesRegion(move);
}



DisplacedGridBoard::DisplacedGridBoard() : GridBoard() {}

Board* DisplacedGridBoard::copy() const
{
	return new DisplacedGridBoard(*this);
}

QString DisplacedGridBoard::variant() const
{
	return "displacedgrid";
}

QString DisplacedGridBoard::defaultFenString() const
{
	return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}

bool DisplacedGridBoard::restriction(const Move& move, bool) const
{
	return leavesRegion(move, -1, -1);
}



SlippedGridBoard::SlippedGridBoard() : GridBoard() {}

Board* SlippedGridBoard::copy() const
{
	return new SlippedGridBoard(*this);
}

QString SlippedGridBoard::variant() const
{
	return "slippedgrid";
}

QString SlippedGridBoard::defaultFenString() const
{
	return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}

bool SlippedGridBoard::restriction(const Move& move, bool) const
{
	return leavesRegion(move, -1, 0);
}



BerolinaGridBoard::BerolinaGridBoard() : GridBoard()
{
 /*
  * Berolina pawns move one square diagonally forward and they capture
  * one square straight ahead.
  */
	m_pawnSteps.clear();
	m_pawnSteps += {FreeStep, -1};
	m_pawnSteps += {CaptureStep, 0};
	m_pawnSteps += {FreeStep, 1};
}

Board* BerolinaGridBoard::copy() const
{
	return new BerolinaGridBoard(*this);
}

QString BerolinaGridBoard::variant() const
{
	return "gridolina";
}

QString BerolinaGridBoard::defaultFenString() const
{
	return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}

} // namespace Chess
