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

#ifndef UTIL_H
#define UTIL_H

#include <QtGlobal>


// Defined constants
#define NULLMOVE	00		// an empty move or a null move
#define MOVE_ERROR	01


/* Get a 32-bit value with bit x set.  */
#define BIT(x) ((1) << (x))

#define SIGN(color) ((color) == Chessboard::White ? 1 : -1)

#define SQ_FILE(square) ((square) & 7)	/* get the file (column) of a ChessSquare */
#define SQ_RANK(square) ((square) / 8)	/* get the rank (row) of a ChessSquare */


/* Macros for extracting components from a Move.  */
#define GET_FROM(a)	(a & 0000000077)		/* "from" square */
#define GET_TO(a)	((a & 0000007700) >> 6)		/* "to" square */
#define GET_PC(a)	((a & 0000070000) >> 12)	/* moving piece */
#define GET_CAPT(a)	((a & 0000700000) >> 15)	/* captured piece */
#define GET_PROM(a)	((a & 0007000000) >> 18)	/* promotion piece */
#define GET_EPSQ(a)	((a & 0770000000) >> 21)	/* en passant square */
#define IS_CASTLING(a)	((a & 01000000000) >> 27)
#define GET_CASTLE(a)	((a & 02000000000) >> 28)	/* castling type */
#define IS_CHECK(a)	((a & 04000000000) >> 29)


/**
 * Castling masks and squares for move generation, castle rights, etc.
 * Defined and declared in util because these are used in so many places.
 */
typedef struct _Castling
{
	/**
	 * King's source and target squares in castling moves.
	 * USAGE: kingSquares[color][castling side][C_FROM / C_TO]
	 */
	int kingSquares[2][2][2];
	/**
	 * Rook's source and target squares in castling moves.
	 * USAGE: rookSquares[color][castling side][C_FROM / C_TO]
	 */
	int rookSquares[2][2][2];
	/**
	 * Castling rights masks.
	 * USAGE: rights[color][castling side]
	 */
	unsigned rights[2][2];
	/**
	 * Castling rights masks where both castling sides are combined
	 * into one mask.
	 * USAGE: allRights[color]
	 */
	unsigned allRights[2];
} Castling;

// Castling array indeces
#define C_KSIDE 0	// kingside castle
#define C_QSIDE 1	// queenside castle
#define C_FROM 0	// "from" square for king or rook
#define C_TO 1		// "to" square for king or rook
extern const Castling castling;

// An array of bitmasks where each mask has one bit set
extern const quint64 bit64[64];


// Returns true if <square> is a valid square index
extern bool isOnBoard(int square);

// Returns a pseudo-random integer between 1 and 2147483646
extern int myRand(void);

// Initialize the random number generator with a new seed
extern void setRandSeed(int new_seed);

// Locates the first (least significant) "one" bit in a bitboard
extern int getLsb(quint64 b);

// Same as getLsb(), but also clears the first bit in *b
extern int popLsb(quint64 *b);

// Returns the number of "one" bits in a 64-bit word
extern int popcount(quint64 b);

#endif // UTIL_H

