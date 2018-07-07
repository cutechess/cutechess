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

#ifndef MOVE_H
#define MOVE_H

#include <QtGlobal>
#include <QMetaType>

namespace Chess {

/*!
 * \brief A small and efficient chessmove class.
 *
 * This class is designed to be used internally by Board objects,
 * and to store minimal information about the move. A Board object
 * is needed to verify the move's legality or to convert it to a string.
 *
 * The source and target squares have an integer format specific to a
 * certain type of chess variant. The Board class has methods for
 * converting between these integers and the generic Square type.
 *
 * \sa Piece
 * \sa Board
 * \sa GenericMove
 */
class Move
{
	public:
		/*! Creates an empty Move (null move). */
		Move();
		/*!
		 * Creates a new Move object with at least a source
		 * square and a target square.
		 */
		Move(int sourceSquare,
		     int targetSquare,
		     int promotion = 0);

		/*!
		 * The source square.
		 *
		 * A value of 0 means that this move is a piece drop,
		 * a special move allowed by some variants.
		 */
		int sourceSquare() const;
		/*! The target square. */
		int targetSquare() const;
		/*!
		 * Type of the promotion piece.
		 *
		 * A value of 0 means no promotion.
		 * If this move is a piece drop, the promotion type
		 * denotes the type of the dropped piece.
		 */
		int promotion() const;

		/*! Returns true if this is a null move. */
		bool isNull() const;
		/*! Returns true if \a other is equal to this move. */
		bool operator==(const Move& other) const;
		/*! Returns true if \a other is different from this move. */
		bool operator!=(const Move& other) const;

	private:
		quint32 m_data;
};


inline Move::Move()
	: m_data(0)
{
}

inline Move::Move(int sourceSquare,
		  int targetSquare,
		  int promotion)
	: m_data(sourceSquare |
		 (targetSquare << 10) |
		 (promotion << 20))
{
	Q_ASSERT(sourceSquare >= 0 && sourceSquare <= 0x3FF);
	Q_ASSERT(targetSquare >= 0 && targetSquare <= 0x3FF);
	Q_ASSERT(promotion >= 0 && promotion <= 0x3FF);
}

inline bool Move::isNull() const
{
	return (m_data == 0);
}

inline bool Move::operator==(const Move& other) const
{
	return (m_data == other.m_data);
}

inline bool Move::operator!=(const Move& other) const
{
	return (m_data != other.m_data);
}

inline int Move::sourceSquare() const
{
	return m_data & 0x3FF;
}

inline int Move::targetSquare() const
{
	return (m_data >> 10) & 0x3FF;
}

inline int Move::promotion() const
{
	return (m_data >> 20) & 0x3FF;
}

} // namespace Chess

Q_DECLARE_METATYPE(Chess::Move)

#endif // MOVE_H
