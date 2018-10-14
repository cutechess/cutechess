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

#ifndef RIFLEBOARD_H
#define RIFLEBOARD_H

#include "westernboard.h"

namespace Chess {

/*!
 * \brief A board for Rifle Chess
 *
 * Rifle Chess is a variant of standard chess with a different
 * way to capture pieces.
 *
 * An attacking piece stays on its square and removes (shoots)
 * an opponent piece without replacing it.
 *
 * Introduced by William B. Seabrook, USA 1921 / 1927.
 *
 * \note Rules: https://en.wikipedia.org/wiki/Rifle_chess
 */
class LIB_EXPORT RifleBoard : public WesternBoard
{
	public:
		/*! Creates a new RifleBoard object. */
		RifleBoard();

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;

	protected:
		// Inherited from WesternBoard
		virtual void vMakeMove(const Move& move,
				       BoardTransition* transition);
		virtual void vUndoMove(const Move& move);
		virtual bool inCheck(Side side, int square = 0) const;
		virtual void addPromotions(int sourceSquare,
					   int targetSquare,
					   QVarLengthArray< Move >& moves) const;
};


/*!
 * \brief A board for Shoot Chess
 *
 * Shoot Chess or Shooting Chess is a variant of Rifle Chess.
 * If a side can make a legal capturing move they must capture.
 *
 * Reference: S. Reuben, British Chess Magazine, February 1990
 *
 * \sa Rifle Chess
 * \sa Antichess
 */
class LIB_EXPORT ShootBoard : public RifleBoard
{
	public:
		/*! Creates a new ShootBoard object. */
		ShootBoard();

		// Inherited from RifleBoard
		virtual Board* copy() const;
		virtual QString variant() const;

	protected:
		// Inherited from WesternBoard
		virtual bool vIsLegalMove(const Move& move);
	private:
		quint64 m_testKey;
		bool m_canCapture;
};

} // namespace Chess
#endif // RIFLEBOARD_H
