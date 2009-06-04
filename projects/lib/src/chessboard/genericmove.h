#ifndef GENERICMOVE_H
#define GENERICMOVE_H

#include "chess.h"
#include "square.h"
#include "piece.h"

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
			    Chess::Piece::Type promotion);
		
		/*! Returns true if \a other is the same as this move. */
		bool operator==(const GenericMove& other) const;
		
		/*! Returns true if \a other is different from this move. */
		bool operator!=(const GenericMove& other) const;

		/*! Returns true if this is a null move. */
		bool isNull() const;

		/*! The source square. */
		const Chess::Square& sourceSquare() const;
		
		/*! The target square. */
		const Chess::Square& targetSquare() const;
		
		/*! Type of the promotion piece. */
		Chess::Piece::Type promotion() const;

	private:	
		Chess::Square m_sourceSquare;
		Chess::Square m_targetSquare;
		Chess::Piece::Type m_promotion;
};

#endif // GENERICMOVE_H
