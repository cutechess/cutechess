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

#include "chessmove.h"

using namespace Chess;

Move::Move()
	: m_sourceSquare(0),
	  m_targetSquare(0),
	  m_promotion(Piece::NoPiece),
	  m_castlingSide(-1)
{

}

Move::Move(int sourceSquare,
           int targetSquare,
	   Piece::Type promotion,
           int castlingSide)
	: m_sourceSquare(sourceSquare),
	  m_targetSquare(targetSquare),
	  m_promotion(promotion),
	  m_castlingSide(castlingSide)
{

}

bool Move::operator==(const Move &other) const
{
	return (other.sourceSquare() == m_sourceSquare &&
	        other.targetSquare() == m_targetSquare &&
	        other.promotion() == m_promotion &&
	        other.castlingSide() == m_castlingSide);
}

bool Move::isNull() const
{
	return (m_sourceSquare == 0 || m_targetSquare == 0);
}

int Move::sourceSquare() const
{
	return m_sourceSquare;
}

int Move::targetSquare() const
{
	return m_targetSquare;
}

Piece::Type Move::promotion() const
{
	return m_promotion;
}

int Move::castlingSide() const
{
	return m_castlingSide;
}
