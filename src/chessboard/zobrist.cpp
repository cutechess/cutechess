#include "zobrist.h"


int Zobrist::m_randomSeed = 1;
quint64 Zobrist::m_side;
quint64 Zobrist::m_enpassant[MaxSquares];
quint64 Zobrist::m_castling[2][2];
quint64 Zobrist::m_piece[2][MaxPieceTypes][MaxSquares];

/* The "minimal standard" random number generator by Park and Miller.
   Returns a pseudo-random integer between 1 and 2147483646.  */
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

// Returns a pseudo-random unsigned 64-bit number
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

quint64 Zobrist::castling(Chessboard::ChessSide side, int castlingSide)
{
	Q_ASSERT(side != Chessboard::NoSide);
	Q_ASSERT(castlingSide == CastlingRights::LeftSide || castlingSide == CastlingRights::RightSide);
	return m_castling[side][castlingSide];
}

quint64 Zobrist::piece(Chessboard::ChessSide side, ChessPiece::PieceType type, int square)
{
	Q_ASSERT(side != Chessboard::NoSide);
	Q_ASSERT(type != ChessPiece::PT_None);
	Q_ASSERT(square >= 0 && square < MaxSquares);
	return m_piece[side][type][square];
}

// Initialize the zobrist values
void Zobrist::initialize()
{
	int side;
	int square;

	m_side = random64();
	for (side = 0; side < 2; side++)
	{
		int piece;
		m_castling[side][CastlingRights::LeftSide] = random64();
		m_castling[side][CastlingRights::RightSide] = random64();
		for (piece = 0; piece < MaxPieceTypes; piece++)
		{
			for (square = 0; square < MaxSquares; square++)
			{
				m_piece[side][piece][square] = random64();
			}
		}
	}
	for (square = 0; square < MaxSquares; square++)
		m_enpassant[square] = random64();
}

// Generate the Zobrist key for a board position
quint64 Zobrist::key(const Chessboard* board)
{
	quint64 key = 0;

	for (int i = 0; i < 2; i++)
	{
		const CastlingRights *cr = board->castlingRights((Chessboard::ChessSide)i);
		
		if (cr->canCastle(CastlingRights::LeftSide))
			key ^= castling((Chessboard::ChessSide)i, CastlingRights::LeftSide);
		if (cr->canCastle(CastlingRights::RightSide))
			key ^= castling((Chessboard::ChessSide)i, CastlingRights::RightSide);

		foreach (ChessPiece* pc, board->m_pieces[i])
			key ^= piece((Chessboard::ChessSide)i, pc->type(), pc->square());
	}
	
	if (board->enpassantSquare() != -1)
		key ^= enpassant(board->enpassantSquare());
	if (board->side() == Chessboard::Black)
		key ^= side();

	return key;
}
