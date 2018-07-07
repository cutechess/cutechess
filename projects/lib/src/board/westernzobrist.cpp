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

#include "westernzobrist.h"
#include <QMutexLocker>
#include "piece.h"


namespace Chess {

WesternZobrist::WesternZobrist(const quint64* keys)
	: Zobrist(keys),
	  m_castlingIndex(0),
	  m_pieceIndex(0)
{
}

void WesternZobrist::initialize(int squareCount,
				int pieceTypeCount)
{
	QMutexLocker locker(&m_mutex);

	if (isInitialized())
		return;

	Zobrist::initialize(squareCount, pieceTypeCount);

	m_castlingIndex = 1 + squareCount;
	m_pieceIndex = m_castlingIndex + squareCount * 2;
}

quint64 WesternZobrist::side() const
{
	return keys()[0];
}

quint64 WesternZobrist::piece(const Piece& piece, int square) const
{
	Q_ASSERT(piece.isValid());
	Q_ASSERT(piece.type() >= 0 && piece.type() < pieceTypeCount());
	Q_ASSERT(square >= 0 && square < squareCount());

	int i = m_pieceIndex + squareCount() * pieceTypeCount() * piece.side() +
		piece.type() * squareCount() + square;
	return keys()[i];
}

quint64 WesternZobrist::enpassant(int square) const
{
	Q_ASSERT(square >= 0 && square < squareCount());

	return keys()[1 + square];
}

quint64 WesternZobrist::castling(int side, int square) const
{
	Q_ASSERT(side != Side::NoSide);
	Q_ASSERT(square >= 0 && square < squareCount());

	return keys()[m_castlingIndex + squareCount() * side + square];
}

} // namespace Chess
