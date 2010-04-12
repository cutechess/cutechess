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

#include "crazyhouseboard.h"
#include "westernzobrist.h"

namespace Chess {

REGISTER_BOARD(CrazyhouseBoard, "crazyhouse")

CrazyhouseBoard::CrazyhouseBoard(QObject* parent)
	: WesternBoard(new WesternZobrist(), parent)
{
	setPieceType(PromotedKnight, tr("promoted knight"), "N-", KnightMovement);
	setPieceType(PromotedBishop, tr("promoted bishop"), "B-", BishopMovement);
	setPieceType(PromotedRook, tr("promoted rook"), "R-", RookMovement);
	setPieceType(PromotedQueen, tr("promoted queen"), "Q-", BishopMovement | RookMovement);
}

QString CrazyhouseBoard::variant() const
{
	return "crazyhouse";
}

bool CrazyhouseBoard::variantHasDrops() const
{
	return true;
}

QString CrazyhouseBoard::defaultFenString() const
{
	return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - KQkq - 0 1";
}

int CrazyhouseBoard::handPieceType(int pieceType) const
{
	if (pieceType >= PromotedKnight && pieceType <= PromotedQueen)
		return Pawn;
	return pieceType;
}

int CrazyhouseBoard::normalPieceType(int type)
{
	switch (type)
	{
	case PromotedKnight:
		return Knight;
	case PromotedBishop:
		return Bishop;
	case PromotedRook:
		return Rook;
	case PromotedQueen:
		return Queen;
	default:
		return type;
	}
}

int CrazyhouseBoard::promotedPieceType(int type)
{
	switch (type)
	{
	case Knight:
		return PromotedKnight;
	case Bishop:
		return PromotedBishop;
	case Rook:
		return PromotedRook;
	case Queen:
		return PromotedQueen;
	default:
		return type;
	}
}

QString CrazyhouseBoard::lanMoveString(const Move& move)
{
	Move tmp(move.sourceSquare(),
		 move.targetSquare(),
		 normalPieceType(move.promotion()));

	return WesternBoard::lanMoveString(tmp);
}

Move CrazyhouseBoard::moveFromLanString(const QString& str)
{
	Move move(WesternBoard::moveFromLanString(str));
	if (move.promotion() == Piece::NoPiece || move.sourceSquare() == 0)
		return move;

	return Move(move.sourceSquare(),
		    move.targetSquare(),
		    promotedPieceType(move.promotion()));
}

void CrazyhouseBoard::vMakeMove(const Move& move, QVarLengthArray<int>& changedSquares)
{
	Q_UNUSED(changedSquares);

	int ctype = captureType(move);
	if (ctype != Piece::NoPiece)
		addHandPiece(Piece(sideToMove(), handPieceType(ctype)));
	else if (move.sourceSquare() == 0)
		removeHandPiece(Piece(sideToMove(), move.promotion()));

	return WesternBoard::vMakeMove(move, changedSquares);
}

void CrazyhouseBoard::vUndoMove(const Move& move)
{
	WesternBoard::vUndoMove(move);

	int ctype = captureType(move);
	if (ctype != Piece::NoPiece)
		removeHandPiece(Piece(sideToMove(), handPieceType(ctype)));
	else if (move.sourceSquare() == 0)
		addHandPiece(Piece(sideToMove(), move.promotion()));
}

void CrazyhouseBoard::addPromotions(int sourceSquare,
				    int targetSquare,
				    QVarLengthArray<Move>& moves) const
{
	moves.append(Move(sourceSquare, targetSquare, PromotedKnight));
	moves.append(Move(sourceSquare, targetSquare, PromotedBishop));
	moves.append(Move(sourceSquare, targetSquare, PromotedRook));
	moves.append(Move(sourceSquare, targetSquare, PromotedQueen));
}

void CrazyhouseBoard::generateMovesForPiece(QVarLengthArray<Move>& moves,
					    int pieceType,
					    int square) const
{
	// Generate drops
	if (square == 0)
	{
		const int size = arraySize();
		const int maxRank = height() - 2;
		for (int i = 0; i < size; i++)
		{
			Piece tmp = pieceAt(i);
			if (!tmp.isEmpty())
				continue;
			if (pieceType == Pawn)
			{
				Square sq(chessSquare(i));
				if (sq.rank() < 1 || sq.rank() > maxRank)
					continue;
			}

			moves.append(Move(0, i, pieceType));
		}
	}
	else
		WesternBoard::generateMovesForPiece(moves, pieceType, square);
}

} // namespace Chess
