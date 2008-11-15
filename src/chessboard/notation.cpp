/*
    This file is part of SloppyGUI.

    SloppyGUI is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SloppyGUI is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SloppyGUI.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "chessboard.h"
#include "notation.h"

using namespace Chess;


namespace {

QChar fileChar(int file)
{
	return QChar('a' + file);
}

QChar rankChar(int rank)
{
	return QChar('1' + rank);
}

int fileInt(const QChar& c)
{
	return c.toAscii() - 'a';
}

int rankInt(const QChar& c)
{
	return c.toAscii() - '1';
}

} // unnamed namespace


namespace Notation {

QChar pieceChar(int pieceCode)
{
	QChar c;
	
	switch (abs(pieceCode)) {
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
		return '\0';
	}
	
	if (pieceCode < 0)
		return c.toLower();
	return c;
}

int pieceCode(const QChar& pieceChar)
{
	int p;
	
	switch (pieceChar.toUpper().toAscii()) {
	case 'P':
		p = Pawn;
		break;
	case 'N':
		p = Knight;
		break;
	case 'B':
		p = Bishop;
		break;
	case 'R':
		p = Rook;
		break;
	case 'Q':
		p = Queen;
		break;
	case 'K':
		p = King;
		break;
	case 'A':
		p = Archbishop;
		break;
	case 'C':
		p = Chancellor;
		break;
	default:
		return NoPiece;
	}
	
	if (pieceChar.isLower())
		return -p;
	return p;
}

QString squareString(const Square& square)
{
	QString str;
	str += fileChar(square.file);
	str += rankChar(square.rank);
	return str;
}

Square square(const QString& str)
{
	Square sq = { -1, -1 };
	if (str.length() != 2)
		return sq;
	
	sq.file = fileInt(str[0]);
	sq.rank = rankInt(str[1]);
	
	return sq;
}

} // namespace Notation
