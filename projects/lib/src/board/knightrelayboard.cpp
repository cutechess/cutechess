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

bool KnightRelayBoard::pieceCanCapture(int pieceType) const
{
	return pieceType != Knight;
}

bool KnightRelayBoard::vIsLegalMove(const Move& move)
{
	// Knights cannot be captured
	if (captureType(move) == Knight)
		return false;

	Piece piece(pieceAt(move.sourceSquare()));

	// Pawns can promote normally, but not by knight-move relay.
	// They also cannot knight-move to the first rank.
	if (piece.type() == Pawn
	&&  move.promotion() == Piece::NoPiece)
	{
		int targetRank = chessSquare(move.targetSquare()).rank();
		if (targetRank == 0 || targetRank == height() - 1)
			return false;
	}

	return StandardBoard::vIsLegalMove(move);
}

void KnightRelayBoard::generateMovesForPiece(QVarLengthArray<Move>& moves,
                                             int pieceType,
                                             int square) const
{
	// Generate standard chess moves
	StandardBoard::generateMovesForPiece(moves, pieceType, square);
	if (pieceType == King || pieceType == Knight)
		return;

	// If the piece is not a knight or the king, but is defended by a
	// friendly knight, generate relayed knight moves.
	bool hasKnightRelayMoves = false;
	Chess::Side side(sideToMove());
	QVarLengthArray<Move> knightMoves;
	for (int offset: m_knightOffsets)
	{
		int tgSq = square + offset;
		Piece piece(pieceAt(tgSq));
		Chess::Side tgSide(piece.side());
		int tgType(piece.type());

		if (tgSide == side)
		{
			if (tgType == Knight)
				hasKnightRelayMoves = true;
		}
		else if (tgType != Knight && !piece.isWall())
			knightMoves << Chess::Move(square, tgSq);
	}
	if (hasKnightRelayMoves)
		moves.append(knightMoves.constData(), knightMoves.size());
}

} // namespace Chess
