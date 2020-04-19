/*
	This file is part of Cute Chess.
	Copyright (C) 2008-2020 Cute Chess authors

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

#include "knightrelayboard.h"

namespace Chess {

KnightRelayBoard::KnightRelayBoard()
	: StandardBoard()
{
}

Board* KnightRelayBoard::copy() const
{
	return new KnightRelayBoard(*this);
}

QString KnightRelayBoard::variant() const
{
	return "knightrelay";
}

bool KnightRelayBoard::hasEnPassantCaptures() const
{
	return false;
}

bool KnightRelayBoard::pieceHasCaptureMovement(Piece piece, int square, unsigned movement) const
{
	if (piece.type() == Knight)
		return false;

	return pieceHasMovement(piece.type(), square, movement);
}

bool KnightRelayBoard::pieceHasMovement(Piece piece, int square, unsigned movement) const
{
	if (movement == KnightMovement && defendedByKnight(piece.side(), square))
		return true;

	return StandardBoard::pieceHasMovement(piece, square, movement);
}

bool KnightRelayBoard::vIsLegalMove(const Move& move)
{
	if (captureType(move) == Knight)
		return false;

	// Pawns can promote normally, but not by knight-move relay.
	if (pieceAt(move.sourceSquare()).type() == Pawn
	&&  move.promotion() == Piece::NoPiece)
	{
		int targetRank = chessSquare(move.targetSquare()).rank();
		if (targetRank == 0 || targetRank == height() - 1)
			return false;
	}

	return StandardBoard::vIsLegalMove(move);
}

} // namespace Chess
