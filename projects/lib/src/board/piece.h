/*
    This file is part of Cute Chess.
    Copyright (C) 2008-2018 Cute Chess authors

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

#include <QtGlobal>
#include "side.h"

namespace Chess {

/*!
 * \brief A chess piece
 *
 * The Piece class represents the contents of a square on the chessboard.
 * It can be an empty piece (NoPiece) or a wall piece (WallPiece) belonging
 * to neither player (NoSide), or it can be any chess piece belonging
 * to either White or Black.
 *
 * For performance reasons the class is just a wrapper for integer, and it
 * has a constructor for implicitly converting integers to Piece objects.
 *
 * \note A Board object is needed to convert between a Piece and a string.
 */
class Piece
{
	public:
		/*! No piece. Used for empty squares. */
		static const int NoPiece = 0;
		/*! A wall square outside of board. */
		static const int WallPiece = 100;

		/*! Creates a new piece of type \a type for \a NoSide. */
		Piece(int type = NoPiece);
		/*! Creates a new piece of type \a type for \a side. */
		Piece(Side side, int type);

		/*! Returns true if \a other is the same as this piece. */
		bool operator==(const Piece& other) const;
		/*! Returns true if \a other is different from this piece. */
		bool operator!=(const Piece& other) const;
		/*! Returns true if this piece is less than \a other. */
		bool operator<(const Piece& other) const;
		/*! Returns true if this piece is more than \a other. */
		bool operator>(const Piece& other) const;

		/*! Returns true if the piece is empty (type is NoPiece). */
		bool isEmpty() const;
		/*! Returns true if this is a valid chess piece. */
		bool isValid() const;
		/*! Returns true if this is a wall piece (inaccessible square). */
		bool isWall() const;

		/*! Returns the side the piece belongs to. */
		Side side() const;
		/*! Returns the type of the piece. */
		int type() const;

		/*! Sets the side to \a side. */
		void setSide(Side side);
		/*! Sets the type to \a type. */
		void setType(int type);

	private:
		quint16 m_data;
};


inline Piece::Piece(int type)
	: m_data(type | (Side::NoSide << 10))
{
}

inline Piece::Piece(Side side, int type)
	: m_data(type | (side << 10))
{
	Q_ASSERT(!side.isNull());
	Q_ASSERT(type != WallPiece);
	Q_ASSERT(type != NoPiece);
}

inline bool Piece::operator==(const Piece& other) const
{
	return m_data == other.m_data;
}

inline bool Piece::operator!=(const Piece& other) const
{
	return m_data != other.m_data;
}

inline bool Piece::operator<(const Piece& other) const
{
	return m_data < other.m_data;
}

inline bool Piece::operator>(const Piece& other) const
{
	return m_data > other.m_data;
}

inline bool Piece::isEmpty() const
{
	return type() == NoPiece;
}

inline bool Piece::isValid() const
{
	return !side().isNull();
}

inline bool Piece::isWall() const
{
	return type() == WallPiece;
}

inline Side Piece::side() const
{
	return Side(Side::Type(m_data >> 10));
}

inline int Piece::type() const
{
	return m_data & 0x3FF;
}

inline void Piece::setSide(Side side)
{
	m_data = type() | (side << 10);
}

inline void Piece::setType(int type)
{
	m_data = type | (m_data & 0xC00);
}

} // namespace Chess
#endif // PIECE_H
