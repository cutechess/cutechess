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

#include "kingofthehillboard.h"

namespace Chess {

KingOfTheHillBoard::KingOfTheHillBoard()
	: StandardBoard(),
	  m_centralSquares { 54, 55, 64, 65 } // Squares d5, e5, d4, and e4
{
}

Board* KingOfTheHillBoard::copy() const
{
	return new KingOfTheHillBoard(*this);
}

QString KingOfTheHillBoard::variant() const
{
	return "kingofthehill";
}

Result KingOfTheHillBoard::result()
{
	if (kingInCenter(Side::White))
		return Result(Result::Win, Side::White,
			      tr("White wins with king in the center"));
	if (kingInCenter(Side::Black))
		return Result(Result::Win, Side::Black,
			      tr("Black wins with king in the center"));
	return StandardBoard::result();
}

/*! Returns true if the king of \a side is occupying a central square */
bool KingOfTheHillBoard::kingInCenter(Side side) const
{
	return m_centralSquares.contains(kingSquare(side));
}

} // namespace Chess
