#ifndef BOOKMOVE_H
#define BOOKMOVE_H

#include "chess.h"

/*!
 * \brief A variant and book-independent opening book move.
 *
 * When a move is retrieved from an opening book of any kind, it will
 * be in this format. Later it can be converted to a Chess::Move by a
 * Chess::Board object.
 */
class LIB_EXPORT BookMove
{
	public:
		/*! Constructs a new empty book move. */
		BookMove();
		
		/*! Constructs and initializes a new book move. */
		BookMove(const Chess::Square& sourceSquare,
		         const Chess::Square& targetSquare,
		         int promotion);
		
		/*! Compares two book moves for equality. */
		bool operator==(const BookMove& move) const;
		
		/*! The source square. */
		const Chess::Square& sourceSquare() const;
		
		/*! The target square. */
		const Chess::Square& targetSquare() const;
		
		/*! The promotion piece. */
		int promotion() const;

	private:	
		Chess::Square m_sourceSquare;
		Chess::Square m_targetSquare;
		int m_promotion;
};

#endif // BOOKMOVE_H

