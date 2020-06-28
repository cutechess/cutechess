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

#include "dobutsushogiboard.h"

namespace Chess {

DobutsuShogiBoard::DobutsuShogiBoard()
	: ShogiBoard()
{
	for (int type = Pawn; type < PromotedRook; type++)
		setPieceType(type, tr("no-piece"), "", 0);

	setPieceType(Pawn, tr("chick"), "C", 0, "P");
	setPieceType(Bishop, tr("elephant"), "E", FerzMovement, "F");
	setPieceType(Rook, tr("giraffe"), "G", WazirMovement, "R");
	setPieceType(King, tr("lion"), "L", 0, "K");

	setPieceType(PromotedPawn, tr("tokin"), "+P", GoldMovement, "W");

}

Board* DobutsuShogiBoard::copy() const
{
	return new DobutsuShogiBoard(*this);
}

QString DobutsuShogiBoard::variant() const
{
	return "dobutsu";
}

int DobutsuShogiBoard::height() const
{
	return 4;
}

int DobutsuShogiBoard::width() const
{
	return 3;
}

QString DobutsuShogiBoard::defaultFenString() const
{
	return "gle/1c1/1C1/ELG[-] w - 1";
}

int DobutsuShogiBoard::promotedPieceType(int type) const
{
	switch (type)
	{
		case Pawn:
			return PromotedPawn;
		default:
			return type;
	}
}

bool DobutsuShogiBoard::isLegalPosition()
{
	return true;
}


Result DobutsuShogiBoard::result()
{
	Side side = sideToMove();
	Side opp = side.opposite();
	QString str;

	bool foundKing = false;
	int kingSquare = 0;

	// find Lion (King)
	for (int i = 0; i < arraySize(); i++)
	{
		if (pieceAt(i) == Piece(side, King))
		{
			foundKing = true;
			break;
		}
	}

	// Lion (King) got captured
	if (!foundKing)
	{
		str = tr("Lion captured - %1 wins").arg(opp);
		return Result(Result::Win, opp, str);
	}

	// find opponent's Lion
	for (int i = 0; i < arraySize(); i++)
	{
		if (pieceAt(i) == Piece(opp, King))
		{
			kingSquare = i;
			break;
		}
	}

	// test whether opponent's Lion reached farthest rank
	int arwidth = width() + 2;
	int rank = kingSquare / arwidth - 2;
	int flag = (opp == Side::White) ? 0 : height() - 1;
	if (rank == flag && !inCheck(opp))
	{
		str = tr("Lion reached farthest rank");
		return Result(Result::Win, opp, str);
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
