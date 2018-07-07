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

#include "hordeboard.h"

namespace Chess {

HordeBoard::HordeBoard()
	: StandardBoard()
{
}

Board* HordeBoard::copy() const
{
	return new HordeBoard(*this);
}

QString HordeBoard::variant() const
{
	return "horde";
}

/*!
 * Horde chess, lichess.org variant has 36 white pawns and starting FEN
 * rnbqkbnr/pppppppp/8/1PP2PP1/PPPPPPPP/PPPPPPPP/PPPPPPPP/PPPPPPPP w kq - 0 1
 * Dunsany's chess only has 32 white pawns and black starts with FEN
 * rnbqkbnr/pppppppp/8/8/PPPPPPPP/PPPPPPPP/PPPPPPPP/PPPPPPPP b kq - 0 1
 * Original Horde chess has 32 black pawns vs a white standard set of pieces
 * ppp2ppp/pppppppp/pppppppp/pppppppp/3pp3/8/PPPPPPPP/RNBQKBNR w KQ - 0 1
 */
QString HordeBoard::defaultFenString() const
{
	return "rnbqkbnr/pppppppp/8/1PP2PP1/PPPPPPPP/PPPPPPPP/PPPPPPPP/PPPPPPPP w kq - 0 1";
}

bool HordeBoard::vIsLegalMove(const Move& m)
{
	if (!StandardBoard::vIsLegalMove(m))
		return false;

	/*
	 * Workaround for Stockfish (lichess.org) asymmetry:
	 * accept en passant on 3rd (6th) rank only.
	 */
	int src = m.sourceSquare();
	int tgt = m.targetSquare();
	Piece piece = pieceAt(src);

	if (piece.type() != Pawn
	|| tgt != enpassantSquare())
		return true;

	int targetRank = chessSquare(tgt).rank();
	return targetRank == 2 || targetRank == height() - 3;
}

bool HordeBoard::kingsCountAssertion(int whiteKings, int blackKings) const
{
	return whiteKings + blackKings == 1;
}

Result HordeBoard::result()
{
	Side side = sideToMove();
	Side opp = side.opposite();
	if (!hasMaterial(side))
		return Result(Result::Win, opp,
			      tr("%1 wins").arg(opp.toString()));

	return StandardBoard::result();
}

/*!
 * Returns true if \a side has material else false.
 */
bool HordeBoard::hasMaterial(Side side) const
{
	for (int i = 0; i < arraySize(); i++)
	{
		if  (side == pieceAt(i).side()
		&&   pieceAt(i).isValid())
			return true;
	}
	return false;
}

} // namespace Chess
