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

#ifndef SIDE_H
#define SIDE_H

#include <QObject>
#include <QString>

namespace Chess {

/*! The side or color of a chess player. */
enum Side
{
	White,	//!< The side with the white pieces.
	Black,	//!< The side with the black pieces.
	NoSide	//!< No side
};

/*! Returns the opposing side of \a side. */
inline Side otherSide(Side side)
{
	Q_ASSERT(side != NoSide);
	return Side(side ^ 1);
}

/*! Returns the character representation of \a side. */
inline QChar sideChar(Side side)
{
	if (side == White)
		return 'w';
	else if (side == Black)
		return 'b';
	return QChar();
}

/*! Converts a string into a Side. */
inline Side sideFromString(const QString& str)
{
	if (str == "w")
		return White;
	else if (str == "b")
		return Black;
	return NoSide;
}

/*! Returns a verbose, localized string representation of \a side. */
inline QString sideString(Side side)
{
	if (side == White)
		return QObject::tr("White");
	else if (side == Black)
		return QObject::tr("Black");
	return QString();
}

} // namespace Chess

#endif // SIDE_H
