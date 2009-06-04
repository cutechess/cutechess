#ifndef CHESSMOVE_H
#define CHESSMOVE_H

#include "piece.h"

namespace Chess {

/**
 * \brief A simple chess move.
 *
 * Move was designed to have minimal information about the move,
 * so a Board object is needed to verify the move's legality or
 * to convert it to a string.
 *
 * The source and target squares have an integer format specific to a
 * certain type of chess variant. The Board class has methods for
 * converting between these integers and the generic ChessSquare type.
 *
 * \sa Board
 * \sa Square
 * \sa Piece
 */
class LIB_EXPORT Move
{
	public:
		/*! Creates an empty Move. */
		Move();
		
		/*!
		 * Creates a new Move with at least a source square
		 * and a target square.
		 */
		Move(int sourceSquare,
		     int targetSquare,
		     Piece::Type promotion = Piece::NoPiece,
		     int castlingSide = -1);
		
		/*! Returns true if \a other is equal to this move. */
		bool operator==(const Move& other) const;

		/*! Returns true if \a other is different from this move. */
		bool operator!=(const Move& other) const;

		/*! Returns true if this is a null move. */
		bool isNull() const;
		
		/*! The source square. */
		int sourceSquare() const;
		
		/*! The target square. */
		int targetSquare() const;
		
		/*! Type of the promotion piece. */
		Piece::Type promotion() const;
		
		/*! Castling side, or -1 if the move is not a castling move. */
		int castlingSide() const;
	
	private:
		int m_sourceSquare;
		int m_targetSquare;
		Piece::Type m_promotion;
		int m_castlingSide;
};

} // namespace Chess
#endif // CHESSMOVE
