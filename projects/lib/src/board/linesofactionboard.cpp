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

#include "linesofactionboard.h"
#include "westernzobrist.h"
#include <QRegularExpression>

namespace Chess {

LinesOfActionBoard::LinesOfActionBoard()
	: WesternBoard(new WesternZobrist())
{
	setPieceType(Pawn,   "piece",    "P");
	setPieceType(Knight, "no-knight", "");
	setPieceType(Bishop, "no-bishop", "");
	setPieceType(Rook,   "no-rook",   "");
	setPieceType(Queen,  "no-queen",  "");
	setPieceType(King,   "no-king",   "");
}

Board* LinesOfActionBoard::copy() const
{
	return new LinesOfActionBoard(*this);
}

QString LinesOfActionBoard::variant() const
{
	return "linesofaction";
}

QString LinesOfActionBoard::defaultFenString() const
{
	return "1pppppp1/P6P/P6P/P6P/P6P/P6P/P6P/1pppppp1 b - - 0 1";
}

bool LinesOfActionBoard::kingsCountAssertion(int whiteKings,
					     int blackKings) const
{
	return whiteKings == 0 && blackKings == 0;
}

void LinesOfActionBoard::vInitialize()
{
	WesternBoard::vInitialize();

	int arwidth = width() + 2;

	m_offsets.resize(8);
	m_offsets[0] = -arwidth - 1;
	m_offsets[1] = -arwidth + 1;
	m_offsets[2] = arwidth - 1;
	m_offsets[3] = arwidth + 1;
	m_offsets[4] = -arwidth;
	m_offsets[5] = -1;
	m_offsets[6] = 1;
	m_offsets[7] = arwidth;

	m_minIndex = 2 * arwidth  + 1;
	m_maxIndex = arraySize() - 1 - m_minIndex;
	m_drawIfBothFinished = false;
}

bool LinesOfActionBoard::isLegalPosition()
{
	return true;
}

QString LinesOfActionBoard::sanMoveString(const Move& move)
{
	QString str = squareString(move.sourceSquare());
	str.append(captureType(move) != Piece::NoPiece ?  "x" : "-" );
	str.append(squareString(move.targetSquare()));

	return str;
}

Move LinesOfActionBoard::moveFromSanString(const QString& str)
{
	QString s(str);
	s.remove(QRegularExpression("[-:]"));
	return WesternBoard::moveFromLanString(s);
}

void LinesOfActionBoard::generateLineMoves(int sourceSquare,
					   const QVarLengthArray<int>& offsets,
					   QVarLengthArray<Move>& moves) const
{
	Side side = sideToMove();
	Side opp = side.opposite();
	for (int i = 0; i < offsets.size(); i++)
	{
		// At first count all pieces on the line
		int offset = offsets[i];
		int targetSquare = sourceSquare + offset;
		int count = 0;
		Piece capture;
		while (!(capture = pieceAt(targetSquare)).isWall())
		{
			if (capture.isValid())
				count++;
			targetSquare += offset;
		}
		targetSquare = sourceSquare;
		while (!(capture = pieceAt(targetSquare)).isWall())
		{
			if (capture.isValid())
				count++;
			targetSquare -= offset;
		}

		// Move exactly count squares in offset direction
		targetSquare = sourceSquare + offset;
		while (!(capture = pieceAt(targetSquare)).isWall())
		{
			count--;
			if (count == 0 && capture.side() != side)
				moves.append(Move(sourceSquare, targetSquare));
			// Done / Cannot jump over opponent pieces
			if (count == 0 || capture.side() == opp)
				break;
			targetSquare += offset;
		}
	}
}

void LinesOfActionBoard::generateMovesForPiece(QVarLengthArray<Move>& moves,
					       int pieceType,
					       int square) const
{
	if (pieceType != Pawn)
		return;

	generateLineMoves(square, m_offsets, moves);
}

int LinesOfActionBoard::components(Side side) const
{
	QVarLengthArray<int> pieces;
	int areas = 0;
	int count = 0;

	for (int index = m_minIndex; index <= m_maxIndex; index++)
	{
		if (pieceAt(index).side() != side)
			continue;

		if (!pieces.contains(index))
		{
			pieces.append(index);
			++areas;
		}

		while (count < pieces.count())
		{
			int square = pieces.at(count++);
			for (int offset: m_offsets)
			{
				int target = square + offset;
				if (pieceAt(target).side() == side
				&& !pieces.contains(target))
					pieces.append(target);
			}
		}
	}
	return areas;
}

Result LinesOfActionBoard::result()
{
	Side side = sideToMove();
	Side opp  = side.opposite();
	bool sideFinished = (components(side) < 2);
	bool oppFinished  = (components(opp) < 2);

	// Gamut of Games (1st ed.) rule by Sackson: draw if both connected
	if (oppFinished && sideFinished && m_drawIfBothFinished)
		return Result(Result::Draw, Side::NoSide, tr("Draw"));

	// Soucie's rule: player making last move wins if both connected
	Side winner = Side::NoSide;
	if (oppFinished)
		winner = opp;
	else if (sideFinished)
		winner = side;

	// Normal wins
	if (oppFinished || sideFinished)
	{
		QString str = tr("%1 wins").arg(winner.toString());
		return Result(Result::Win, winner, str);
	}

	// 3-fold repetition
	if (repeatCount() >= 2)
		return Result(Result::Draw, Side::NoSide, tr("Draw by repetition"));

	// Stalemated player loses
	if(!canMove())
	{
		QString str = tr("%1 wins by stalemate").arg(opp.toString());
		return Result(Result::Win, opp, str);
	}

	return Result();
}

} // namespace Chess
