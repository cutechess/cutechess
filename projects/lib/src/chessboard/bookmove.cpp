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

#include "bookmove.h"


BookMove::BookMove()
	: m_promotion(0)
{
}

BookMove::BookMove(const Chess::Square& sourceSquare,
                   const Chess::Square& targetSquare,
                   int promotion)
	: m_sourceSquare(sourceSquare),
	  m_targetSquare(targetSquare),
	  m_promotion(promotion)
{

}

bool BookMove::operator==(const BookMove& move) const
{
	if (m_sourceSquare == move.m_sourceSquare
	&&  m_targetSquare == move.m_targetSquare
	&&  m_promotion == move.m_promotion)
		return true;
	return false;
}

const Chess::Square& BookMove::sourceSquare() const
{
	return m_sourceSquare;
}

const Chess::Square& BookMove::targetSquare() const
{
	return m_targetSquare;
}

int BookMove::promotion() const
{
	return m_promotion;
}
