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

#include "modernboard.h"
#include "westernzobrist.h"

namespace Chess {

ModernBoard::ModernBoard()
	: WesternBoard(new WesternZobrist())
{
	// Use Minister "M" symbol for notation and Archbishop graphics
	setPieceType(Minister, tr("minister"), "M",
		     KnightMovement | BishopMovement, "A");
}

Board* ModernBoard::copy() const
{
	return new ModernBoard(*this);
}

QString ModernBoard::variant() const
{
	return "modern";
}

QString ModernBoard::defaultFenString() const
{
	return "rnbqkmbnr/ppppppppp/9/9/9/9/9/PPPPPPPPP/RNBMKQBNR w KQkq - 0 1";
}

int ModernBoard::width() const
{
	return 9;
}

int ModernBoard::height() const
{
	return 9;
}

int ModernBoard::castlingFile(WesternBoard::CastlingSide castlingSide) const
{
	Q_ASSERT(castlingSide != NoCastlingSide);
	// QueenSide denotes lower file side, towards a-rook
	return castlingSide == QueenSide ? 2 : 6; // c-file and g-file
}

void ModernBoard::addPromotions(int sourceSquare, int targetSquare, QVarLengthArray< Move >& moves) const
{
	WesternBoard::addPromotions(sourceSquare, targetSquare, moves);
	moves.append(Move(sourceSquare, targetSquare, Minister));
}


// Variant's SAN notation for castling moves: O-M-O (left) and O-Q-O (right).
QString ModernBoard::sanMoveString(const Move& move)
{
	QString str = WesternBoard::sanMoveString(move);

	if (!str.startsWith("O-O"))
		return str;

	QString r(str.mid(str.lastIndexOf("O") + 1));

	Side side = pieceAt(move.sourceSquare()).side();
	if (str.startsWith("O-O-O"))
		return side == Side::White ? "O-M-O" + r: "O-Q-O" + r;
	else
		return side == Side::White ? "O-Q-O" + r: "O-M-O" + r;
}

// This method accepts Modern Chess notation for castling and also standard chess notation. 
Move ModernBoard::moveFromSanString(const QString& str)
{
	bool isWhite = (sideToMove() == Side::White);
	if (str.startsWith("O-M-O"))
		return WesternBoard::moveFromSanString(isWhite ? "O-O-O":"O-O");
	if (str.startsWith("O-Q-O"))
		return WesternBoard::moveFromSanString(isWhite ? "O-O":"O-O-O");

	return WesternBoard::moveFromSanString(str);
}

} // namespace Chess
