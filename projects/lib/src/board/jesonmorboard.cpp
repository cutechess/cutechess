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

#include "jesonmorboard.h"
#include "westernzobrist.h"

namespace Chess {

JesonMorBoard::JesonMorBoard()
	: WesternBoard(new WesternZobrist()),
	m_centralSquare(71) // e5
{
}

Board* JesonMorBoard::copy() const
{
	return new JesonMorBoard(*this);
}

QString JesonMorBoard::variant() const
{
	return "jesonmor";
}

int JesonMorBoard::width() const
{
	return 9;
}

int JesonMorBoard::height() const
{
	return 9;
}

QString JesonMorBoard::defaultFenString() const
{
	return "nnnnnnnnn/9/9/9/9/9/9/9/NNNNNNNNN w - - 0 1";
}

bool JesonMorBoard::kingsCountAssertion(int, int) const
{
	return true;
}

bool JesonMorBoard::inCheck(Side side, int square) const
{
	if (square == 0)
		return false;

	return WesternBoard::inCheck(side, square);
}

Result JesonMorBoard::result()
{
	QString str;
	Side side = sideToMove();
	Side opp = side.opposite();

	if (!canMove())
	{
		str = tr("%1 cannot move").arg(side.toString());
		return Result(Result::Win, opp, str);
	}

	Piece piece = pieceAt(m_centralSquare);
	Side centralSide = piece.side();

	/*
	 * The game is won if the side on the central square can move
	 * immediately or else cannot be captured by the opponent.
	 */
	if (centralSide == side
	|| (centralSide == opp && !inCheck(opp, m_centralSquare)))
	{
		str = tr("%1 wins").arg(centralSide.toString());
		return Result(Result::Win, centralSide, str);
	}

	return Result();
}

} // namespace Chess
