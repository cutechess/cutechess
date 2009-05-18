#ifndef CHESS_H
#define CHESS_H

#include <QString>

/*!
 * \brief Fundamental Chess types.
 */
namespace Chess {

/*! The side or color of a chess player. */
enum Side
{
	NoSide = -1,	//!< No side, for observer (force) mode
	White,		//!< The side with the white pieces.
	Black		//!< The side with the black pieces.
};

/*! Notation for move strings. */
enum MoveNotation
{
	StandardAlgebraic,	//!< Standard Algebraic notation (SAN).
	LongAlgebraic,		//!< Long Algebraic/Coordinate notation.
	UciLongAlgebraic	//!< Long Algebraic notation used by UCI engines
};

/*! Notation for FEN strings. */
enum FenNotation
{
	XFen,		//!< X-FEN notation
	ShredderFen	//!< Shredder FEN notation
};

class Variant;
class Result;
class Square;
class Board;
class Piece;
class Move;

} // namespace Chess
#endif // CHESS
