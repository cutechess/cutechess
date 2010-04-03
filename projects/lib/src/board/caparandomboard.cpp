/*
    This file is part of Cute Chess.

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

#include "caparandomboard.h"


static void addPiece(QVector<int>& pieces,
		     int piece,
		     int pos,
		     int start = 0,
		     int step = 1)
{
	int i = 0;
	for (int j = start; j < pieces.size(); j += step)
	{
		if (pieces[j] != Chess::Piece::NoPiece)
			continue;
		if (i == pos)
		{
			pieces[j] = piece;
			break;
		}
		i++;
	}
}


namespace Chess {

REGISTER_BOARD(CaparandomBoard, "Caparandom")

CaparandomBoard::CaparandomBoard(QObject* parent)
	: CapablancaBoard(parent)
{
}

QString CaparandomBoard::variant() const
{
	return "Caparandom";
}

bool CaparandomBoard::isRandomVariant() const
{
	return true;
}

bool CaparandomBoard::pawnsAreSafe(const QVector<int>& pieces) const
{
	int size = pieces.size();

	for (int i = 0; i < size; i++)
	{
		bool safe = false;

		for (int j = i - 2; j <= i + 2; j += 4)
		{
			if (j < 0 || j >= size)
				continue;
			if (pieceHasMovement(pieces[j], KnightMovement))
				safe = true;
		}
		for (int j = i - 1; j <= i + 1; j += 2)
		{
			if (j < 0 || j >= size)
				continue;
			if (pieceHasMovement(pieces[j], BishopMovement)
			||  pieces[j] == King)
				safe = true;
		}
		if (pieceHasMovement(pieces[i], RookMovement) || pieces[i] == King)
			safe = true;

		if (!safe)
			return false;
	}

	return true;
}

QString CaparandomBoard::defaultFenString() const
{
	const int empty = Piece::NoPiece;
	QVector<int> pieces(10);

	// Generate positions until we get one where all the pawns are
	// protected. This usually takes a handful of tries.
	do
	{
		pieces.fill(empty);
		if ((qrand() % 2) == 0)
		{
			addPiece(pieces, Queen, qrand() % 5, 0, 2);
			addPiece(pieces, Archbishop, qrand() % 5, 1, 2);
		}
		else
		{
			addPiece(pieces, Archbishop, qrand() % 5, 0, 2);
			addPiece(pieces, Queen, qrand() % 5, 1, 2);
		}
		addPiece(pieces, Bishop, qrand() % 4, 0, 2);
		addPiece(pieces, Bishop, qrand() % 4, 1, 2);
		addPiece(pieces, Chancellor, qrand() % 6);
		addPiece(pieces, Knight, qrand() % 5);
		addPiece(pieces, Knight, qrand() % 4);
		addPiece(pieces, Rook, 0);
		addPiece(pieces, King, 0);
		addPiece(pieces, Rook, 0);
	}
	while (!pawnsAreSafe(pieces));

	QString fen;

	// Black pieces
	foreach (int pieceType, pieces)
		fen += pieceSymbol(Piece(Black, pieceType));
	fen += '/';
	// Black pawns
	for (int i = 0; i < width(); i++)
		fen += pieceSymbol(Piece(Black, Pawn));
	fen += '/';

	// Empty squares
	for (int i = 0; i < height() - 4; i++)
		fen += QString::number(pieces.size()) + '/';

	// White pawns
	for (int i = 0; i < width(); i++)
		fen += pieceSymbol(Piece(White, Pawn));
	fen += '/';
	// White pieces
	foreach (int pieceType, pieces)
		fen += pieceSymbol(Piece(White, pieceType));

	// Side to move, castling rights, enpassant square, etc.
	fen += " w KQkq - 0 1";

	return fen;
}

} // namespace Chess
