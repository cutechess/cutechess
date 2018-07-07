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

#include "twokingseachboard.h"
#include "westernzobrist.h"

namespace Chess {

TwoKingsEachBoard::TwoKingsEachBoard()
	: WesternBoard(new WesternZobrist()),
	  m_symmetrical(false),
	  m_castlingSourceFile(4)
{
}

Board* TwoKingsEachBoard::copy() const
{
	return new TwoKingsEachBoard(*this);
}

QString TwoKingsEachBoard::variant() const
{
	return "twokings";
}

QString TwoKingsEachBoard::defaultFenString() const
{
	return "rnbqkknr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKKNR w KQkq - 0 1";
}

void TwoKingsEachBoard::vInitialize()
{
	m_symmetrical = isSymmetrical();
	WesternBoard::vInitialize();
}

bool TwoKingsEachBoard::kingsCountAssertion(int whiteKings, int blackKings) const
{
	return whiteKings > 0 && blackKings > 0;
}

bool TwoKingsEachBoard::isSymmetrical() const
{
	return false;
}

int TwoKingsEachBoard::royalKingSquare(Side side) const
{
	Q_ASSERT(!side.isNull());
	bool reverseOrder = m_symmetrical && side == Side::Black;
	int boardWidth = width();
	int boardHeight = height();

	for (int file = 0; file < boardWidth; file++)
		for (int rank = 0; rank < boardHeight; rank++)
		{
			int relativeRank = reverseOrder ?
				boardHeight - 1 - rank: rank;
			Square sq = Square(file, relativeRank);
			Piece piece = pieceAt(sq);

			if (piece.type() == King
			&&  piece.side() == side)
				return squareIndex(sq);
		}
	return 0;
}

bool TwoKingsEachBoard::inCheck(Side side, int square) const
{
	if (square == 0)
		square = royalKingSquare(side);

	return WesternBoard::inCheck(side, square);
}

void TwoKingsEachBoard::generateMovesForPiece(QVarLengthArray< Move >& moves, int pieceType, int square) const
{
	if (pieceType != King)
		return WesternBoard::generateMovesForPiece(moves,
							   pieceType,
							   square);

	QVarLengthArray< Move > testmoves;
	WesternBoard::generateMovesForPiece(testmoves, King, square);

	for (const auto& move : testmoves)
	{
		int src = move.sourceSquare();
		int tgt = move.targetSquare();
		Square srcSq = chessSquare(src);
		Square tgtSq = chessSquare(tgt);
		// only use non-castling moves
		if (abs(srcSq.file() - tgtSq.file()) <= 1)
			moves.append(move);
	}
	// castling
	Side side = sideToMove();
	Square sq = chessSquare(square);
	int rrank = (side == Side::White) ? sq.rank()
					  : height() - 1 - sq.rank();
	int rookSqQ = square - m_castlingSourceFile;
	int rookSqK = rookSqQ + width() - 1;

	if (sq.file() == m_castlingSourceFile && rrank == 0)
	{
		bool freeK = true;
		bool freeQ = true;
		for (int index = square + 1; index < rookSqK; index++)
			if (!pieceAt(index).isEmpty())
				freeK = false;
		for (int index = rookSqQ + 1; index < square; index++)
			if (!pieceAt(index).isEmpty())
				freeQ = false;

		if (hasCastlingRight(side, KingSide) && freeK)
			moves.append(Move(square, rookSqK));

		if (hasCastlingRight(side, QueenSide) && freeQ)
			moves.append(Move(square, rookSqQ));
	}
}

Move TwoKingsEachBoard::moveFromLanString(const QString& str)
{
	Move move(Board::moveFromLanString(str));

	int source = move.sourceSquare();
	int target = move.targetSquare();
	Piece piece = pieceAt(source);
	int srcFile = chessSquare(source).file();
	int tgtFile = chessSquare(target).file();

	if (piece.type() == King
	&&  qAbs(srcFile - tgtFile) > 1)
	{
		// find out which target square is needed by WesternBoard
		Side side = sideToMove();
		Move move1 = Move(kingSquare(side), target);
		QString str2(lanMoveString(move1));
		Move move2 = WesternBoard::moveFromLanString(str2);
		Move move3 = Move(source, move2.targetSquare());
		return move3;
	}

	return move;
}

Move TwoKingsEachBoard::moveFromSanString(const QString& str)
{
	if (!str.startsWith("O-O"))
		return WesternBoard::moveFromSanString(str);

	Side side = sideToMove();
	int srcFile = m_castlingSourceFile;
	int rank = (side == Side::White) ? 0 : height() - 1;
	int tgtFile = (str.startsWith("O-O-O")) ? 0 : width() - 1;
	Square srcSq = Square(srcFile, rank);
	Square tgtSq = Square(tgtFile, rank);

	Move move(squareIndex(srcSq), squareIndex(tgtSq));
	return move;
}

Result TwoKingsEachBoard::result()
{
	QString str;

	// Checkmate/Stalemate
	if (!canMove())
	{
		if (inCheck(sideToMove()))
		{
			Side winner = sideToMove().opposite();
			str = tr("%1 mates").arg(winner.toString());

			return Result(Result::Win, winner, str);
		}
		else
		{
			str = tr("Draw by stalemate");
			return Result(Result::Draw, Side::NoSide, str);
		}
	}

	// Insufficient mating material
	int material = -4; // two kings
	bool bishops[] = { false, false };
	for (int i = 0; i < arraySize(); i++)
	{
		const Piece& piece = pieceAt(i);
		if (!piece.isValid())
			continue;

		switch (piece.type())
		{
		case Bishop:
		{
			auto color = chessSquare(i).color();
			if (color != Square::NoColor && !bishops[color])
			{
				material++;
				bishops[color] = true;
			}
			break;
		}
		case Knight:
			material++;
			break;
		default:
			material += 2;
			break;
		}
	}
	if (material <= 1)
	{
		str = tr("Draw by insufficient mating material");
		return Result(Result::Draw, Side::NoSide, str);
	}

	// 50 move rule
	if (reversibleMoveCount() >= 100)
	{
		str = tr("Draw by fifty moves rule");
		return Result(Result::Draw, Side::NoSide, str);
	}

	// 3-fold repetition
	if (repeatCount() >= 2)
	{
		str = tr("Draw by 3-fold repetition");
		return Result(Result::Draw, Side::NoSide, str);
	}

	return Result();
}



TwoKingsSymmetricalBoard::TwoKingsSymmetricalBoard()
	: TwoKingsEachBoard()
{
}

Board* TwoKingsSymmetricalBoard::copy() const
{
	return new TwoKingsSymmetricalBoard(*this);
}

QString TwoKingsSymmetricalBoard::variant() const
{
	return "twokingssymmetric";
}

bool TwoKingsSymmetricalBoard::isSymmetrical() const
{
	return true;
}

QString TwoKingsSymmetricalBoard::defaultFenString() const
{
	return "rnbqkknr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKKNR w KQkq - 0 1";
}

} // namespace Chess
