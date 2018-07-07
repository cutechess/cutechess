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

#include "knightmateboard.h"
#include "westernzobrist.h"

namespace Chess {

KnightMateBoard::KnightMateBoard()
	: WesternBoard(new WesternZobrist())
{
	setPieceType(King, tr("king"), "K", 0, "RN");
	setPieceType(Mann, tr("mann"), "M", 0);
}

Board* KnightMateBoard::copy() const
{
	return new KnightMateBoard(*this);
}

QString KnightMateBoard::variant() const
{
	return "knightmate";
}

QString KnightMateBoard::defaultFenString() const
{
	return "rmbqkbmr/pppppppp/8/8/8/8/PPPPPPPP/RMBQKBMR w KQkq - 0 1";
}

void KnightMateBoard::addPromotions(int sourceSquare,
				    int targetSquare,
				    QVarLengthArray< Move >& moves) const
{
	moves.append(Move(sourceSquare, targetSquare, Mann));
	moves.append(Move(sourceSquare, targetSquare, Bishop));
	moves.append(Move(sourceSquare, targetSquare, Rook));
	moves.append(Move(sourceSquare, targetSquare, Queen));
}

void KnightMateBoard::generateMovesForPiece(QVarLengthArray<Move>& moves,
					    int pieceType,
					    int square) const
{
	if (pieceType == King || pieceType == Mann)
	{
		QVarLengthArray <Move> testmoves;
		WesternBoard::generateMovesForPiece(testmoves, King, square);
		for (const auto m: testmoves)
		{
			Square src = chessSquare(m.sourceSquare());
			Square dest = chessSquare(m.targetSquare());
			bool castling = abs(src.file() - dest.file()) > 1;

			if ((pieceType == King && castling)
			||  (pieceType == Mann && !castling))
				moves.append(m);
		}
		if (pieceType == Mann)
			return;
		// king's leaps
		return WesternBoard::generateMovesForPiece(moves, Knight, square);
	}
	return WesternBoard::generateMovesForPiece(moves, pieceType, square);
}

bool KnightMateBoard::inCheck(Side side, int square) const
{
	if (square == 0)
		square = kingSquare(side);

	QVarLengthArray <Move> moves;
	if (sideToMove() == side)
		// needs symmetry of piece movement of both sides
		for (int type = Pawn; type <= Mann; type++)
		{
			generateMovesForPiece(moves, type, square);
			for (const auto m: moves)
			{
				if (captureType(m) == type)
					return true;
			}
			moves.clear();
		}
	else
	{
		generateMoves(moves);
		for (const auto m: moves)
		{
			if (m.targetSquare() == square)
				return true;
		}
	}
	return false;
}

Move KnightMateBoard::moveFromSanString(const QString& str)
{
	QString kingSymbol(pieceSymbol(King).toUpper());
	if (!str.startsWith(kingSymbol))
		return WesternBoard::moveFromSanString(str);  //main path

	// besides O-O/O-O-O also accepts Kc1/Kc8/Kg1/Kg8 format for castling
	Side side = sideToMove();

	// Identify check/mate/strong move/blunder notation
	int ksymlen = kingSymbol.length();
	int len = str.length();
	const QString notation("+#!?");

	while (notation.contains(str.at(len - 1)))
		len--;

	// Skip king symbol and omit trailing notation parts
	int target = squareIndex(str.mid(ksymlen, len - ksymlen));
	int kingFile = chessSquare(kingSquare(side)).file();

	if (hasCastlingRight(side, QueenSide)
	&&  target == kingSquare(side) + castlingFile(QueenSide) - kingFile)
		return moveFromSanString("O-O-O");

	if (hasCastlingRight(side, KingSide)
	&&  target == kingSquare(side) + castlingFile(KingSide) - kingFile)
		return moveFromSanString("O-O");

	return WesternBoard::moveFromSanString(str); // other king moves
}

} // namespace Chess
