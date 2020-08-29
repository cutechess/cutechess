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

#include "judkinsshogiboard.h"

namespace Chess {

JudkinsShogiBoard::JudkinsShogiBoard()
	: ShogiBoard()
{
	setPieceType(Lance, tr("no-lance"), "", 0);
	setPieceType(PromotedLance, tr("no-promoted-lance"), "", 0);
}

Board* JudkinsShogiBoard::copy() const
{
	return new JudkinsShogiBoard(*this);
}

QString JudkinsShogiBoard::variant() const
{
	return "judkins";
}

int JudkinsShogiBoard::height() const
{
	return 6;
}

int JudkinsShogiBoard::width() const
{
	return 6;
}

QString JudkinsShogiBoard::defaultFenString() const
{
	return "rbnsgk/5p/6/6/P5/KGSNBR[-] w - 1";
}

Result JudkinsShogiBoard::impassePointRule(int points, int) const
{
	// 12-Point rule
	Side side = sideToMove();
	Side winner;

	if (points < 12)
		winner = side.opposite();
	else if (points > 16)
		winner = side;
	else
		return Result(Result::Draw, Side::NoSide, tr("Impasse: draw"));

	QString winStr = winner == Side::White ? tr("Sente") : tr("Gote");
	QString str = tr("Impasse: %1 wins by 12-point rule").arg(winStr);
	return Result(Result::Win, winner, str);
}

} // namespace Chess
