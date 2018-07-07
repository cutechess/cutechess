/*
    This file is part of Cute Chess.
    Copyright (C) 2008-2018 Cute Chess authors

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

#include "square.h"


namespace Chess {

Square::Square()
	: m_file(-1),
	  m_rank(-1)
{
}

Square::Square(int file, int rank)
	: m_file(file),
	  m_rank(rank)
{
}

bool Square::operator==(const Square& other) const
{
	return (m_file == other.m_file) && (m_rank == other.m_rank);
}

bool Square::operator!=(const Square& other) const
{
	return (m_file != other.m_file) || (m_rank != other.m_rank);
}

bool Square::isValid() const
{
	return (m_file >= 0) && (m_rank >= 0);
}

int Square::file() const
{
	return m_file;
}

int Square::rank() const
{
	return m_rank;
}

Square::Color Square::color() const
{
	if (!isValid())
		return NoColor;
	if ((m_file % 2) != (m_rank % 2))
		return Light;
	return Dark;
}

void Square::setFile(int file)
{
	m_file = file;
}

void Square::setRank(int rank)
{
	m_rank = rank;
}

} // namespace Chess
