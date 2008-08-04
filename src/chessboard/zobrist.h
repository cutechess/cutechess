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

#ifndef ZOBRIST_H
#define ZOBRIST_H

#include <QtGlobal>


/**
 * Random 64-bit values for generating Zobrist hash keys.
 */
typedef struct _Zobrist
{
	/** Zobrist value to hash the color. */
	quint64 color;
	/** Zobrist values to hash the en passant square. */
	quint64 enpassant[64];
	/** Zobrist values to hash the castling rights. */
	quint64 castle[2][2];
	/** Zobrist values to hash the piece types and positions. */
	quint64 piece[2][8][64];
} Zobrist;


/**
 * Values for everything that's needed to create a hash key (side to move,
 * enpassant square, castling rights, and piece positions).
 */
extern Zobrist zobrist;

#endif // ZOBRIST_H

