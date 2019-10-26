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

#include "placementboard.h"
#include "westernzobrist.h"

namespace Chess {

PlacementBoard::PlacementBoard()
	: WesternBoard(new WesternZobrist()),
	  m_inSetUp(true),
	  m_previouslyInSetUp(true)
{
}

Board* PlacementBoard::copy() const
{
	return new PlacementBoard(*this);
}

QString PlacementBoard::variant() const
{
	return "placement";
}

QString PlacementBoard::defaultFenString() const
{
	return "8/pppppppp/8/8/8/8/PPPPPPPP/8[KQRRBBNNkqrrbbnn] w - - 0 1";
}

bool PlacementBoard::variantHasDrops() const
{
	return true;
}

QList< Piece > PlacementBoard::reservePieceTypes() const
{
	QList< Piece > types;
	for (Side side: { Side::White, Side::Black} )
	{
		for (int type = Knight; type <= King; type++)
			types << Piece(side, type);
	}
	return types;
}

bool PlacementBoard::kingsCountAssertion(int whiteKings, int blackKings) const
{
	Piece whiteKing(Side::White, King);
	Piece blackKing(Side::Black, King);
	return whiteKings + reserveCount(whiteKing) == 1 && blackKings + reserveCount(blackKing) == 1;
}

void PlacementBoard::generateMovesForPiece(QVarLengthArray< Move >& moves,
					  int pieceType,
					  int square) const
{
	// Normal game moves
	if (!m_inSetUp)
	{
		// No drops after set up
		if (square == 0)
			return;

		WesternBoard::generateMovesForPiece(moves, pieceType, square);
		return;
	}

	// Set-up: generate drops onto own half
	if (square != 0)
		return;

	Side side = sideToMove();
	const int size = arraySize();
	const bool isBlack = (side == Side::Black);
	const int arwidth = width() + 2;
	const int start = 2 * arwidth + 1;
	const int end = start + width();

	// loop index i will have Black side perspective
	for (int i = start; i < end; i++)
	{
		bool ok = true;
		int index = isBlack ? i : size - 1 - i;
		Piece tmp = pieceAt(index);
		if (!tmp.isEmpty())
			continue;

		// Square on a-file
		int a = 1 + index / arwidth * arwidth;

		// Bishops must not be placed on same coloured squares
		if(pieceType == Bishop)
		{
			for (int s = a; s < a + width(); s++)
			{
				if (s % 2 != index % 2)
					continue;
				if (pieceAt(s).type() == Bishop)
					ok = false;
			}
		}
		else if (reserveCount(Piece(side, Bishop)) > 0)
		{
			bool colorOk[2] {false, false};

			for (int s = a; s < a + width(); s++)
			{
				if (s == index)
					continue;
				tmp = pieceAt(s);
				if (tmp.isEmpty() || tmp.type() == Bishop)
					colorOk[s % 2] = true;
			}
			ok = colorOk[0] && colorOk[1];
		}

		if (ok)
			moves.append(Move(0, index, pieceType));
	}
}

bool PlacementBoard::inSetup() const
{
	// Set-up phase ends when no pieces are left in reserve
	const QList<Piece> reserveTypes = reservePieceTypes();
	for (const Piece piece: reserveTypes)
	{
		if (reserveCount(piece) > 0)
			return true;
	}
	return false;
}

bool PlacementBoard::vSetFenString(const QStringList& fen)
{
	int ret = WesternBoard::vSetFenString(fen);
	m_inSetUp = inSetup();

	return ret;
}


void PlacementBoard::vMakeMove(const Move& move, BoardTransition* transition)
{
	WesternBoard::vMakeMove(move, transition);

	if (move.sourceSquare() == 0)
		removeFromReserve(Piece(sideToMove(), move.promotion()));

	m_inSetUp = inSetup();
}

void PlacementBoard::vUndoMove(const Move& move)
{
	WesternBoard::vUndoMove(move);

	if (move.sourceSquare() == 0)
		addToReserve(Piece(sideToMove(), move.promotion()));

	m_inSetUp = inSetup();
}

bool PlacementBoard::isLegalPosition()
{
	if (!m_inSetUp)
		return WesternBoard::isLegalPosition();

	return true;
}

void PlacementBoard::setCastlingRights()
{
	for (const QChar c: "AHah")
		parseCastlingRights(c);

	// only support orthodox castling
	const Side sides[2]{Side::White, Side::Black};
	for (const Side side: sides)
	{
		int ksq = kingSquare(side);
		if (chessSquare(ksq).file() != width()/2)
			removeCastlingRights(side);
	}
}

Result PlacementBoard::result()
{
	if (m_previouslyInSetUp && !m_inSetUp)
		setCastlingRights();

	if (m_inSetUp && !m_previouslyInSetUp)
	{
	      removeCastlingRights(Side::White);
	      removeCastlingRights(Side::Black);
	}
	m_previouslyInSetUp = m_inSetUp;
	return WesternBoard::result();
}

} // namespace Chess
