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

#include "piece.h"
#include <QChar>
using namespace Chess;


Piece::Piece(int code)
	: m_code(code)
{
}

Piece::Piece(Side side, Type type)
{
	Q_ASSERT(side != NoSide);
	Q_ASSERT(type != WallPiece);
	Q_ASSERT(type != NoPiece);

	if (side == White)
		m_code = type;
	else
		m_code = -type;
}

Piece::Piece(const QChar& c)
{
	Q_ASSERT(!c.isNull());

	switch (c.toUpper().toLatin1())
	{
	case 'P':
		m_code = Pawn;
		break;
	case 'N':
		m_code = Knight;
		break;
	case 'B':
		m_code = Bishop;
		break;
	case 'R':
		m_code = Rook;
		break;
	case 'Q':
		m_code = Queen;
		break;
	case 'K':
		m_code = King;
		break;
	case 'A':
		m_code = Archbishop;
		break;
	case 'C':
		m_code = Chancellor;
		break;
	default:
		m_code = NoPiece;
		return;
	}

	if (c.isLower())
		m_code = -m_code;
}

bool Piece::operator==(const Piece& other) const
{
	return m_code == other.m_code;
}

bool Piece::operator!=(const Piece& other) const
{
	return m_code != other.m_code;
}

bool Piece::isEmpty() const
{
	return m_code == NoPiece;
}

bool Piece::isValid() const
{
	return m_code != WallPiece && m_code != NoPiece;
}

bool Piece::isWall() const
{
	return m_code == WallPiece;
}

int Piece::code() const
{
	return m_code;
}

Side Piece::side() const
{
	if (!isValid())
		return NoSide;

	if (m_code > 0)
		return White;
	return Black;
}

Piece::Type Piece::type() const
{
	return (Type)qAbs(m_code);
}

QChar Piece::toChar() const
{
	QChar c;
	switch (type())
	{
	case Pawn:
		c = 'P';
		break;
	case Knight:
		c = 'N';
		break;
	case Bishop:
		c = 'B';
		break;
	case Rook:
		c = 'R';
		break;
	case Queen:
		c = 'Q';
		break;
	case King:
		c = 'K';
		break;
	case Archbishop:
		c = 'A';
		break;
	case Chancellor:
		c = 'C';
		break;
	default:
		return QChar();
	}

	if (side() == White)
		return c;
	return c.toLower();
}

QString Piece::internalName() const
{
	if (!isValid())
		return QString();

	QString s;
	if (side() == White)
		s = "w";
	else
		s = "b";

	switch (type())
	{
	case Pawn:
		return s + "pawn";
	case Knight:
		return s + "knight";
	case Bishop:
		return s + "bishop";
	case Rook:
		return s + "rook";
	case Queen:
		return s + "queen";
	case King:
		return s + "king";
	case Archbishop:
		return s + "archbishop";
	case Chancellor:
		return s + "chancellor";
	default:
		return QString();
	}
}

QString Piece::symbol() const
{
	if (side() == White)
	{
		switch (type())
		{
		case Pawn:
			return QString::fromUtf8("\u2659");
		case Knight:
			return QString::fromUtf8("\u2658");
		case Bishop:
			return QString::fromUtf8("\u2657");
		case Rook:
			return QString::fromUtf8("\u2656");
		case Queen:
			return QString::fromUtf8("\u2655");
		case King:
			return QString::fromUtf8("\u2654");
		case Archbishop:
			return "A";
		case Chancellor:
			return "C";
		default:
			return QString();
		}
	}
	else if (side() == Black)
	{
		switch (type())
		{
		case Pawn:
			return QString::fromUtf8("\u265f");
		case Knight:
			return QString::fromUtf8("\u265e");
		case Bishop:
			return QString::fromUtf8("\u265d");
		case Rook:
			return QString::fromUtf8("\u265c");
		case Queen:
			return QString::fromUtf8("\u265b");
		case King:
			return QString::fromUtf8("\u265a");
		case Archbishop:
			return "A";
		case Chancellor:
			return "C";
		default:
			return QString();
		}
	}

	return QString();
}

void Piece::setSide(Side side)
{
	if (side == NoSide)
		m_code = NoPiece;
	else if (side == White || !isValid())
		m_code = type();
	else
		m_code = -type();
}

void Piece::setType(Type type)
{
	if (type == NoPiece || type == WallPiece || side() != Black)
		m_code = type;
	else
		m_code = -type;
}
