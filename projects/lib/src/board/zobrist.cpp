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

#include "zobrist.h"
#include <QVarLengthArray>
#include <QMutex>
#include <QMutexLocker>
#include "piece.h"

namespace {

QVarLengthArray<quint64, 0x2000> s_keys;
QMutex s_mutex;

} // anonymous namespace

namespace Chess {

int Zobrist::s_randomSeed = 1;

/*!
 * The "minimal standard" random number generator
 * by Park and Miller.
 * Returns a pseudo-random integer between 1 and 2147483646.
 */
int Zobrist::random32()
{
	const int a = 16807;
	const int m = 2147483647;
	const int q = (m / a);
	const int r = (m % a);

	int hi = s_randomSeed / q;
	int lo = s_randomSeed % q;
	int test = a * lo - r * hi;

	if (test > 0)
		s_randomSeed = test;
	else
		s_randomSeed = test + m;

	return s_randomSeed;
}


Zobrist::Zobrist(const quint64* keys)
	: m_initialized(false),
	  m_squareCount(0),
	  m_pieceTypeCount(0),
	  m_keys(keys)
{
}

bool Zobrist::isInitialized() const
{
	return m_initialized;
}

void Zobrist::initialize(int squareCount,
			 int pieceTypeCount)
{
	Q_ASSERT(squareCount > 0);
	Q_ASSERT(pieceTypeCount > 1);

	QMutexLocker locker(&s_mutex);

	if (m_initialized)
		return;

	m_squareCount = squareCount;
	m_pieceTypeCount = pieceTypeCount;

	if (m_keys == nullptr)
	{
		// Initialize the global zobrist array
		if (s_keys.isEmpty())
		{
			for (int i = 0; i < s_keys.capacity(); i++)
				s_keys.append(random64());
		}
		m_keys = s_keys.constData();
	}
	m_initialized = true;
}

quint64 Zobrist::side() const
{
	return m_keys[0];
}

quint64 Zobrist::piece(const Piece& piece, int square) const
{
	Q_ASSERT(piece.isValid());
	Q_ASSERT(piece.type() >= 0 && piece.type() < m_pieceTypeCount);
	Q_ASSERT(square >= 0 && square < m_squareCount);

	int i = 1 + m_squareCount * m_pieceTypeCount * piece.side() +
		piece.type() * m_squareCount + square;
	return m_keys[i];
}

quint64 Zobrist::reservePiece(const Piece& piece, int slot) const
{
	Q_ASSERT(slot >= 0);

	// HACK: Use the "wall" squares (0...n) as slots
	// for hand pieces.
	return this->piece(piece, slot);
}

quint64 Zobrist::random64()
{
	quint64 random1 = (quint64)random32();
	quint64 random2 = (quint64)random32();
	quint64 random3 = (quint64)random32();

	return random1 ^ (random2 << 31) ^ (random3 << 62);
}

} // namespace Chess
