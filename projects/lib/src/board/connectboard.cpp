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

#include "connectboard.h"
#include "westernzobrist.h"

namespace Chess {

ConnectBoard::ConnectBoard()
	: WesternBoard(new WesternZobrist()),
	  m_hasGravity(true),
	  m_connectToWin(4),
	  m_pieceCounter()
{
	setPieceType(Pawn, tr("pawn"), "P", 0);
	setPieceType(Knight, tr("no-knight"), "", 0);
	setPieceType(Bishop, tr("no-bishop"), "", 0);
	setPieceType(Rook, tr("no-rook"), "", 0);
	setPieceType(Queen, tr("no-queen"), "", 0);
	setPieceType(King, tr("no-king"), "", 0);
}

Board* ConnectBoard::copy() const
{
	return new ConnectBoard(*this);
}

QList<Chess::Piece> ConnectBoard::reservePieceTypes() const
{
	QList<Piece> list { Piece(Side::White, Pawn), Piece(Side::Black, Pawn) };
	return list;
}

bool ConnectBoard::variantHasDrops() const
{
	return true;
}

QString ConnectBoard::variant() const
{
	return "cfour";
}

QString ConnectBoard::defaultFenString() const
{
	return "7/7/7/7/7/7[PPPPPPPPPPPPPPPPPPPPPppppppppppppppppppppp] w - - 0 1";
}

int ConnectBoard::width() const
{
	return 7;
}

int ConnectBoard::height() const
{
	return 6;
}

bool ConnectBoard::hasGravity() const
{
	return true;
}

int ConnectBoard::connectToWin() const
{
	return 4;
}

int ConnectBoard::requiredNumberOfPieces() const
{
	return width() * height();
}

bool ConnectBoard::overlinesWin(Side) const
{
	return true;
}

void ConnectBoard::vInitialize()
{
	WesternBoard::vInitialize();
	m_hasGravity   = hasGravity();
	m_connectToWin = connectToWin();
}

bool ConnectBoard::vSetFenString(const QStringList &fen)
{
	m_pieceCounter.clear();
	m_pieceCounter.resize(width());

	if (!WesternBoard::vSetFenString(fen))
		return false;

	// Count pieces per file in this position
	for (int i = 0; i < arraySize(); i++)
	{
		if (!pieceAt(i).isValid())
			continue;
		int file = chessSquare(i).file();
		m_pieceCounter[file]++;
	}
	return true;
}

bool ConnectBoard::kingsCountAssertion(int, int) const
{
	return pieceCountOk();
}

void ConnectBoard::generateMovesForPiece(QVarLengthArray<Chess::Move> & moves,
					     int pieceType,
					     int square) const
{
	// Return if this is not a drop move for a Pawn
	if (pieceType != Pawn || square != 0)
		return;

	// No moves if game has finished
	if (connected(m_connectToWin, sideToMove().opposite()))
		return;

	// If the game has no gravity then insert the piece on any empty square
	if (!m_hasGravity)
	{
		int minIndex = 2 * (width() + 2) + 1;
		for ( int i = minIndex; i < arraySize() - minIndex; i++)
			if (pieceAt(i).isEmpty())
				moves.append(Move(0, i, pieceType));
		return;
	}

	// Insert piece on top of others, use piece counter per file
	for (int file = 0; file < width(); file++)
	{
		int rank = m_pieceCounter.at(file);
		if (rank >= height())
			continue;

		Square sq(file, rank);
		int target = squareIndex(sq);
		const Move m(0, target, pieceType);
		moves.append(m);
	}
}

void ConnectBoard::vMakeMove(const Move & move,
				 BoardTransition * transition)
{
	// Increase piece counter for target file
	Square sq = chessSquare(move.targetSquare());
	m_pieceCounter[sq.file()]++;

	int prom = move.promotion();
	if (prom != Piece::NoPiece)
		removeFromReserve(Piece(sideToMove(), prom));

	WesternBoard::vMakeMove(move, transition);
}

void ConnectBoard::vUndoMove(const Move & move)
{
	// Decrease piece counter for target file
	Square sq = chessSquare(move.targetSquare());
	m_pieceCounter[sq.file()]--;

	int prom = move.promotion();
	if (prom != Piece::NoPiece)
		addToReserve(Piece(sideToMove(), prom));

	WesternBoard::vUndoMove(move);
	return;
}

bool ConnectBoard::isLegalPosition()
{
	//Skip test pieceCountOk(): kingsCountAssertion used for this
	return true;
}

bool ConnectBoard::pieceCountOk() const
{
	// Set the side balance counter one off for an odd number of squares
	int squares = width() * height();
	int counter = squares % 2;

	// Check the numbers of pieces White vs Black in reserve
	int total = 0;
	const QList<Chess::Piece> pieceTypes = reservePieceTypes();
	for (const auto& piece: pieceTypes)
	{
		int reserve = reserveCount(piece);
		total += reserve;
		Side side = piece.side();
		counter += (side == Side::Black ? reserve: -reserve);
	}
	int expectedValue = (sideToMove() == Side::White ? 0 : 1);
	if (counter != expectedValue)
		return false;

	// Check the numbers of pieces White vs Black on the board
	counter = 0;
	for (int i = 0; i < arraySize(); i++)
	{
		Side side = pieceAt(i).side();
		if (side == Side::White || side == Side::Black)
			total++;
		if (side == Side::White)
			counter++;
		else if (side == Side::Black)
			counter--;
	}
	if (counter != expectedValue)
		return false;

	// Check the total number of pieces
	if (requiredNumberOfPieces() < 0)
		return true;

	return total == requiredNumberOfPieces();
}

bool ConnectBoard::connected(int n, Side side) const
{
	bool overlineWins = overlinesWin(side);
	// Examine files
	for (int file = 0; file < width(); file++)
	{
		int count = 0;
		for (int rank = 0; rank < height(); rank++)
		{
			if (pieceAt(Square(file, rank)).side() == side)
				count++;
			else if (count == n)
				return true;
			else
				count = 0;
			if (count >= n && overlineWins)
				return true;
		}
		if (count == n)
			return true;
	}
	// Examine ranks
	for (int rank = 0; rank < height(); rank++)
	{
		int count = 0;
		for (int file = 0; file < width(); file++)
		{
			if (pieceAt(Square(file, rank)).side() == side)
				count++;
			else if (count == n)
				return true;
			else
				count = 0;
			if (count >= n && overlineWins)
				return true;
		}
		if (count == n)
			return true;
	}
	// Examine rising to rhs diagonals
	for (int d = -width() + n; d <= height() - n; d++)
	{
		int count = 0;
		for (int file = 0; file < width(); file++)
		{
			int rank = file + d;
			if (rank < 0 || rank >= height())
				continue;
			if (pieceAt(Square(file, rank)).side() == side)
				count++;
			else if (count == n)
				return true;
			else
				count = 0;
			if (count >= n && overlineWins)
				return true;
		}
		if (count == n)
			return true;
	}
	// Examine falling to rhs diagonals
	for (int d = n - 1; d <= width() + height() - (n + 1); d++)
	{
		int count = 0;
		for (int file = 0; file < width(); file++)
		{
			int rank = -file + d;
			if (rank < 0 || rank >= height())
				continue;
			if (pieceAt(Square(file, rank)).side() == side)
				count++;
			else if (count == n)
				return true;
			else
				count = 0;
			if (count >= n && overlineWins)
				return true;
		}
		if (count == n)
			return true;
	}
	return false;
}

Result ConnectBoard::result()
{
	Side side = sideToMove();
	Side opp = side.opposite();
	QString str;

	// Check whether enough pieces have been connected
	if (connected(m_connectToWin, opp))
	{
		str.append(tr("%1 wins").arg(opp.toString()));
		return Result(Result::Win, opp, str);
	}
	if (plyCount() == 0 && connected(m_connectToWin, side))
	{
		str.append(tr("%1 wins").arg(side.toString()));
		return Result(Result::Win, side, str);
	}
	// The game goes on if there are pieces in the reserve
	const QList<Chess::Piece> pieceTypes = reservePieceTypes();
	for (const auto& pieceType: pieceTypes)
	{
		if (reserveCount(pieceType) > 0)
			return Result();
	}

	// No pieces left
	str.append(tr("Draw by stalemate"));
	return Result(Result::Draw, Side::NoSide, str);
}


TicTacToeBoard::TicTacToeBoard()
	: ConnectBoard()
{
}

Board * TicTacToeBoard::copy() const
{
	return new TicTacToeBoard(*this);
}

QString TicTacToeBoard::variant() const
{
	return "tictactoe";
}

QString TicTacToeBoard::defaultFenString() const
{
	return "3/3/3[PPPPPpppp] w - - 0 1";
}

int TicTacToeBoard::width() const
{
	return 3;
}

int TicTacToeBoard::height() const
{
	return 3;
}

bool TicTacToeBoard::hasGravity() const
{
	return false;
}

int TicTacToeBoard::connectToWin() const
{
	return 3;
}


GomokuFreestyleBoard::GomokuFreestyleBoard()
	: ConnectBoard()
{
}

Board * GomokuFreestyleBoard::copy() const
{
	return new GomokuFreestyleBoard(*this);
}

QString GomokuFreestyleBoard::variant() const
{
	return "gomokufreestyle";
}

QString GomokuFreestyleBoard::defaultFenString() const
{
	return "15/15/15/15/15/15/15/15/15/15/15/15/15/15/15[PPPPPPPPPPPPPPPPPP"
	"PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP"
	"PPPPPPPPPPPPPPPPPPPPPPPPPppppppppppppppppppppppppppppppppppppppppppppp"
	"ppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp] w"
	" - - 0 1";
}

int GomokuFreestyleBoard::width() const
{
	return 15;
}

int GomokuFreestyleBoard::height() const
{
	return 15;
}

bool GomokuFreestyleBoard::hasGravity() const
{
	return false;
}

int GomokuFreestyleBoard::connectToWin() const
{
	return 5;
}


GomokuBoard::GomokuBoard()
	: GomokuFreestyleBoard()
{
}

Board * GomokuBoard::copy() const
{
	return new GomokuBoard(*this);
}

QString GomokuBoard::variant() const
{
	return "gomoku";
}

bool GomokuBoard::overlinesWin(Side) const
{
	return false;
}

} // namespace Chess
