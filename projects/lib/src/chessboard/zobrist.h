#ifndef ZOBRIST_H
#define ZOBRIST_H

#include <QtGlobal>

/*!
 * \brief Unsigned 64-bit values for generating zobrist position keys.
 *
 * Chess::Board uses zobrist keys to quickly and easily compare two
 * positions for equality. Primary uses for zobrist keys are:
 * - Detecting repetitions, ie. a draw by three-fold repetition
 * - Opening books
 * - In hash table entries
 *
 * The Zobrist keys are compatible with the Polyglot opening book
 * specs: http://alpha.uhasselt.be/Research/Algebra/Toga/book_format.html
 *
 * \note This class is automatically initialized by a static
 * initializer object, so users shouldn't try to initialize it.
 */
class Zobrist
{
	public:
		/*!
		 * Returns the zobrist value for side to move.
		 * This value must be in the key on black's turn.
		 */
		static quint64 side();
		
		/*!
		 * Returns the zobrist value for en-passant target
		 * at \a square.
		 */
		static quint64 enpassant(int square);
		
		/*!
		 * Returns the zobrist value for player \a side's
		 * castling rook at \a square.
		 */
		static quint64 castling(int side, int square);
		
		/*!
		 * Returns the zobrist value for player \a side's
		 * \a type piece at \a square.
		 */
		static quint64 piece(int side, int type, int square);
		
	private:
		friend class ZobristInitializer;
		static const int MaxSquares = 144;
		static const int MaxPieceTypes = 16;
		
		Zobrist();
		
		/*! Initializes all zobrist components. */
		static void initialize();
		
		/*!
		 * The "minimal standard" random number generator
		 * by Park and Miller.
		 * Returns a pseudo-random integer between 1 and 2147483646.
		 */
		static int random();
		
		/*! Returns an unsigned 64-bit pseudo-random number. */
		static quint64 random64();
		
		static int m_randomSeed;
		static bool m_isInitialized;
		
		static quint64 m_side;
		static quint64 m_enpassant[MaxSquares];
		static quint64 m_castling[2][MaxSquares];
		static quint64 m_piece[2][MaxPieceTypes][MaxSquares];
};

#endif // ZOBRIST
