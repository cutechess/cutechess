#ifndef GENERICMOVE_H
#define GENERICMOVE_H

#include "chess.h"
#include "square.h"

/*!
 * \brief A chess move independent of chess variant or opening book format
 *
 * When a move is made by a human or retrieved from an opening book of any
 * kind, it will be in this format. Later it can be converted to Chess::Move
 * by a Chess::Board object.
 */
class LIB_EXPORT GenericMove
{
	public:
		/*! Constructs a new empty move. */
		GenericMove();
		
		/*! Constructs and initializes a new move. */
		GenericMove(const Chess::Square& sourceSquare,
			    const Chess::Square& targetSquare,
			    int promotion);
		
		/*! Returns true if \a other is the same as this move. */
		bool operator==(const GenericMove& other) const;
		
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

#endif // GENERICMOVE_H
