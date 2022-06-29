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

#include "clobberboard.h"
#include "westernzobrist.h"


namespace Chess {

ClobberBoard::ClobberBoard(int width, int height)
	: WesternBoard(new WesternZobrist()),
	  m_width(width),
	  m_height(height)
{
	setPieceType(Stone, tr("stone"), "P");
	setPieceType(Knight, tr("no knight"), "");
	setPieceType(Bishop, tr("no bishop"), "");
	setPieceType(Rook, tr("no rool"), "");
	setPieceType(Queen, tr("no queen"), "");
	setPieceType(King, tr("no king"), "");
}

Board* ClobberBoard::copy() const
{
	return new ClobberBoard(*this);
}

QString ClobberBoard::variant() const
{
	return "clobber";
}

QString ClobberBoard::defaultFenString() const
{
	return "PpPpP/pPpPp/PpPpP/pPpPp/PpPpP/pPpPp w - - 0 1";
}

int ClobberBoard::width() const
{
	return m_width;
}

int ClobberBoard::height() const
{
	return m_height;
}

bool ClobberBoard::hasCastling() const
{
	return false;
}

bool ClobberBoard::hasEnPassantCaptures() const
{
	return false;
}

bool ClobberBoard::kingsCountAssertion(int, int) const
{
	return true;
}

void ClobberBoard::vInitialize()
{
	WesternBoard::vInitialize();

	int arwidth = width() + 2;
	m_offsets.resize(4);
	m_offsets[0] = -arwidth;
	m_offsets[1] = -1;
	m_offsets[2] =  1;
	m_offsets[3] =  arwidth;
}

const QVarLengthArray<int>& ClobberBoard::targetOffsets() const
{
	return m_offsets;
}

void ClobberBoard::addPromotions(int, int, QVarLengthArray< Chess::Move >&) const
{
}

void ClobberBoard::generateMovesForPiece(QVarLengthArray<Move> & moves,
						   int pieceType,
						   int square) const
{
	if (pieceType != Stone)
		return;

	QVarLengthArray<Move> moves1;
	generateHoppingMoves(square, m_offsets, moves1);

	// Only capture moves allowed
	for (const auto& m: moves1)
	{
		if (captureType(m) == Stone)
			moves.append(m);
	}
}

bool ClobberBoard::inCheck(Side, int) const
{
	return false;
}

Result ClobberBoard::result()
{
	if (canMove())
		return Result();

	Side opp = sideToMove().opposite();
	QString str = tr("%1 wins").arg(opp.toString());

	return Result(Result::Win, opp, str);
}

QString ClobberBoard::sanMoveString(const Move& move)
{
	return WesternBoard::lanMoveString(move);
}


Clobber10Board::Clobber10Board() : ClobberBoard(10, 10) {}

Board* Clobber10Board::copy() const
{
	return new Clobber10Board(*this);
}

QString Clobber10Board::variant() const
{
	return "clobber10";
}

QString Clobber10Board::defaultFenString() const
{
	return "PpPpPpPpPp/pPpPpPpPpP/PpPpPpPpPp/pPpPpPpPpP/PpPpPpPpPp/pPpPpPpPpP/"
	       "PpPpPpPpPp/pPpPpPpPpP/PpPpPpPpPp/pPpPpPpPpP w - - 0 1";
}


CannibalClobberBoard::CannibalClobberBoard(int width, int height)
	: ClobberBoard(width, height)
{
}

Board* CannibalClobberBoard::copy() const
{
	return new CannibalClobberBoard(*this);
}

QString CannibalClobberBoard::variant() const
{
	return "cannibalclobber";
}

void CannibalClobberBoard::generateMovesForPiece(QVarLengthArray<Chess::Move>& moves,
						 int pieceType,
						 int square) const
{
	if (pieceType != Stone)
		return;

	const QVarLengthArray<int>& offsets = targetOffsets();
	for (int i = 0; i < offsets.size(); i++)
	{
		int targetSquare = square + offsets.at(i);
		if (!isValidSquare(chessSquare(targetSquare)))
			continue;
		Piece capture = pieceAt(targetSquare);
		if (!capture.isEmpty())
			moves.append(Move(square, targetSquare));
	}
}


CannibalClobber10Board::CannibalClobber10Board() : CannibalClobberBoard(10, 10)
{
}

Board* CannibalClobber10Board::copy() const
{
	return new CannibalClobber10Board(*this);
}

QString CannibalClobber10Board::variant() const
{
	return "cannibalclobber10";
}

QString CannibalClobber10Board::defaultFenString() const
{
	return "PpPpPpPpPp/pPpPpPpPpP/PpPpPpPpPp/pPpPpPpPpP/PpPpPpPpPp/pPpPpPpPpP/"
	       "PpPpPpPpPp/pPpPpPpPpP/PpPpPpPpPp/pPpPpPpPpP w - - 0 1";
}

} // namespace Chess
