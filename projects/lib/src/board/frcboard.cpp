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

#include "frcboard.h"
#include "piece.h"
#include <mersenne.h>

namespace {

void addPiece(QVector<int>& pieces,
	      int piece,
	      int pos,
	      int start = 0,
	      int step = 1)
{
	int i = 0;
	for (int j = start; j < pieces.size(); j += step)
	{
		if (pieces.at(j) != Chess::Piece::NoPiece)
			continue;
		if (i == pos)
		{
			pieces[j] = piece;
			break;
		}
		i++;
	}
}

} // anonymous namespace

namespace Chess {

FrcBoard::FrcBoard()
	: StandardBoard()
{
}

Board* FrcBoard::copy() const
{
	return new FrcBoard(*this);
}

QString FrcBoard::variant() const
{
	return "fischerandom";
}

bool FrcBoard::isRandomVariant() const
{
	return true;
}

QString FrcBoard::defaultFenString() const
{
	const int empty = Piece::NoPiece;
	QVector<int> pieces(8, empty);

	addPiece(pieces, Bishop, Mersenne::random() % 4, 0, 2);
	addPiece(pieces, Bishop, Mersenne::random() % 4, 1, 2);
	addPiece(pieces, Queen, Mersenne::random() % 6);
	addPiece(pieces, Knight, Mersenne::random() % 5);
	addPiece(pieces, Knight, Mersenne::random() % 4);
	addPiece(pieces, Rook, 0);
	addPiece(pieces, King, 0);
	addPiece(pieces, Rook, 0);

	QString fen;

	// Black pieces
	for (int pieceType : qAsConst(pieces))
		fen += pieceSymbol(Piece(Side::Black, pieceType));
	fen += '/';
	// Black pawns
	for (int i = 0; i < width(); i++)
		fen += pieceSymbol(Piece(Side::Black, Pawn));
	fen += '/';

	// Empty squares
	for (int i = 0; i < height() - 4; i++)
		fen += QString::number(pieces.size()) + '/';

	// White pawns
	for (int i = 0; i < width(); i++)
		fen += pieceSymbol(Piece(Side::White, Pawn));
	fen += '/';
	// White pieces
	for (int pieceType : qAsConst(pieces))
		fen += pieceSymbol(Piece(Side::White, pieceType));

	// Side to move, castling rights, enpassant square, etc.
	fen += " w KQkq - 0 1";

	return fen;
}

} // namespace Chess
