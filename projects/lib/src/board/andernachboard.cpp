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

#include "andernachboard.h"

namespace Chess {

AndernachBoard::AndernachBoard()
	: StandardBoard()
{
}

Board* AndernachBoard::copy() const
{
	return new AndernachBoard(*this);
}

QString AndernachBoard::variant() const
{
	return "andernach";
}

Move AndernachBoard::moveFromSanString(const QString& str)
{
	// import: ignore redundant move information in brackets: Nxd5(=bN)
	const QString istr = str.left(str.indexOf("("));
	return WesternBoard::moveFromSanString(istr);
}

/*
 * Cutechess follows convention and writes redundant information about side
 * changes to SAN: e.g. Nxd5(=bN) instead of Nxd5. Imports both formats.
 */
QString AndernachBoard::sanMoveString(const Move& move)
{
	Piece pc = pieceAt(move.sourceSquare());
	if (!switchesSides(move))
		return WesternBoard::sanMoveString(move);

	//append new piece color and type in brackets
	int promotion = move.promotion();
	int type = (promotion == Piece::NoPiece) ? pc.type() : promotion;
	Piece piece(pc.side(), type);
	QString color = (piece.side() == Side::White) ? "b" : "w";
	QString a = "(=" + color + pieceSymbol(piece).toUpper() + ")";

	return WesternBoard::sanMoveString(move) + a;
}

void AndernachBoard::vMakeMove(const Move& move,
			       BoardTransition* transition)
{
	bool moveSwitchesSides = switchesSides(move);
	StandardBoard::vMakeMove(move, transition);

	// switch side if the move is a capture (not by king)
	Piece piece = pieceAt(move.targetSquare());
	if (piece.isValid()
	&&  moveSwitchesSides)
		piece.setSide(piece.side().opposite());

	setSquare(move.targetSquare(), piece);
}

void AndernachBoard::vUndoMove(const Move& move)
{
	// switch side on target square if occupied by opposite side
	Piece piece = pieceAt(move.targetSquare());
	if (piece.side() == sideToMove().opposite())
		piece.setSide(piece.side().opposite());

	setSquare(move.targetSquare(), piece);

	StandardBoard::vUndoMove(move);
}

bool AndernachBoard::switchesSides(const Move& move) const
{
	return captureType(move) != Piece::NoPiece
	&&     pieceAt(move.sourceSquare()).type() != King;
}



AntiAndernachBoard::AntiAndernachBoard()
	: AndernachBoard()
{
}

Board* AntiAndernachBoard::copy() const
{
	return new AntiAndernachBoard(*this);
}

QString AntiAndernachBoard::variant() const
{
	return "antiandernach";
}

bool AntiAndernachBoard::switchesSides(const Move& move) const
{
	return captureType(move) == Piece::NoPiece
	&&     pieceAt(move.sourceSquare()).type() != King;
}



SuperAndernachBoard::SuperAndernachBoard()
	: AndernachBoard()
{
}

Board* SuperAndernachBoard::copy() const
{
	return new SuperAndernachBoard(*this);
}

QString SuperAndernachBoard::variant() const
{
	return "superandernach";
}

bool SuperAndernachBoard::switchesSides(const Move& move) const
{
	return pieceAt(move.sourceSquare()).type() != King;
}

} // namespace Chess
