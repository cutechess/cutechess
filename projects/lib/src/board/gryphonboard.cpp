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

#include "gryphonboard.h"
#include "westernzobrist.h"

namespace Chess {

GryphonBoard::GryphonBoard()
	: WesternBoard(new WesternZobrist()),
	  m_start(0),
	  m_end(120),
	  m_pieceStack()
{
}

Board* GryphonBoard::copy() const
{
	return new GryphonBoard(*this);
}

QString GryphonBoard::variant() const
{
	return "gryphon";
}

QString GryphonBoard::defaultFenString() const
{
	return "rnbq1bnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQ1BNR w - - 0 1";
}

void GryphonBoard::vInitialize()
{
	WesternBoard::vInitialize();

	m_start = (width() + 2) * 2;
	m_end = arraySize() - m_start;
}

bool GryphonBoard::kingsCountAssertion(int whiteKings, int blackKings) const
{
	return whiteKings <= 15 && blackKings <= 15;
}

bool GryphonBoard::inCheck(Side side, int square) const
{
	if (square != 0)
		return WesternBoard::inCheck(side, square);

	const Piece kingPiece(side, King);

	// test whether any king is in check
	for (int sq = m_start; sq < m_end; sq++)
	{
		if (pieceAt(sq) == kingPiece
		&&  WesternBoard::inCheck(side, sq))
			return true;
	}
	return false;
}

int GryphonBoard::successorType(int type, bool reversed) const
{
	static const QList<int> types = {Pawn, Knight, Bishop, Rook, Queen, King};
	static const int length = types.length();

	int index = types.indexOf(type);
	index = reversed ? index - 1 : index + 1;
	index = qMin( qMax(0, index), length - 1);
	return types[index];
}

void GryphonBoard::vMakeMove(const Move& move, BoardTransition* transition)
{
	WesternBoard::vMakeMove(move, transition);

	int target = move.targetSquare();
	Piece piece = pieceAt(target);
	m_pieceStack.push_back(Piece(piece));

	if (move.promotion() != Piece::NoPiece)
		  return;

	int srcType = piece.type();
	int tgtType = successorType(srcType, false);

	if (srcType != tgtType)
	{
		piece.setType(tgtType);
		setSquare(target, piece);
	}
}

void GryphonBoard::vUndoMove(const Move& move)
{
	int target = move.targetSquare();
	Piece piece = pieceAt(target);
	Piece oldpiece = m_pieceStack.pop();
	Q_ASSERT(oldpiece.side() == piece.side());
	piece.setType(oldpiece.type());
	setSquare(target, piece);

	WesternBoard::vUndoMove(move);
}

bool GryphonBoard::isLegalPosition()
{
	if (!WesternBoard::isLegalPosition())
		return false;

	int count[2][King+1] = {{0, 15, 4, 4, 4, 2, 15},
				{0, 15, 4, 4, 4, 2, 15}};

	for (int sq = m_start; sq < m_end; sq++)
	{
		Piece piece = pieceAt(sq);
		if (piece.isValid()
		&&  --count[piece.side()][piece.type()] < 0)
			// too many pieces of this type
			return false;
	}

	return true;
}

void GryphonBoard::generateMovesForPiece(QVarLengthArray< Move >& moves,
					 int pieceType,
					 int square) const
{
	// return early if successor of pieceType is already fully occupied
	Side side = sideToMove();
	int successor = successorType(pieceType);
	static const int count[King+1] = {0, 15, 4, 4, 4, 2, 15};
	int cnt = count[successor];

	for (int sq = m_start; sq < m_end; sq++)
	{
		if (pieceAt(sq) == Piece(side, successor) && --cnt < 1)
			return;
	}

	WesternBoard::generateMovesForPiece(moves, pieceType, square);
}



SimplifiedGryphonBoard::SimplifiedGryphonBoard()
	: GryphonBoard(),
	  m_captures {0, 0}
{
}

Board* SimplifiedGryphonBoard::copy() const
{
	return new SimplifiedGryphonBoard(*this);
}

QString SimplifiedGryphonBoard::variant() const
{
	return "simplifiedgryphon";
}

QString SimplifiedGryphonBoard::defaultFenString() const
{
	return "4k3/pppppppp/8/8/8/8/PPPPPPPP/4K3 w - - 0 1";
}

void SimplifiedGryphonBoard::generateMovesForPiece(QVarLengthArray< Move >& moves,
						   int pieceType,
						   int square) const
{
	if (pieceType != King)
	{
		GryphonBoard::generateMovesForPiece(moves, pieceType, square);
		return;
	}

	QVarLengthArray< Move > newmoves;
	GryphonBoard::generateMovesForPiece(newmoves, King, square);

	for (const Move move: newmoves)
	{
		Square srcSq = chessSquare(move.sourceSquare());
		Square tgtSq = chessSquare(move.targetSquare());
		int rrank = tgtSq.rank() - srcSq.rank();

		if (sideToMove() == Side::Black)
			rrank = -rrank;

		// side without captures must not move king backward or sideways
		if (m_captures[sideToMove()] > 0 || rrank > 0)
			moves.append(move);
	}
}

void SimplifiedGryphonBoard::vMakeMove(const Move& move,
				       BoardTransition* transition)
{
	if (captureType(move) != Piece::NoPiece)
		m_captures[sideToMove()]++;

	GryphonBoard::vMakeMove(move, transition);
}

void SimplifiedGryphonBoard::vUndoMove(const Move& move)
{
	GryphonBoard::vUndoMove(move);

	if (captureType(move) != Piece::NoPiece)
		m_captures[sideToMove()]--;
}



CircularGryphonBoard::CircularGryphonBoard()
	: GryphonBoard()
{
}

Board* CircularGryphonBoard::copy() const
{
	return new CircularGryphonBoard(*this);
}

QString CircularGryphonBoard::variant() const
{
	return "circulargryphon";
}

QString CircularGryphonBoard::defaultFenString() const
{
	return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1";
}

bool CircularGryphonBoard::kingsCountAssertion(int whiteKings,
					       int blackKings) const
{
	return WesternBoard::kingsCountAssertion(whiteKings, blackKings);
}

bool CircularGryphonBoard::inCheck(Side side, int square) const
{
	return WesternBoard::inCheck(side, square);
}

int CircularGryphonBoard::successorType(int type, bool reversed) const
{
	static const QList<int> types = {Pawn, Knight, Bishop, Rook, Queen};
	static const int length = types.length();

	if (type == King)
		  return King;

	int index = types.indexOf(type);
	index = reversed ? index - 1 : index + 1;
	// circular index: prevent underflow and overflow
	index = (index + length) % length;
	return types[index];
}



ChangeOverBoard::ChangeOverBoard()
	: CircularGryphonBoard()
{
}

Board* ChangeOverBoard::copy() const
{
	return new ChangeOverBoard(*this);
}

QString ChangeOverBoard::defaultFenString() const
{
	return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1";
}

QString ChangeOverBoard::variant() const
{
	return "changeover";
}

int ChangeOverBoard::successorType(int type, bool reversed) const
{
	// Change-Over Chess does the changes the other way round
	return CircularGryphonBoard::successorType(type, !reversed);
}
/*
 * This variant does not have the restriction to two sets of regular pieces.
 */
void ChangeOverBoard::generateMovesForPiece(QVarLengthArray< Move >& moves,
					    int pieceType,
					    int square) const
{
	WesternBoard::generateMovesForPiece(moves, pieceType, square);
}

bool ChangeOverBoard::isLegalPosition()
{
	return WesternBoard::isLegalPosition();
}

} // namespace Chess
