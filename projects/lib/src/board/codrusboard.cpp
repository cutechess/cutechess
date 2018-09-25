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

#include "codrusboard.h"

namespace Chess {

CodrusBoard::CodrusBoard()
	: GiveawayBoard()
{
}

Board* CodrusBoard::copy() const
{
	return new CodrusBoard(*this);
}

QString CodrusBoard::variant() const
{
	return "codrus";
}

bool CodrusBoard::kingsCountAssertion(int whiteKings, int blackKings) const
{
	return whiteKings + blackKings > 0;
}

void CodrusBoard::addPromotions(int sourceSquare,
				int targetSquare,
				QVarLengthArray<Move>& moves) const
{
	moves.append(Move(sourceSquare, targetSquare, Knight));
	moves.append(Move(sourceSquare, targetSquare, Bishop));
	moves.append(Move(sourceSquare, targetSquare, Rook));
	moves.append(Move(sourceSquare, targetSquare, Queen));
}

int CodrusBoard::pieceCount(Side side, int ptype) const
{
	int count = 0;
	for (int i = 0; i < arraySize(); i++)
	{
		if ((ptype == Piece::NoPiece || ptype == pieceAt(i).type())
	        &&  (side == Side::NoSide || side == pieceAt(i).side()))
			++count;
	}
	return count;
}

bool CodrusBoard::vIsLegalMove(const Move& move)
{
	Side side(sideToMove());
	int ksq = kingSquare(side);

	// no moves when without King
	if (pieceAt(ksq) != Piece(side, King)
	&&  pieceCount(side, King) < 1)
		  return false;

	return Chess::GiveawayBoard::vIsLegalMove(move);
}

Result CodrusBoard::vResultOfStalemate() const
{
	QString str { tr("Draw by stalemate") };
	return Result(Result::Draw, Side::NoSide, str);
}

Result CodrusBoard::result()
{
	const Side side = sideToMove();
	if (pieceCount(side, King) == 0)
	{
		QString str = tr("%1 wins").arg(side.toString());
		return Result(Result::Win, side, str);
	}
	return GiveawayBoard::result();
}

} // namespace Chess
