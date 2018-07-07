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

#include "boardtransition.h"

namespace Chess {

BoardTransition::BoardTransition()
{
}

bool BoardTransition::isEmpty() const
{
	return (m_moves.isEmpty() &&
		m_drops.isEmpty() &&
		m_squares.isEmpty() &&
		m_reserve.isEmpty());
}

void BoardTransition::clear()
{
	m_moves.clear();
	m_drops.clear();
	m_squares.clear();
	m_reserve.clear();
}

QList<BoardTransition::Move> BoardTransition::moves() const
{
	return m_moves;
}

QList<BoardTransition::Drop> BoardTransition::drops() const
{
	return m_drops;
}

QList<Square> BoardTransition::squares() const
{
	return m_squares;
}

QList<Piece> BoardTransition::reserve() const
{
	return m_reserve;
}

void BoardTransition::addMove(const Square& source, const Square& target)
{
	Move move = { source, target };
	m_moves.append(move);

	addSquare(source);
	addSquare(target);
}

void BoardTransition::addDrop(const Piece& piece, const Square& target)
{
	Drop drop = { piece, target };
	m_drops.append(drop);

	addSquare(target);
	addReservePiece(piece);
}

void BoardTransition::addSquare(const Square& square)
{
	if (!m_squares.contains(square))
		m_squares.append(square);
}

void BoardTransition::addReservePiece(const Piece& piece)
{
	if (!m_reserve.contains(piece))
		m_reserve.append(piece);
}

} // namespace Chess
