#ifndef CHESS_H
#define CHESS_H

/*!
 * \brief Fundamental Chess types.
 */
namespace Chess
{

	/*! Chess piece types for all supported variants. */
	enum Piece
	{
		NoPiece,	//!< No piece. Used for empty squares.
		Pawn,		//!< Pawn
		Knight,		//!< Knight
		Bishop,		//!< Bishop
		Rook,		//!< Rook
		Queen,		//!< Queen
		King,		//!< King
		Archbishop,	//!< Archbishop. Only for Capablanca variants.
		Chancellor,	//!< Chancellor. Only for Capablanca variants.
		InvalidPiece	//!< Invalid piece, for wall squares outside of board.
	};
	
	/*! The side or color of a chess player. */
	enum Side
	{
		NoSide = -1,	//!< No side, for observer (force) mode
		White,		//!< The side with the white pieces.
		Black		//!< The side with the black pieces.
	};
	
	/*! The result of a chess game. */
	enum Result
	{
		//! White wins by mating the black player.
		WhiteMates,
		//! Black wins by mating the white player.
		BlackMates,
		//! Draw by a stalemate.
		Stalemate,
		//! Draw by insufficient mating material on both sides.
		DrawByMaterial,
		//! Draw by 3 repetitions of the same position.
		DrawByRepetition,
		//! Draw by 50 consecutive reversible moves.
		DrawByFiftyMoves,
		//! No result. The game may continue.
		NoResult
	};
	
	/*! All supported variants of chess. */
	enum Variant
	{
		StandardChess,	//!< Standard chess or Chess960
		CapablancaChess	//!< Capablanca/Gothic chess.
	};
	
	/*! Notation for move strings. */
	enum MoveNotation
	{
		StandardAlgebraic,	//!< Standard Algebraic notation (SAN).
		LongAlgebraic		//!< Long Algebraic/Coordinate notation.
	};
	
	/*!
	* \brief A generic chess square type consisting of a file and a rank.
	*
	* Square is mainly used as a middle-layer between the Board
	* class (which uses integers for squares) and the various string
	* conversion (notation) functions.
	*/
	struct Square
	{
		//! Zero-based file of the square. 0 is the 'a' file.
		int file;
		//! Zero-based rank of the square. 0 is white's first rank
		int rank;
	};
	
	class Board;
	class Move;
}

#endif // CHESS
