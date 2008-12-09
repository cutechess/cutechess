#ifndef CHESSMOVE_H
#define CHESSMOVE_H

#include "chess.h"


/**
 * \brief A simple chess move.
 *
 * Move was designed to have minimal information about the move,
 * so a Chess::Board object is needed to verify the move's legality or
 * to convert it to a string.
 *
 * The source and target squares have an integer format specific to a
 * certain type of chess variant. The Chess::Board class has methods for
 * converting between these integers and the generic ChessSquare type.
 *
 * \sa Chess::Board
 * \sa Chess::Square
 * \sa Chess::Piece
 */
class LIB_EXPORT Chess::Move
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
		     int promotion = 0,
		     int castlingSide = -1);
		
		/*! The source square. */
		int sourceSquare() const;
		
		/*! The target square. */
		int targetSquare() const;
		
		/*!
		 * Type of the promotion piece.
		 * \sa Chess::Piece
		 */
		int promotion() const;
		
		/*! Castling side, or -1 if the move is not a castling move. */
		int castlingSide() const;
	
	private:
		int m_sourceSquare;
		int m_targetSquare;
		int m_promotion;
		int m_castlingSide;
};

#endif // CHESSMOVE
