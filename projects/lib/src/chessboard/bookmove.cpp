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

#include "bookmove.h"


BookMove::BookMove()
{
	m_sourceSquare.file = -1;
	m_sourceSquare.rank = -1;
	m_targetSquare.file = -1;
	m_targetSquare.rank = -1;
	m_promotion = 0;
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
	if (m_sourceSquare.file == move.sourceSquare().file
	&&  m_sourceSquare.rank == move.sourceSquare().rank
	&&  m_targetSquare.file == move.targetSquare().file
	&&  m_targetSquare.rank == move.targetSquare().rank
	&&  m_promotion == move.promotion())
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
