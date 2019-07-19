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

#include "grandboard.h"

namespace Chess {

GrandBoard::GrandBoard()
	: CapablancaBoard()
{
}

Board* GrandBoard::copy() const
{
	return new GrandBoard(*this);
}

QString GrandBoard::variant() const
{
	return "grand";
}

int GrandBoard::height() const
{
	return 10;
}

QString GrandBoard::defaultFenString() const
{
	return "r8r/1nbqkcabn1/pppppppppp/10/10/10/10/PPPPPPPPPP/1NBQKCABN1/R8R w - - 0 1";
}

bool GrandBoard::hasCastling() const
{
	return false;
}

inline int GrandBoard::pawnMoveOffset(const PawnStep& ps, int sign) const
{
	return sign * ps.file - sign * (width() + 2) * 1;
}

void GrandBoard::generateMovesForPiece(QVarLengthArray< Move >& moves,
				       int pieceType,
				       int square) const
{
	Chess::CapablancaBoard::generateMovesForPiece(moves, pieceType, square);
	if (pieceType != Pawn)
		return;

	Side side = sideToMove();
	Side opp = side.opposite();
	int sign = (side == Side::White) ? 1 : -1;
	int rank = chessSquare(square).rank();

	// add missing pawn double steps from third rank
	int rank3 = (side == Side::White) ? 2 : height() - 3;
	if (rank == rank3)
	{
		for (const PawnStep& pStep: m_pawnSteps)
		{
			if (pStep.type != FreeStep)
				continue;

			int targetSquare = square + pawnMoveOffset(pStep, sign);
			Piece capture = pieceAt(targetSquare);

			if (capture.isEmpty())
			{
				targetSquare += pawnMoveOffset(pStep, sign);
				capture = pieceAt(targetSquare);
				if (capture.isEmpty())
					moves.append(Move(square, targetSquare));
			}
		}
	}

	//add optional promotions on eighth and ninth ranks
	int rank7 = (side == Side::White) ? height() - 4 : 3;
	int rank8 = rank7 + sign;

	if (rank != rank7 && rank != rank8)
		return;

	for (const PawnStep& pStep: m_pawnSteps)
	{
		int targetSquare = square + pawnMoveOffset(pStep, sign);
		Piece capture = pieceAt(targetSquare);
		bool isCapture = capture.side() == opp
				||  targetSquare == enpassantSquare();

		if ((capture.isEmpty() && pStep.type == FreeStep)
		||  (isCapture && pStep.type == CaptureStep))
			addPromotions(square, targetSquare, moves);
	}
}

bool GrandBoard::vIsLegalMove(const Move& move)
{
	int promotion = move.promotion();

	if (promotion == Piece::NoPiece)
		return Chess::CapablancaBoard::vIsLegalMove(move);

	// only allow promotion to already captured piece
	Side side = sideToMove();
	int count = 1;

	for (int i = 0; i < arraySize(); i++)
	{
		Piece piece = pieceAt(i);
		if (piece.side() == side && piece.type() == promotion)
			count++;
	}

	if (promotion == Queen
	||  promotion == Chancellor
	||  promotion == Archbishop)
		return count <= 1
		&&     Chess::CapablancaBoard::vIsLegalMove(move);

	if (promotion == Rook
	||  promotion == Bishop
	||  promotion == Knight)
		return count <= 2
		&&     Chess::CapablancaBoard::vIsLegalMove(move);

	return false;
}

} // namespace Chess
