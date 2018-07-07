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

#ifndef GRIDBOARD_H
#define GRIDBOARD_H

#include "restrictedmoveboard.h"

namespace Chess {

/*!
 * \brief A board for Grid Chess
 *
 * Grid Chess is a variant of standard chess with an additional rule.
 * The board is divided into 16 quadratic regions of 2x2 squares.
 * Moves within a region are forbidden.
 *
 * As a consequence a piece cannot be captured or protected from inside
 * its own region. A king cannot be checked from inside his own region.
 *
 * Introduced by Walter Stead in 1952.
 *
 * \note Rules: http://en.wikipedia.org/wiki/Grid_chess
 */
class LIB_EXPORT GridBoard : public RestrictedMoveBoard
{
	public:
		/*! Creates a new GridBoard object. */
		GridBoard();

		// Inherited from RestrictedMoveBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;

	protected:
		// Inherited from RestrictedMoveBoard
		virtual bool restriction(const Move& move,
					 bool reverse = false) const;

		/*!
		 * Returns true if the \a move targets a different region
		 * else false. Possible displacement of the grid is given by
		 * file shift \a fs and rank shift \a rs.
		 */
		bool leavesRegion(const Move& move,
				  int fs = 0,
				  int rs = 0) const;
};

/*!
 * \brief A board for Displaced-grid Chess
 *
 * Displaced-grid Chess is a variant of Grid Chess. The grid is shifted
 * by one rank and one file. There are 25 regions of nine central 2x2,
 * twelve 1x2 (or 2x1) and four 1x1 cells at the corners.
 *
 * Introduced by Doug Grant in 1974, the variant is also known as DGChess.
 *
 * \sa GridBoard
 *
 * \note Rules: http://en.wikipedia.org/wiki/Grid_chess
 */
class LIB_EXPORT DisplacedGridBoard : public GridBoard
{
	public:
		/*! Creates a new DisplacedGridBoard object. */
		DisplacedGridBoard();

		// Inherited from GridBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;

	protected:
		// Inherited from GridBoard
		virtual bool restriction(const Move& move,
					 bool reverse = false) const;
};

/*!
 * \brief A board for Slipped-grid Chess
 *
 * Slipped-grid Chess is a variant of Grid Chess. The grid is shifted
 * by one file. There are 20 regions of twelve 2x2, and eight 2x1 cells
 * at the sides.
 *
 * \sa GridBoard
 *
 * \note Rules: http://en.wikipedia.org/wiki/Grid_chess
 */
class LIB_EXPORT SlippedGridBoard : public GridBoard
{
	public:
		/*! Creates a new SlippedGridBoard object. */
		SlippedGridBoard();

		// Inherited from GridBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;

	protected:
		// Inherited from GridBoard
		virtual bool restriction(const Move& move,
					 bool reverse = false) const;
};

/*!
 * \brief A board for Berolina Grid Chess
 *
 * Berolina Grid Chess is a combination of Grid Chess and Berolina Chess.
 * The Berolina pawns move one step diagonally and capture straight.
 * In the grid they are more mobile than orthodox pawns.
 *
 * Introduced in NOST.
 *
 * \sa GridBoard
 * \sa BerolinaBoard
 *
 * \note Rules: http://en.wikipedia.org/wiki/Grid_chess
 */
class LIB_EXPORT BerolinaGridBoard : public GridBoard
{
	public:
		/*! Creates a new BerolinaGridBoard object. */
		BerolinaGridBoard();

		// Inherited from GridBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
};

} // namespace Chess
#endif // GRIDBOARD_H
