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

#include "shatranjboard.h"
#include "westernzobrist.h"

namespace Chess {

ShatranjBoard::ShatranjBoard()
	: WesternBoard(new WesternZobrist())
{
	setPieceType(Ferz, tr("ferz"), "Q", FerzMovement, "F");
	setPieceType(Alfil, tr("alfil"), "B", AlfilMovement, "E");
}

Board* ShatranjBoard::copy() const
{
	return new ShatranjBoard(*this);
}

QString ShatranjBoard::variant() const
{
	return "shatranj";
}

QString ShatranjBoard::defaultFenString() const
{
	return "rnbkqbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBKQBNR w - - 0 1";
}

bool ShatranjBoard::hasCastling() const
{
	return false;
}

bool ShatranjBoard::pawnHasDoubleStep() const
{
	return false;
}

void ShatranjBoard::vInitialize()
{
	WesternBoard::vInitialize();

	m_arwidth = width() + 2;

	m_ferzOffsets.resize(4);
	m_ferzOffsets[0] = -m_arwidth - 1;
	m_ferzOffsets[1] = -m_arwidth + 1;
	m_ferzOffsets[2] = m_arwidth - 1;
	m_ferzOffsets[3] = m_arwidth + 1;

	m_alfilOffsets.resize(4);
	m_alfilOffsets[0] = -2 * m_arwidth - 2;
	m_alfilOffsets[1] = -2 * m_arwidth + 2;
	m_alfilOffsets[2] = 2 * m_arwidth - 2;
	m_alfilOffsets[3] = 2 * m_arwidth + 2;
}

void ShatranjBoard::addPromotions(int sourceSquare,
				 int targetSquare,
				 QVarLengthArray<Move>& moves) const
{
	moves.append(Move(sourceSquare, targetSquare, Ferz));
}

void ShatranjBoard::generateMovesForPiece(QVarLengthArray< Move >& moves,
					  int pieceType,
					  int square) const
{
	Chess::WesternBoard::generateMovesForPiece(moves, pieceType, square);
	if (pieceHasMovement(pieceType, FerzMovement))
		generateHoppingMoves(square, m_ferzOffsets, moves);
	if (pieceHasMovement(pieceType, AlfilMovement))
		generateHoppingMoves(square, m_alfilOffsets, moves);
}

bool ShatranjBoard::inCheck(Side side, int square) const
{
	Piece piece;
	Side opSide = side.opposite();
	if (square == 0)
		square = kingSquare(side);

	// Ferz attacks
	for (int i = 0; i < m_ferzOffsets.size(); i++)
	{
		piece = pieceAt(square + m_ferzOffsets[i]);
		if (piece.side() == opSide
		&&  pieceHasMovement(piece.type(), FerzMovement))
			return true;
	}
	// Alfil attacks
	for (int i = 0; i < m_alfilOffsets.size(); i++)
	{
		int target = square + m_alfilOffsets[i];
		if (!isValidSquare(chessSquare(target)))
			continue;
		piece = pieceAt(target);
		if (piece.side() == opSide
		&&  pieceHasMovement(piece.type(), AlfilMovement))
			return true;
	}
	return WesternBoard::inCheck(side, square);
}

Result ShatranjBoard::result()
{
	Side side = sideToMove();

	// mate or stalemate
	if (!canMove())
	{
		Side winner = side.opposite();
		bool check = inCheck(side);
		QString str{ tr("%1 %2")
			     .arg(winner.toString())
			     .arg(check ? tr("mates") : tr("wins by stalemate")) };
		return Result(Result::Win, winner, str);
	}

	// bare king
	if (bareKing(side))
	{
		if (bareKing(side.opposite()))
		{
			QString str{ tr("Both kings bare") };
			return Result(Result::Draw, Side::NoSide, str);
		}
		if (!canBareOpponentKing())
		{
			Side winner = side.opposite();
			QString str{ tr("Bare king") };
			return Result(Result::Win, winner, str);
		}
	}
	if (bareKing(side.opposite())) //forfeited chance
	{
		Side winner = side;
		QString str{ tr("Bare king") };
		return Result(Result::Win, winner, str);
	}

	// 70 moves rule
	if (reversibleMoveCount() >= 140)
	{
		QString str{ tr("Draw by seventy moves rule") };
		return Result(Result::Draw, Side::NoSide, str);
	}

	// 3-fold repetition
	if (repeatCount() >= 2)
	{
		QString str{ tr("Draw by 3-fold repetition") };
		return Result(Result::Draw, Side::NoSide, str);
	}

	return Result();
}

bool ShatranjBoard::bareKing(Side side, int count) const
{
	for (int i = 0; count < 2 && i < arraySize(); i++)
	{
		if (side == pieceAt(i).side())
			++count;
	}
	return count < 2;
}

bool ShatranjBoard::canBareOpponentKing()
{
	Side side = sideToMove();
	// too many pieces at strong side?
	if (!bareKing(side.opposite(), -1))
		return false;

	QVarLengthArray<Move>moves;
	generateMovesForPiece(moves, King, kingSquare(side));
	for (const auto &m: moves)
	{
		if (captureType(m) != Piece::NoPiece
		&&  isLegalMove(m))
			return true;
	}
	return false;
}

} // namespace Chess
