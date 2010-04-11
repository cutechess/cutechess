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
	: m_color(BackgroundColor),
	  m_pieceCount(0)
{
}

SquareInfo::SquareInfo(Color color,
		       int pieceCount,
		       const QString& pieceSymbol)
	: m_color(color),
	  m_pieceCount(pieceCount),
	  m_pieceSymbol(pieceSymbol)
{
	Q_ASSERT(pieceCount >= 0);
}

bool SquareInfo::isValid() const
{
	return (m_color != NoColor &&
		m_pieceCount >= 0 &&
		((m_pieceCount == 0) == (m_pieceSymbol.isEmpty())));
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
