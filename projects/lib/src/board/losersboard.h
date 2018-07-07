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

#ifndef LOSERSBOARD_H
#define LOSERSBOARD_H

#include "westernboard.h"

namespace Chess {

/*!
 * \brief A board for Losers chess (or Wild 17)
 *
 * Losers is a variant of Standard chess where the players
 * try to lose all of their pieces, except for the king. If a
 * player can capture an opponent's piece, they must do so.
 *
 * \note Rules: http://wiki.wildchess.org/wiki/index.php/Losers
 */
class LIB_EXPORT LosersBoard : public WesternBoard
{
	public:
		/*! Creates a new LosersBoard object. */
		LosersBoard();

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
		virtual Result result();

	protected:
		// Inherited from WesternBoard
		virtual bool vSetFenString(const QStringList& fen);
		virtual bool vIsLegalMove(const Move& move);

	private:
		bool m_canCapture;
		quint64 m_captureKey;
};

} // namespace Chess
#endif // LOSERSBOARD_H
