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

#include "chessmove.h"

using Chess::Move;

Move::Move()
	: m_sourceSquare(0),
	  m_targetSquare(0),
	  m_promotion(0),
	  m_castlingSide(-1)
{

}

Move::Move(int sourceSquare,
           int targetSquare,
           int promotion,
           int castlingSide)
	: m_sourceSquare(sourceSquare),
	  m_targetSquare(targetSquare),
	  m_promotion(promotion),
	  m_castlingSide(castlingSide)
{

}

int Move::sourceSquare() const
{
	return m_sourceSquare;
}

int Move::targetSquare() const
{
	return m_targetSquare;
}

int Move::promotion() const
{
	return m_promotion;
}

int Move::castlingSide() const
{
	return m_castlingSide;
}
