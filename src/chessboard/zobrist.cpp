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

#include "zobrist.h"
#include "chess.h"

int Zobrist::m_randomSeed = 1;
bool Zobrist::m_isInitialized = false;
quint64 Zobrist::m_side;
quint64 Zobrist::m_enpassant[MaxSquares];
quint64 Zobrist::m_castling[2][MaxSquares];
quint64 Zobrist::m_piece[2][MaxPieceTypes][MaxSquares];


/*! \cond ZobristInitializer. */
class ZobristInitializer
{
	public:
		ZobristInitializer()
		{
			Zobrist::initialize();
		}
};
/*! \endcond ZobristInitializer. */
static ZobristInitializer zobristInitializer;


int Zobrist::random()
{
	const int a = 16807;
	const int m = 2147483647;
	const int q = (m / a);
	const int r = (m % a);

	int hi = m_randomSeed / q;
	int lo = m_randomSeed % q;
	int test = a * lo - r * hi;

	if (test > 0)
		m_randomSeed = test;
	else
		m_randomSeed = test + m;

	return m_randomSeed;
}

quint64 Zobrist::random64()
{
	quint64 random1 = (quint64)random();
	quint64 random2 = (quint64)random();
	quint64 random3 = (quint64)random();

	return random1 ^ (random2 << 31) ^ (random3 << 62);
}

quint64 Zobrist::side()
{
	return m_side;
}

quint64 Zobrist::enpassant(int square)
{
	Q_ASSERT(square >= 0 && square < MaxSquares);
	
	return m_enpassant[square];
}

quint64 Zobrist::castling(int side, int square)
{
	Q_ASSERT(side != -1);
	Q_ASSERT(square >= 0 && square < MaxSquares);
	
	return m_castling[side][square];
}

quint64 Zobrist::piece(int side, int type, int square)
{
	Q_ASSERT(side != -1);
	Q_ASSERT(type > 0);
	Q_ASSERT(square >= 0 && square < MaxSquares);
	
	return m_piece[side][type][square];
}

void Zobrist::initialize()
{
	if (m_isInitialized)
		return;
	
	int side;
	int square;
	
	m_side = random64();
	for (side = 0; side < 2; side++) {
		for (square = 0; square < MaxSquares; square++) {
			m_castling[side][square] = random64();
			
			for (int piece = 0; piece < MaxPieceTypes; piece++) {
				m_piece[side][piece][square] = random64();
			}
		}
	}
	for (square = 0; square < MaxSquares; square++)
		m_enpassant[square] = random64();
	
	m_isInitialized = true;
}
