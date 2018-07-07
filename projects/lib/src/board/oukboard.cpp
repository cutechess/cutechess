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

#include "oukboard.h"

namespace Chess {

OukBoard::OukBoard()
	: MakrukBoard(),
	  m_initialSquare{{{King, 94},{Maiden, 95}},{{King, 25},{Maiden, 24}}},
	  m_initialOffsets{{King, 8}, {King, 12}, {Maiden, 20}},
	  m_moveCount{}
{
	// Movements as in chaturanga and shatranj, except Alfil -> Koul (Base, Pillar)
	setPieceType(Fish, tr("trey"), "P");                       //! Pawn
	setPieceType(Horse, tr("ses"), "N", KnightMovement);       //! Knight
	setPieceType(Pillar, tr("kol"), "S", SilverGeneralMovement, "E"); //! replaces Alfil
	setPieceType(Boat, tr("tuuk"), "R", RookMovement);         //! Rook
	setPieceType(Maiden, tr("neang"), "M", FerzMovement, "F"); //! Queen, Counselor, Mantri, Met
	setPieceType(King, tr("sdaach"), "K");                     //! King
}

Board* OukBoard::copy() const
{
	return new OukBoard(*this);
}

QString OukBoard::variant() const
{
	return "cambodian";
}

QString OukBoard::defaultFenString() const
{
	return "rnsmksnr/8/pppppppp/8/8/PPPPPPPP/8/RNSKMSNR w DEde 0 0 1";
}

MakrukBoard::CountingRules OukBoard::countingRules() const
{
	return BareKing;
}

/*!
 * Ouk Chatrang has special initial moves for King (Ang) and Maiden (Neang).
 * The initial files of unmoved Kings and Maidens are listed in the castling
 * field of the FEN. So it is status "DEde" for the starting position.
 */
QString OukBoard::vFenString(Board::FenNotation notation) const
{
	const QString vfs = MakrukBoard::vFenString(notation);

	// Set castling rights string
	QString castlingStr;
	if (m_moveCount[Side::White][King] == 0)
		  castlingStr.append('D');
	if (m_moveCount[Side::White][Maiden] == 0)
		  castlingStr.append('E');
	if (m_moveCount[Side::Black][Maiden] == 0)
		  castlingStr.append('d');
	if (m_moveCount[Side::Black][King] == 0)
		  castlingStr.append('e');

	QStringList vfen = vfs.split(' ', QString::SkipEmptyParts);

	// Append the rest of the FEN string
	QString s = (!castlingStr.isEmpty() ? castlingStr : "-");
	for (int i = 1; i < vfen.count(); i++)
		s.append(" ").append(vfen.at(i));

	return s;
}

bool OukBoard::parseCastlingRights(QChar c)
{
	if (c == '-')
		return true;

	const QString tokens("DEde");
	if (tokens.indexOf(c) < 0)
		return false;

	// Enable initial moves by clearing matching move count
	if (c == 'D')
		m_moveCount[Side::White][King] = 0;
	else if (c == 'E')
		m_moveCount[Side::White][Maiden] = 0;
	else if (c == 'd')
		m_moveCount[Side::Black][Maiden] = 0;
	else if (c == 'e')
		m_moveCount[Side::Black][King] = 0;

	return true;
}

bool OukBoard::vSetFenString(const QStringList& fen)
{
	// At first assume that King and Maiden pieces have moved
	m_moveCount[Side::White][King] = 1;
	m_moveCount[Side::Black][King] = 1;
	m_moveCount[Side::White][Maiden] = 1;
	m_moveCount[Side::Black][Maiden] = 1;

	// parseCastlingRights is called via vSetFenString of base class
	if (!MakrukBoard::vSetFenString(fen))
		return false;

	// Expect pieces on their initial squares if move counter is zero
	const OukPieceType types[2]{King, Maiden};
	for (int side = Side::White; side <= Side::Black; side++)
	{
		for (auto type: types)
		{
			int index = m_initialSquare[side][type];
			if ( m_moveCount[side][type] == 0
			&&   pieceAt(index) != Piece(Side::Type(side), type))
				return false;
		}
	}
	return true;
}

void OukBoard::generateMovesForPiece(QVarLengthArray< Move >& moves,
					  int pieceType,
					  int square) const
{
	MakrukBoard::generateMovesForPiece(moves, pieceType, square);

	Side side = sideToMove();

	// Only consider King and Neang on their initial squares
	if ((pieceType != King || square != m_initialSquare[side][King])
	&&  (pieceType != Maiden || square != m_initialSquare[side][Maiden]))
		return;
	// Return if the piece has moved already
	if (m_moveCount[side].value((OukPieceType)pieceType, -1) != 0)
		return;
	// No special moves for King in check
	if (pieceType == King && inCheck(side))
		return;

	// Generate initial move option for King (leap of Horse)
	// and for Neang (forward two squares)
	int sign = (side == Side::White) ? 1 : -1;

	for (const auto& i: m_initialOffsets)
	{
		if (pieceType != i.type)
			continue;

		int target = square - i.offset * sign;
		const Piece& piece = pieceAt(target);

		if  (piece.isEmpty())
			moves.append(Move(square, target));
	}
}

bool OukBoard::inCheck(Side side, int square) const
{
	int sign = (side == Side::White) ? 1 : -1;
	Side opSide = side.opposite();
	if (square == 0)
		square = kingSquare(side);

	// Attack by Advisor (Maiden, Neang), initial move
	if (!m_moveCount[opSide][Maiden]
	&&  m_initialSquare[opSide][Maiden] == square - 2 * sign * (width() + 2))
		return true;

	// Attack by King, initial move
	int ksq = kingSquare(opSide);
	bool attacked = (ksq == square - 8 * sign || ksq == square - 12 * sign);

	if (!m_moveCount[opSide][King] && attacked)
	{
		if (square == kingSquare(side)
		|| !inCheck(opSide))
			return true;
	}
	return MakrukBoard::inCheck(side, square);
}

void OukBoard::updateCounter(Move m, int increment)
{
	Side side = sideToMove();
	int source = m.sourceSquare();
	int target= m.targetSquare();

	if (source == m_initialSquare[side][King]
	||  target == m_initialSquare[side][King])
		m_moveCount[side][King] += increment;

	if (source == m_initialSquare[side][Maiden]
	||  target == m_initialSquare[side][Maiden])
		m_moveCount[side][Maiden] += increment;
}

void OukBoard::vMakeMove(const Move& move, BoardTransition* transition)
{
	MakrukBoard::vMakeMove(move, transition);
	updateCounter(move, +1);
}

void OukBoard::vUndoMove(const Move& move)
{
	updateCounter(move, -1);
	MakrukBoard::vUndoMove(move);
}



KarOukBoard::KarOukBoard(): OukBoard(){}

Board* KarOukBoard::copy() const
{
	return new KarOukBoard(*this);
}

QString KarOukBoard::variant() const
{
	return "karouk";
}

QString KarOukBoard::defaultFenString() const
{
	return "rnsmksnr/8/pppppppp/8/8/PPPPPPPP/8/RNSKMSNR w DEde 0 0 1";
}

Result KarOukBoard::result()
{
	Side side = sideToMove();
	if (!inCheck(side))
		return OukBoard::result();

	Side opp = side.opposite();
	QString str = tr("%1 wins by giving check").arg(opp.toString());
	return Result(Result::Win, opp, str);
}

} // namespace Chess
