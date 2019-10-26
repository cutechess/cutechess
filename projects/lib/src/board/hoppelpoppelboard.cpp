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

#include "hoppelpoppelboard.h"
#include "westernzobrist.h"

namespace Chess {

HoppelPoppelBoard::HoppelPoppelBoard()
	: WesternBoard(new WesternZobrist())
{
	// cross-over definitions used for checks and capturing moves
	setPieceType(Knight, "Knibis", "N", BishopMovement);
	setPieceType(Bishop, "Biskni", "B", KnightMovement);
}

Board* HoppelPoppelBoard::copy() const
{
	return new HoppelPoppelBoard(*this);
}

QString HoppelPoppelBoard::variant() const
{
	return "hoppelpoppel";
}

QString HoppelPoppelBoard::defaultFenString() const
{
	return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}

void HoppelPoppelBoard::generateMovesForPiece(QVarLengthArray< Move >& moves,
					      int pieceType,
					      int square) const
{
	if (pieceType != Knight && pieceType != Bishop)
	{
		WesternBoard::generateMovesForPiece(moves, pieceType, square);
		return;
	}

	// Knight and Bishop: sort moves obtained from cross-over definitions
	QVarLengthArray< Move > testmoves;
	WesternBoard::generateMovesForPiece(testmoves, Knight, square);
	for (const auto m: testmoves)
	{
		const bool isCapture = captureType(m) != Piece::NoPiece;
		if ((isCapture && pieceType == Knight)
		|| (!isCapture && pieceType == Bishop))
			moves.append(m);  // diagonal
	}
	testmoves.clear();
	WesternBoard::generateMovesForPiece(testmoves, Bishop, square);
	for (const auto m: testmoves)
	{
		const bool isCapture = captureType(m) != Piece::NoPiece;
		if ((isCapture && pieceType == Bishop)
		|| (!isCapture && pieceType == Knight))
			moves.append(m);  // orthodox knight leaps
	}
} //TODO: insufficient material?


NewZealandBoard::NewZealandBoard()
	: WesternBoard(new WesternZobrist())
{
	// cross-over definitions used for checks and capturing moves
	setPieceType(Knight, "Kniroo", "N", RookMovement);
	setPieceType(Rook, "Rookni", "R", KnightMovement);
}

Board* NewZealandBoard::copy() const
{
	return new NewZealandBoard(*this);
}

QString NewZealandBoard::variant() const
{
	return "newzealand";
}

QString NewZealandBoard::defaultFenString() const
{
	return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}

void NewZealandBoard::generateMovesForPiece(QVarLengthArray< Move >& moves, int pieceType, int square) const
{
	if (pieceType != Knight && pieceType != Rook)
	{
		WesternBoard::generateMovesForPiece(moves, pieceType, square);
		return;
	}

	// Knight and Rook: sort moves obtained from cross-over definitions
	QVarLengthArray< Move > testmoves;
	WesternBoard::generateMovesForPiece(testmoves, Knight, square);
	for (const auto m: testmoves)
	{
		const bool isCapture = captureType(m) != Piece::NoPiece;
		if ((isCapture && pieceType == Knight)
		|| (!isCapture && pieceType == Rook))
			moves.append(m);  // rook move: file or rank
	}
	testmoves.clear();
	WesternBoard::generateMovesForPiece(testmoves, Rook, square);
	for (const auto m: testmoves)
	{
		const bool isCapture = captureType(m) != Piece::NoPiece;
		if ((isCapture && pieceType == Rook)
		|| (!isCapture && pieceType == Knight))
			moves.append(m);  // orthodox knight leaps
	}
}

} // namespace Chess
