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

#include "pocketknightboard.h"
#include "westernzobrist.h"

namespace Chess {

PocketKnightBoard::PocketKnightBoard()
	: WesternBoard(new WesternZobrist())
{
}

Board* PocketKnightBoard::copy() const
{
	return new PocketKnightBoard(*this);
}

QList< Piece > PocketKnightBoard::reservePieceTypes() const
{
	QList<Piece> list;

	list << Piece(Side::White, Knight);
	list << Piece(Side::Black, Knight);

	return list;
}

QString PocketKnightBoard::variant() const
{
	return "pocketknight";
}

/*!
 * Most common set-up positions for Pocket Knight Chess:
 * Pocket Knight Chess with full standard pieces and extra knight in pocket
 * rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR[Nn] w KQkq - 0 1
 * Pocket Knight Chess with Queen's Knight in pocket
 * r1bqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R1BQKBNR[Nn] w KQkq - 0 1
 * Pocket Knight Chess with full standard set and two extra knights in pocket
 * rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR[NNnn] w KQkq - 0 1
 */
QString PocketKnightBoard::defaultFenString() const
{
	return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR[Nn] w KQkq - 0 1";
}

bool PocketKnightBoard::variantHasDrops() const
{
	return true;
}

int PocketKnightBoard::reserveType(int) const
{
	return Piece::NoPiece;
}

void PocketKnightBoard::vMakeMove(const Move& move, BoardTransition* transition)
{
	int source = move.sourceSquare();
	int prom = move.promotion();
	int ctype = captureType(move);

	if (ctype == Piece::NoPiece
	&&  source == 0)
		removeFromReserve(Piece(sideToMove(), prom));

	WesternBoard::vMakeMove(move, transition);
}

void PocketKnightBoard::vUndoMove(const Move& move)
{
	int source = move.sourceSquare();
	int prom = move.promotion();

	WesternBoard::vUndoMove(move);

	int ctype = captureType(move);
	if (ctype == Piece::NoPiece
	&&  source == 0)
		addToReserve(Piece(sideToMove(), prom));
}

void PocketKnightBoard::generateMovesForPiece(QVarLengthArray< Move >& moves, int pieceType, int square) const
{
	// Generate drops
	if (square == 0)
	{
		const int size = arraySize();
		for (int i = 0; i < size; i++)
		{
			Piece tmp = pieceAt(i);
			if (!tmp.isEmpty())
				continue;
			moves.append(Move(0, i, pieceType));
		}
	}
	else
		WesternBoard::generateMovesForPiece(moves, pieceType, square);
}



} // namespace Chess
