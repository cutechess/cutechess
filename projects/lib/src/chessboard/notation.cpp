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

#include "notation.h"
#include <QtGlobal>
#include "chess.h"

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
