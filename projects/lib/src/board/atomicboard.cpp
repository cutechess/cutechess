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

#include "atomicboard.h"
#include "westernzobrist.h"
#include "boardtransition.h"


namespace Chess {

AtomicBoard::AtomicBoard()
	: WesternBoard(new WesternZobrist())
{
	for (int i = 0; i < 8; i++)
		m_offsets[i] = 0;
}

Board* AtomicBoard::copy() const
{
	return new AtomicBoard(*this);
}

QString AtomicBoard::variant() const
{
	return "atomic";
}

QString AtomicBoard::defaultFenString() const
{
	return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}

bool AtomicBoard::kingCanCapture() const
{
	return false;
}

void AtomicBoard::vInitialize()
{
	int arwidth = width() + 2;
	m_offsets[0] = -arwidth - 1;
	m_offsets[1] = -arwidth;
	m_offsets[2] = -arwidth + 1;
	m_offsets[3] = -1;
	m_offsets[4] = 1;
	m_offsets[5] = arwidth - 1;
	m_offsets[6] = arwidth;
	m_offsets[7] = arwidth + 1;

	WesternBoard::vInitialize();
}

bool AtomicBoard::vSetFenString(const QStringList& fen)
{
	m_history.clear();
	return WesternBoard::vSetFenString(fen);
}

bool AtomicBoard::inCheck(Side side, int square) const
{
	if (square == 0)
	{
		int kingSq = kingSquare(side);

		// If the kings touch, there's no check
		for (int i = 0; i < 8; i++)
		{
			Piece pc = pieceAt(kingSq + m_offsets[i]);
			if (pc.type() == King)
				return false;
		}
	}

	return WesternBoard::inCheck(side, square);
}

bool AtomicBoard::vIsLegalMove(const Move& move)
{
	Q_ASSERT(!move.isNull());

	if (captureType(move) != Piece::NoPiece)
	{
		bool explodeOppKing = false;
		int target = move.targetSquare();
		for (int i = 0; i < 8; i++)
		{
			Piece pc = pieceAt(target + m_offsets[i]);
			if (pc.type() == King)
			{
				// Can't explode your own king
				if (pc.side() == sideToMove())
					return false;
				explodeOppKing = true;
			}
		}
		// The move is always legal if the enemy king
		// is in the blast zone and own king is safe
		if (explodeOppKing)
			return true;
	}

	return WesternBoard::vIsLegalMove(move);
}

void AtomicBoard::vMakeMove(const Move& move, BoardTransition* transition)
{
	MoveData md;
	md.isCapture = (captureType(move) != Piece::NoPiece);
	md.piece = pieceAt(move.sourceSquare());

	WesternBoard::vMakeMove(move, transition);

	if (md.isCapture)
	{
		int target = move.targetSquare();
		setSquare(target, Piece::NoPiece);
		for (int i = 0; i < 8; i++)
		{
			int sq = target + m_offsets[i];
			Piece& pc = md.captures[i];
			pc = pieceAt(sq);
			if (pc.isWall() || pc.type() == Pawn)
				continue;
			
			removeCastlingRights(sq);
			setSquare(sq, Piece::NoPiece);

			if (transition != nullptr)
				transition->addSquare(chessSquare(sq));
		}
	}
	m_history << md;
}

void AtomicBoard::vUndoMove(const Move& move)
{
	int source = move.sourceSquare();
	int target = move.targetSquare();

	WesternBoard::vUndoMove(move);

	const MoveData& md = m_history.last();
	if (md.isCapture)
	{
		setSquare(source, md.piece);
		for (int i = 0; i < 8; i++)
		{
			int sq = target + m_offsets[i];
			if (md.captures[i].isValid())
				setSquare(sq, md.captures[i]);
		}
	}

	m_history.pop_back();
}

Result AtomicBoard::result()
{
	Side side(sideToMove());
	if (pieceAt(kingSquare(side)).isEmpty())
	{
		Side winner = side.opposite();
		QString str = tr("%1's king exploded").arg(side.toString());
		return Result(Result::Win, winner, str);
	}

	return WesternBoard::result();
}

} // namespace Chess
