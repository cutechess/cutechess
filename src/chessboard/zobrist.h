#ifndef ZOBRIST_H
#define ZOBRIST_H

#include <QtGlobal>


// Random 64-bit values for generating Zobrist keys
typedef struct _Zobrist
{
	quint64 color;
	quint64 enpassant[64];
	quint64 castle[2][2];
	quint64 piece[2][8][64];
} Zobrist;


// Random values for everything that's needed in a key (side to move,
// enpassant square, castling rights, and piece positions).
extern Zobrist zobrist;

#endif // ZOBRIST_H

