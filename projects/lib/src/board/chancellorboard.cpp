/*
    This file is part of Cute Chess.

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

#include "chancellorboard.h"


namespace Chess {

ChancellorBoard::ChancellorBoard()
	: CapablancaBoard()
{
}

Board* ChancellorBoard::copy() const
{
	return new ChancellorBoard(*this);
}

QString ChancellorBoard::variant() const
{
	return "chancellor";
}

QString ChancellorBoard::defaultFenString() const
{
	return "rnbqkcnbr/ppppppppp/9/9/9/9/9/PPPPPPPPP/RNBQKCNBR w KQkq - 0 1";
}

int ChancellorBoard::width() const
{
	return 9;
}

int ChancellorBoard::height() const
{
	return 9;
}

int ChancellorBoard::castlingFile(WesternBoard::CastlingSide castlingSide) const
{
	Q_ASSERT(castlingSide != NoCastlingSide);
	// QueenSide denotes lower file side, towards a-rook
	return castlingSide == QueenSide ? 2 : 6; // c-file and g-file
}

} // namespace Chess
