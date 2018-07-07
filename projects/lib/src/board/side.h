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

#ifndef SIDE_H
#define SIDE_H

#include <QString>
#include <QMetaType>
#include <QCoreApplication>

namespace Chess {

/*!
 * \brief The side or color of a chess player.
 *
 * This class is a simple wrapper for the enumerated type Side::Type.
 * Side objects can be used just like one would use an enum type
 * (eg. as an array index).
 */
class LIB_EXPORT Side
{
	Q_DECLARE_TR_FUNCTIONS(Side)

	public:
		/*! The enumerated type for the side. */
		enum Type
		{
			White,	//!< The side with the white pieces.
			Black,	//!< The side with the black pieces.
			NoSide	//!< No side
		};

		/*! Constructs a new, null Side object. */
		Side();
		/*! Constructs a new Side object of type \a type. */
		Side(Type type);
		/*!
		 * Constructs a new Side object from a symbol.
		 *
		 * The symbol can be "w" for \a White, "b" for \a Black,
		 * or anything else for \a NoSide.
		 */
		explicit Side(const QString& symbol);

		/*! Returns true if the side is \a NoSide. */
		bool isNull() const;
		/*! Operator for the \a Type value of the side. */
		operator Type() const;

		/*!
		 * Returns the opposite side.
		 * \note The side must not be null.
		 */
		Side opposite() const;
		/*! Returns the text symbol for the side. */
		QString symbol() const;
		/*! Returns a localized name of the side. */
		QString toString() const;

	private:
		Type m_type;
};

inline Side::Side()
	: m_type(NoSide)
{
}

inline Side::Side(Type type)
	: m_type(type)
{
}

inline bool Side::isNull() const
{
	return (m_type == NoSide);
}

inline Side::operator Type() const
{
	return m_type;
}

inline Side Side::opposite() const
{
	Q_ASSERT(!isNull());
	return Side(Type(int(m_type) ^ 1));
}

} // namespace Chess

Q_DECLARE_METATYPE(Chess::Side)

#endif // SIDE_H
