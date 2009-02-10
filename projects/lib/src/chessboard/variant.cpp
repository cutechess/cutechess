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

#include "variant.h"
#include <QtGlobal>
#include <QVector>
#include "chess.h"
#include "notation.h"
using namespace Chess;


Variant::Variant(Code code)
	: m_code(code)
{
}

Variant::Variant(const QString& str)
{
	if (str == "Standard")
		m_code = Standard;
	else if (str == "Capablanca")
		m_code = Capablanca;
	else if (str == "Gothic")
		m_code = Gothic;
	else if (str == "Fischerandom")
		m_code = Fischerandom;
	else if (str == "Caparandom")
		m_code = Caparandom;
	else
		m_code = NoVariant;
}

bool Variant::operator==(const Variant& other) const
{
	return (m_code == other.m_code);
}

bool Variant::operator==(Code otherCode) const
{
	return (m_code == otherCode);
}

bool Variant::operator!=(const Variant& other) const
{
	return (m_code != other.m_code);
}

bool Variant::operator!=(Code otherCode) const
{
	return (m_code != otherCode);
}

Variant::Code Variant::code() const
{
	return m_code;
}

bool Variant::isNone() const
{
	return (m_code == NoVariant);
}

bool Variant::isRandom() const
{
	switch (m_code)
	{
	case Fischerandom:
	case Caparandom:
		return true;
	default:
		return false;
	}
}

bool Variant::isStandard() const
{
	switch (m_code)
	{
	case Standard:
	case Fischerandom:
		return true;
	default:
		return false;
	}
}

bool Variant::isCapablanca() const
{
	switch (m_code)
	{
	case Capablanca:
	case Gothic:
	case Caparandom:
		return true;
	default:
		return false;
	}
}

int Variant::boardWidth() const
{
	switch (m_code)
	{
	case Standard:
	case Fischerandom:
		return 8;
	case Capablanca:
	case Gothic:
	case Caparandom:
		return 10;
	default:
		return 0;
	}
}

int Variant::boardHeight() const
{
	switch (m_code)
	{
	case Standard:
	case Fischerandom:
		return 8;
	case Capablanca:
	case Gothic:
	case Caparandom:
		return 8;
	default:
		return 0;
	}
}

QString Variant::toString() const
{
	switch (m_code)
	{
	case Standard:
		return "Standard";
	case Capablanca:
		return "Capablanca";
	case Gothic:
		return "Gothic";
	case Fischerandom:
		return "Fischerandom";
	case Caparandom:
		return "Caparandom";
	default:
		return "Unknown";
	}
}

static void addPiece(QVector<int>& pieces,
                     int piece,
                     int pos,
                     int start = 0,
                     int step = 1)
{
	int i = 0;
	for (int j = start; j < pieces.size(); j += step)
	{
		if (pieces[j] != 0)
			continue;
		if (i == pos)
		{
			pieces[j] = piece;
			break;
		}
		i++;
	}
}

static QVector<int> fischerandomVector()
{
	QVector<int> pieces(8, 0);
	
	addPiece(pieces, Chess::Bishop, qrand() % 4, 0, 2);
	addPiece(pieces, Chess::Bishop, qrand() % 4, 1, 2);
	addPiece(pieces, Chess::Queen, qrand() % 6);
	addPiece(pieces, Chess::Knight, qrand() % 5);
	addPiece(pieces, Chess::Knight, qrand() % 4);
	addPiece(pieces, Chess::Rook, 0);
	addPiece(pieces, Chess::King, 0);
	addPiece(pieces, Chess::Rook, 0);
	
	return pieces;
}

static QVector<int> caparandomVector()
{
	QVector<int> pieces(10, 0);

	if ((qrand() % 2) == 0)
	{
		addPiece(pieces, Chess::Queen, qrand() % 5, 0, 2);
		addPiece(pieces, Chess::Archbishop, qrand() % 5, 1, 2);
	}
	else
	{
		addPiece(pieces, Chess::Archbishop, qrand() % 5, 0, 2);
		addPiece(pieces, Chess::Queen, qrand() % 5, 1, 2);
	}
	addPiece(pieces, Chess::Bishop, qrand() % 4, 0, 2);
	addPiece(pieces, Chess::Bishop, qrand() % 4, 1, 2);
	addPiece(pieces, Chess::Chancellor, qrand() % 6);
	addPiece(pieces, Chess::Knight, qrand() % 5);
	addPiece(pieces, Chess::Knight, qrand() % 4);
	addPiece(pieces, Chess::Rook, 0);
	addPiece(pieces, Chess::King, 0);
	addPiece(pieces, Chess::Rook, 0);
	
	// TODO: Check for unprotected pawns

	return pieces;
}

static QString randomFen(const QVector<int>& pieces)
{
	QString fen;
	
	// Black pieces
	foreach (int piece, pieces)
		fen += Notation::pieceChar(-piece);
	fen += '/';
	// Black pawns
	fen += QString(pieces.size(), Notation::pieceChar(-Chess::Pawn));
	fen += '/';
	
	// Empty squares
	for (int i = 0; i < 4; i++)
		fen += QString::number(pieces.size()) + '/';
	
	// White pawns
	fen += QString(pieces.size(), Notation::pieceChar(Chess::Pawn));
	// White pieces
	foreach (int piece, pieces)
		fen += Notation::pieceChar(piece);
	fen += '/';

	// Side to move, castling rights, enpassant square, etc.
	fen += " w KQkq - 0 1";
	
	return fen;
}

QString Variant::startingFen() const
{
	switch (m_code)
	{
	case Standard:
		return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	case Capablanca:
		return "rnabqkbcnr/pppppppppp/10/10/10/10/PPPPPPPPPP/RNABQKBCNR w KQkq - 0 1";
	case Gothic:
		return "rnbqckabnr/pppppppppp/10/10/10/10/PPPPPPPPPP/RNBQCKABNR w KQkq - 0 1";
	case Fischerandom:
		return randomFen(fischerandomVector());
	case Caparandom:
		return randomFen(caparandomVector());
	default:
		return "";
	}
}
