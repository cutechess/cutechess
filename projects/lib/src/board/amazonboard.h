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

#ifndef AMAZONBOARD_H
#define AMAZONBOARD_H

#include "westernboard.h"

namespace Chess {

/*!
 * \brief A board for Amazon Chess
 *
 * Amazon Chess is a variant of standard chess where the Queen is
 * replaced by the Amazon (or Empress) with Queen and Knight movements.
 *
 * This variant was described in Italy, 16th century. It was widely
 * adopted in Russia and Georgia.
 *
 * \note Rules: https://en.wikipedia.org/wiki/Amazon_Chess
 *
 */
class LIB_EXPORT AmazonBoard : public WesternBoard
{
	public:
		/*! Creates a new AmazonBoard object. */
		AmazonBoard();

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
	protected:
		enum AmazonPieceType
		{
			Amazon = Queen //!< Amazon (queen + rook) replaces Queen
		};
};

} // namespace Chess
#endif // AMAZONBOARD_H
