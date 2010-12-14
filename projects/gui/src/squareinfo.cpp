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

#include "squareinfo.h"

SquareInfo::SquareInfo()
	: m_flags(NormalSquare),
	  m_color(BackgroundColor),
	  m_pieceCount(0)
{
}

SquareInfo::SquareInfo(TypeFlags flags,
		       Color color,
		       int pieceCount,
		       const QString& pieceSymbol)
	: m_flags(flags),
	  m_color(color),
	  m_pieceCount(pieceCount),
	  m_pieceSymbol(pieceSymbol)
{
	Q_ASSERT(pieceCount >= 0);
}

SquareInfo::TypeFlags SquareInfo::flags() const
{
	return m_flags;
}

SquareInfo::Color SquareInfo::color() const
{
	return m_color;
}

int SquareInfo::pieceCount() const
{
	return m_pieceCount;
}

QString SquareInfo::pieceSymbol() const
{
	return m_pieceSymbol;
}

void SquareInfo::setFlags(TypeFlags flags)
{
	m_flags = flags;
}

void SquareInfo::setColor(Color color)
{
	m_color = color;
}

void SquareInfo::setPieceCount(int count)
{
	Q_ASSERT(count >= 0);
	m_pieceCount = count;
}

void SquareInfo::setPieceSymbol(const QString& symbol)
{
	m_pieceSymbol = symbol;
}
