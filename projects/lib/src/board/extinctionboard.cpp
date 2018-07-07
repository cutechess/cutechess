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

#include "extinctionboard.h"

namespace Chess {

ExtinctionBoard::ExtinctionBoard(const QSet<int>& set,
				 bool allPromotions)
	: StandardBoard(),
	  m_allPromotions(allPromotions)
{
	for (const auto& s: set)
		m_pieceSet.append(s);
}

Board* ExtinctionBoard::copy() const
{
	return new ExtinctionBoard(*this);
}

QString ExtinctionBoard::variant() const
{
	return "extinction";
}

QString ExtinctionBoard::defaultFenString() const
{
	return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}

bool ExtinctionBoard::kingsCountAssertion(int, int) const
{
	return extinctPiece(Side::White).isEmpty()
	||     extinctPiece(Side::Black).isEmpty();
}

void ExtinctionBoard::addPromotions(int sourceSquare,
				int targetSquare,
				QVarLengthArray<Move>& moves) const
{
	if (m_allPromotions)
		StandardBoard::addPromotions(sourceSquare, targetSquare, moves);
	moves.append(Move(sourceSquare, targetSquare, King));
}

bool ExtinctionBoard::inCheck(Side, int) const
{
	return false;
}

Piece ExtinctionBoard::extinctPiece(Side side) const
{
	for (const int type: m_pieceSet)
	{
		bool pieceFound = false;
		for (int i = 0; i < arraySize(); i++)
		{
			Piece piece = pieceAt(i);
			if (side == piece.side() && type == piece.type())
			{
				pieceFound = true;
				break;
			}
		}
		if (!pieceFound)
			return Piece(side, type);
	}
	return Piece();
}

Result ExtinctionBoard::result()
{
	QString str;
	Side side = sideToMove();
	// captures
	Piece piece = extinctPiece(side);
	if (!piece.isValid())
		// promotions
		piece = extinctPiece(side.opposite());
	if (piece.isValid())
	{
		Side winner = piece.side().opposite();
		int type = piece.type();
		str = tr("Missing %1: %2 wins")
			.arg(pieceString(type), winner.toString());
		return Result(Result::Win, winner, str);
	}

	// stalemate
	if (!canMove())
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



KingletBoard::KingletBoard() :
	ExtinctionBoard({Pawn}, false)
{
}

Board* KingletBoard::copy() const
{
	return new KingletBoard(*this);
}

QString KingletBoard::variant() const
{
	return "kinglet";
}

} // namespace Chess
