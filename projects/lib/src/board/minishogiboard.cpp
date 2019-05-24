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

#include "minishogiboard.h"

namespace Chess {

MiniShogiBoard::MiniShogiBoard()
	: ShogiBoard()
{
	setPieceType(Lance, tr("no-lance"), "", 0);
	setPieceType(Knight, tr("no-knight"), "", 0);
	setPieceType(PromotedLance, tr("no-promoted-lance"), "", 0);
	setPieceType(PromotedKnight, tr("no-promoted-knight"), "", 0);
}

Board* MiniShogiBoard::copy() const
{
	return new MiniShogiBoard(*this);
}

QString MiniShogiBoard::variant() const
{
	return "minishogi";
}

int MiniShogiBoard::height() const
{
	return 5;
}

int MiniShogiBoard::width() const
{
	return 5;
}

QString MiniShogiBoard::defaultFenString() const
{
	return "rbsgk/4p/5/P4/KGSBR[-] w - 1";
}

Result MiniShogiBoard::result()
{
	Side side = sideToMove();
	QString str;

	// 4-fold repetition
	if (repeatCount() >= 3)
	{
		if (inCheck(side))
		{
			  str = tr("Illegal perpetual");
			  return Result(Result::Win, side, str);
		}
		str = tr("Fourfold repetition - Gote wins");
		return Result(Result::Win, Side::Black, str);
	}
	return ShogiBoard::result();
}

} // namespace Chess
