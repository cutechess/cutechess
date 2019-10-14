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

#include "euroshogiboard.h"

namespace Chess {

EuroShogiBoard::EuroShogiBoard()
	: ShogiBoard()
{
	setPieceType(Lance, tr("no-lance"), "", 0);
	setPieceType(SilverGeneral, tr("no-silver"), "", 0);
	setPieceType(PromotedLance, tr("no-promoted-lance"), "", 0);
	setPieceType(PromotedSilver, tr("no-promoted-silver"), "", 0);
	setPieceType(Knight, tr("knight"), "N");
}

Board* EuroShogiBoard::copy() const
{
	return new EuroShogiBoard(*this);
}

QString EuroShogiBoard::variant() const
{
	return "euroshogi";
}

int EuroShogiBoard::height() const
{
	return 8;
}

int EuroShogiBoard::width() const
{
	return 8;
}

QString EuroShogiBoard::defaultFenString() const
{
	return "1nbgkgn1/1r4b1/pppppppp/8/8/PPPPPPPP/1B4R1/1NGKGBN1[-] w - 1";
}

int EuroShogiBoard::promotionRank() const
{
	return height() - 3;
}

void EuroShogiBoard::vInitialize()
{
	ShogiBoard::vInitialize();

	int arwidth = width() + 2;
	m_knightOffsets[Side::White].resize(4);
	m_knightOffsets[Side::White][0] = -2 * arwidth - 1;
	m_knightOffsets[Side::White][1] = -2 * arwidth + 1;
	m_knightOffsets[Side::White][2] = -1;
	m_knightOffsets[Side::White][3] =  1;

	m_knightOffsets[Side::Black].resize(4);
	m_knightOffsets[Side::Black][0] =  2 * arwidth + 1;
	m_knightOffsets[Side::Black][1] =  2 * arwidth - 1;
	m_knightOffsets[Side::Black][2] =  1;
	m_knightOffsets[Side::Black][3] = -1;
}

void EuroShogiBoard::generateMovesForPiece(QVarLengthArray< Chess::Move >& moves,
					   int pieceType,
					   int square) const
{
	if (pieceType != Knight)
	{
		ShogiBoard::generateMovesForPiece(moves, pieceType, square);
		return;
	}

	// Generate Knight drops
	if (square == 0)
	{
		int minIndex = 2 * (width() + 2) + 1;
		int maxIndex = arraySize() - minIndex;
		for (int i = minIndex; i < maxIndex; i++)
		{
			Piece tmp = pieceAt(i);
			if (!tmp.isEmpty())
				continue;

			moves.append(Move(0, i, pieceType));
		}
		return;
	}
	//EuroShogi Knight movement
	QVarLengthArray<Move> moves1;
	Side side = sideToMove();
	generateHoppingMoves(square, m_knightOffsets[side], moves1);

	for (Move m: moves1)
	{
		moves.append(m);

		int target = m.targetSquare();
		int rank = chessSquare(target).rank();
		int rrank = (sideToMove() == Side::White) ? rank : height() - 1 - rank;

		if (rrank >= promotionRank())
			moves.append(Move(square, m.targetSquare(), PromotedKnight));
	}
}

bool EuroShogiBoard::ranksAreAllowed() const
{
	return true;
}

Result EuroShogiBoard::result()
{
	Side side = sideToMove();
	QString str;

	// 3-fold repetition
	if (repeatCount() >= 2)
	{
		if (inCheck(side))
		{
			  str = tr("Illegal perpetual");
			  return Result(Result::Win, side, str);
		}
		str = tr("Draw by 3-fold repetition");
		return Result(Result::Draw, Side::NoSide, str);
	}
	return ShogiBoard::result();
}

} // namespace Chess
