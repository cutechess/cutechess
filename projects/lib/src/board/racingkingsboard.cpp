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

#include "racingkingsboard.h"
#include "westernzobrist.h"

namespace Chess {

RacingKingsBoard::RacingKingsBoard()
	: WesternBoard(new WesternZobrist())
{
}

Board* RacingKingsBoard::copy() const
{
	return new RacingKingsBoard(*this);
}

QString RacingKingsBoard::variant() const
{
	return "racingkings";
}

QString RacingKingsBoard::defaultFenString() const
{
	return "8/8/8/8/8/8/krbnNBRK/qrbnNBRQ w - - 0 1";
}

bool RacingKingsBoard::isLegalPosition()
{
	Side side = sideToMove();
	if (inCheck(side))
		return false;

	return WesternBoard::isLegalPosition();
}

/*! Returns true if the king of \a side is on the eighth rank */
bool RacingKingsBoard::finished(Side side) const
{
	Square ksq = chessSquare(kingSquare(side));
	return (ksq.rank() == 7);
}

/*! Returns true if the king of \a side can reach the eighth rank */
bool RacingKingsBoard::canFinish(Side side)
{
	QVarLengthArray<Move> moves;
	WesternBoard::generateMovesForPiece(moves, King, kingSquare(side));
	for (const Move& m: moves)
	{
		Square targetSq = chessSquare(m.targetSquare());
		if (targetSq.rank() == 7
		&&  vIsLegalMove(m) )
			return true;
	}
	return false;
}

Result RacingKingsBoard::result()
{
	QString str;
	bool blackFinished = finished(Side::Black);
	bool whiteFinished = finished(Side::White);

	// Finishing on eighth rank
	if (blackFinished && whiteFinished)
	{
		str = tr("Drawn race");
		return Result(Result::Draw, Side::NoSide, str);
	}

	if (blackFinished)
	{
		str = tr("Black wins the race");
		return Result(Result::Win, Side::Black, str);
	}

	Side side = sideToMove();
	bool mobile = canMove();

	// White finished but Black cannot finish or forfeited the chance
	if (whiteFinished
	&& ((mobile && !canFinish(Side::Black)) || side == Side::White))
	{
		str = tr("White wins the race");
		return Result(Result::Win, Side::White, str);
	}

	// Stalemate
	if (!mobile)
	{
		str = tr("Draw by stalemate");
		return Result(Result::Draw, Side::NoSide, str);
	}

	// 50 move rule
	if (reversibleMoveCount() >= 100)
	{
		str = tr("Draw by fifty moves rule");
		return Result(Result::Draw, Side::NoSide, str);
	}

	// 3-fold repetition
	if (repeatCount() >= 2)
	{
		str = tr("Draw by 3-fold repetition");
		return Result(Result::Draw, Side::NoSide, str);
	}

	return Result();
}

} // namespace Chess
