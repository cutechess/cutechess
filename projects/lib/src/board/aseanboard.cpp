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

#include "aseanboard.h"

namespace Chess {

AseanBoard::AseanBoard()
	: MakrukBoard()
{
	// all pieces have standard chess names but move like Makruk pieces
	setPieceType(Pawn, tr("pawn"), "P");
	setPieceType(Knight, tr("knight"), "N", KnightMovement);
	setPieceType(Bishop, tr("bishop"), "B", SilverGeneralMovement); //! Khon
	setPieceType(Rook, tr("rook"), "R", RookMovement);
	setPieceType(Queen, tr("queen"), "Q", FerzMovement); //! Ferz
	setPieceType(King, tr("king"), "K");
}

Board* AseanBoard::copy() const
{
	return new AseanBoard(*this);
}

QString AseanBoard::variant() const
{
	return "asean";
}

QString AseanBoard::defaultFenString() const
{
	return "rnbqkbnr/8/pppppppp/8/8/PPPPPPPP/8/RNBQKBNR w - - 0 1";
}

QString AseanBoard::vFenString(Board::FenNotation notation) const
{
	return WesternBoard::vFenString(notation);
}

bool AseanBoard::vSetFenString(const QStringList& fen)
{
	if (!WesternBoard::vSetFenString(fen))
		return false;

	initHistory();
	setAllPieceCounters();
	return true;
}

int AseanBoard::promotionRank(int) const
{
	return 7; // eighth rank (base rank is 0)
}

void AseanBoard::addPromotions(int sourceSquare, int targetSquare, QVarLengthArray< Move >& moves) const
{
	WesternBoard::addPromotions(sourceSquare, targetSquare, moves);
}

MakrukBoard::CountingRules AseanBoard::countingRules() const
{
	return BareKing;
}

int AseanBoard::countingLimit() const
{
	// ASEAN-Chess Article 5.2e
	Side side = sideToMove();
	int rooks = pieceCount(side, Rook);
	if (rooks > 0)
		return 16;

	int bishops = pieceCount(side, Bishop);
	int queens = pieceCount(side,Queen);
	if (bishops > 0 && queens > 0)
		return 44;

	int knights = pieceCount(side, Knight);
	if (knights > 0 && queens > 0)
		return 64;

	return 1000; // intentional limit
}

Result AseanBoard::result()
{
	// Use standard chess result
	Result gameResult = WesternBoard::result();
	if (!gameResult.isNone())
	{
		// In ASEAN-Chess a three-fold repetition is not a draw
		if (!gameResult.isDraw() || repeatCount() < 2)
			return gameResult;
	}
	// Insufficent material: KvK, KQvK. KQQvK, KNvK
	if (insufficientMaterial())
	{
		QString str = tr("Draw by insufficient mating material");
		return Result(Result::Draw, Side::NoSide, str);
	}

	// apply counting rules
	return resultFromCounting();
}

} // namespace Chess
