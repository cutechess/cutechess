#include "chessboard.h"
#include "util.h"
#include "zobrist.h"


// Random values for everything that's needed in a key (side to move,
// enpassant square, castling rights, and piece positions).
Zobrist zobrist;

// Returns a pseudo-random unsigned 64-bit number
static quint64
rand64()
{
	quint64 rand1 = (quint64)myRand();
	quint64 rand2 = (quint64)myRand();
	quint64 rand3 = (quint64)myRand();

	return rand1 ^ (rand2 << 31) ^ (rand3 << 62);
}

// Initialize the zobrist values
void Chessboard::initZobristKeys()
{
	int color;
	int square;

	zobrist.color = rand64();
	for (color = White; color <= Black; color++) {
		int piece;
		zobrist.castle[color][C_KSIDE] = rand64();
		zobrist.castle[color][C_QSIDE] = rand64();
		for (piece = Pawn; piece <= King; piece++) {
			for (square = 0; square < 64; square++) {
				zobrist.piece[color][piece][square] = rand64();
			}
		}
	}
	for (square = 0; square < 64; square++)
		zobrist.enpassant[square] = rand64();
}

// Generate the Zobrist key for a board position
void Chessboard::computeZobristKey(void)
{
	int color;
	int enpassantSquare;
	unsigned castleRights;
	quint64 key = 0;

	castleRights = m_posp->castleRights;
	enpassantSquare = m_posp->enpassantSquare;
	for (color = White; color <= Black; color++) {
		int piece;
		quint64 mask;

		if (castleRights & castling.rights[color][C_KSIDE])
			key ^= zobrist.castle[color][C_KSIDE];
		if (castleRights & castling.rights[color][C_QSIDE])
			key ^= zobrist.castle[color][C_QSIDE];

		for (piece = Pawn; piece <= King; piece++) {
			mask = m_pieces[color][piece];
			while (mask)
				key ^= zobrist.piece[color][piece][popLsb(&mask)];
		}
	}
	if (enpassantSquare)
		key ^= zobrist.enpassant[enpassantSquare];
	if (m_color == Black)
		key ^= zobrist.color;

	m_posp->key = key;
}

