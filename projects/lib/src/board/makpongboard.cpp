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

#include "makpongboard.h"

namespace Chess {

MakpongBoard::MakpongBoard()
	: MakrukBoard()
{
}

Board * MakpongBoard::copy() const
{
	return new MakpongBoard(*this);
}

QString MakpongBoard::variant() const
{
	return "makpong";
}

bool MakpongBoard::insufficientMaterial() const
{
	for (int i = 0; i < arraySize(); i++)
	{
		const Piece& piece = pieceAt(i);
		if (piece.isValid() && piece.type() != King)
			return false;
	}
	return true;
}

void MakpongBoard::generateMovesForPiece(QVarLengthArray<Chess::Move>& moves,
					 int pieceType,
					 int square) const
{
	// King cannot move out of check
	Side side = sideToMove();
	if (pieceType != King || square != kingSquare(side) || !inCheck(side))
	{
		MakrukBoard::generateMovesForPiece(moves, pieceType, square);
		return;
	}

	// Find pieces attacking the King
	int attacker = 0;
	Side opp = side.opposite();
	QVarLengthArray<Move> moves1;

	int minIndex = 2 * (width() + 2) + 1;
	int maxIndex = arraySize() - 1 - minIndex;
	for (int i = minIndex; i <= maxIndex; i++)
	{
		Piece piece = pieceAt(i);
		if (piece.side() != opp)
			continue;

		moves1.clear();
		MakrukBoard::generateMovesForPiece(moves1, piece.type(), square);
		for (const Move& m: moves1)
		{
			// Test assumes point symmetry of movement between sides
			if (m.targetSquare() == i)
			{
				// No moves if in double check
				if (attacker != 0)
					return;
				attacker = i;
			}
		}
	}

	// When in check only pieces giving check can be captured by the King
	moves1.clear();
	MakrukBoard::generateMovesForPiece(moves1, King, square);
	for (const Move move: moves1)
	{
		int index = move.targetSquare();
		if (attacker == index)
			moves.append(move);
	}
}

} // namespace Chess
