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

#include "rifleboard.h"
#include "westernzobrist.h"
#include "boardtransition.h"

namespace Chess {

RifleBoard::RifleBoard()
	: WesternBoard(new WesternZobrist())
{
}

Board* RifleBoard::copy() const
{
	return new RifleBoard(*this);
}

QString RifleBoard::variant() const
{
	return "rifle";
}

QString RifleBoard::defaultFenString() const
{
	return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}

void RifleBoard::vMakeMove(const Move& move, BoardTransition* transition)
{
	int source = move.sourceSquare();
	int target = move.targetSquare();
	bool isCapture = (captureType(move) != Piece::NoPiece);

	WesternBoard::vMakeMove(move, transition);

	if (isCapture)
	{
		setSquare(source, pieceAt(target));
		setSquare(target, Piece::NoPiece);
		if (transition != nullptr)
		{
			transition->addSquare(chessSquare(source));
			transition->addSquare(chessSquare(target));
		}
	}
}

void RifleBoard::vUndoMove(const Move& move)
{
	int source = move.sourceSquare();
	int target = move.targetSquare();

	if (pieceAt(target) == Piece::NoPiece
	&&  pieceAt(source) != Piece::NoPiece)
	{
		setSquare(target, pieceAt(source));
		setSquare(source, Piece::NoPiece);
	}
	WesternBoard::vUndoMove(move);
}

bool RifleBoard::inCheck(Side side, int square) const
{
	// After King has made a capture WesternBoard's kingSquare is incorrect
	Piece king(side, King);
	int ksq = kingSquare(side);

	if (square == 0 &&  pieceAt(ksq) != king)
	{
		square = 2 * width() + 1;
		const int size = arraySize() - square;
		while (square < size && pieceAt(square) != king)
			square++;
	}
	return WesternBoard::inCheck(side, square);
}

void RifleBoard::addPromotions(int sourceSquare, int targetSquare,
			       QVarLengthArray< Move >& moves) const
{
	// Prevent capturing Pawn from promoting on seventh rank
	if (pieceAt(targetSquare).isEmpty())
	{
		WesternBoard::addPromotions(sourceSquare,
					    targetSquare,
					    moves);
		return;
	}
	moves.append(Move(sourceSquare, targetSquare));
}


ShootBoard::ShootBoard()
	: RifleBoard(),
	  m_testKey(0),
	  m_canCapture(false)
{
}

Board* ShootBoard::copy() const
{
	return new ShootBoard(*this);
}

QString ShootBoard::variant() const
{
	return "shoot";
}

bool ShootBoard::vIsLegalMove(const Move& move)
{
	if (!WesternBoard::vIsLegalMove(move))
		return false;

	if (captureType(move) != Piece::NoPiece)
		return true;

	// Try to use old results to gain higher efficiency.
	if (key() != m_testKey)
	{
		m_testKey = key();
		m_canCapture = false;
		QVarLengthArray<Move> moves;
		generateMoves(moves);

		// Search for any legal capture move.
		for (int i = 0; i < moves.size(); i++)
		{
			if (captureType(moves[i]) != Piece::NoPiece
			&&  WesternBoard::vIsLegalMove(moves[i]))
			{
				m_canCapture = true;
				break;
			}
		}
	}
	// Move is illegal if a capture move exists else legal.
	return !m_canCapture;
}

} // namespace Chess
