/*
    This file is part of Cute Chess.

    Cute Chess is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Cute Chess is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Cute Chess.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PIECE_H
#define PIECE_H

#include "chess.h"
class QString;

namespace Chess {

/*!
 * \brief A chess piece
 *
 * The Piece class represents the contents of a square on the chessboard.
 * It can be an empty piece (NoPiece) or a wall piece (WallPiece) belonging
 * to neither player (Chess::NoSide), or it can be any chess piece belonging
 * to either White or Black.
 *
 * For performance reasons the class is just a wrapper for integer, and it
 * has a constructor for implicitly converting integers to Piece objects.
 */
class LIB_EXPORT Piece
{
	public:
		/*! Piece types for all supported chess variants. */
		enum Type
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
			WallPiece	//!< A wall square outside of board.
		};

		/*! Creates a new piece with piece code \a code. */
		Piece(int code = NoPiece);
		/*! Creates a new piece of type \a type for \a side. */
		Piece(int side, int type);
		/*! Creates a new piece from piece character \a c. */
		Piece(const QChar& c);

		/*! Returns true if \a other is the same as this piece. */
		bool operator==(const Piece& other) const;
		/*! Returns true if \a other is different from this piece. */
		bool operator!=(const Piece& other) const;

		/*! Returns true if the piece is empty (type is NoPiece). */
		bool isEmpty() const;
		/*! Returns true if this is a valid chess piece. */
		bool isValid() const;
		/*! Returns true if this is a wall piece (an inaccessible square). */
		bool isWall() const;

		/*! Returns the piece code, the raw data of this object. */
		int code() const;
		/*! Returns the side the piece belongs to. */
		Side side() const;
		/*! Returns the type of the piece. */
		Type type() const;
		/*! Returns the piece in character format. */
		QChar toChar() const;
		/*! Returns the internal non-internationalized name of the piece. */
		QString internalName() const;

		/*! Sets the side to \a side. */
		void setSide(int side);
		/*! Sets the type to type. */
		void setType(int type);

	private:
		int m_code;
};

} // namespace Chess
#endif // PIECE_H
