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

#ifndef CLOBBERBOARD_H
#define CLOBBERBOARD_H

#include "westernboard.h"

namespace Chess {

/*!
 * \brief A board for Clobber
 *
 * Clobber is an abstract strategy game of two sides. Initially White
 * pieces occupy all of the light squares and Black has their pieces on
 * all of the dark squares. The players take turns capturing (clobbering)
 * opponent pieces. A piece can only move one square vertically or hori-
 * zontally onto an opponent's square and displace the target stone.
 * The side that cannot move anymore loses.
 *
 * This game was introduced in 2001 by Michael H. Albert, J.P. Grossman
 * and Richard Nowakowsk.
 *
 * \note Rules: https://en.wikipedia.org/wiki/Clobber
 *
 */
class LIB_EXPORT ClobberBoard : public WesternBoard
{
	public:
		/*! Creates a new ClobberBoard object. */
		ClobberBoard(int width = 5, int height = 6);

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
		virtual int width() const;
		virtual int height() const;
		virtual Result result();
	protected:
		enum ClobberPieceType
		{
			Stone = Pawn //!< Stone
		};

		/*! Returns target square offsets */
		const QVarLengthArray<int>& targetOffsets() const;

		// Inherited from WesternBoard
		virtual bool hasCastling() const;
		virtual bool hasEnPassantCaptures() const;
		virtual bool kingsCountAssertion(int, int) const;
		virtual void vInitialize();
		virtual void addPromotions(int, int,
					   QVarLengthArray< Chess::Move >&) const;
		virtual void generateMovesForPiece(QVarLengthArray<Move> & moves,
						   int pieceType,
						   int square) const;
		virtual bool inCheck(Side side, int square) const;
		virtual QString sanMoveString(const Move & move);
	private:
		int m_width;
		int m_height;
		QVarLengthArray<int> m_offsets;
};


/*!
 * \brief A board for the 10 x 10 squares version of Clobber
 *
 * Clobber10 uses a bigger board with 10 files and 10 rows.
 * It is played at Computer Olympiads.
 *
 * \sa ClobberBoard
 */
class LIB_EXPORT Clobber10Board : public ClobberBoard
{
	public:
		Clobber10Board();
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
};


/*!
 * \brief A board for the cannibalistic version of Clobber
 *
 * Also own pieces can be clobbered.
 * Introduced by Ingo Althöfer, Germany 2003.
 *
 * \sa ClobberBoard
 */
class LIB_EXPORT CannibalClobberBoard : public ClobberBoard
{
	public:
		CannibalClobberBoard(int width = 5, int height = 6);
		virtual Board* copy() const;
		virtual QString variant() const;

	protected:
		// Inherited from ClobberBoard
		virtual void generateMovesForPiece(QVarLengthArray<Move> & moves,
						   int pieceType,
						   int square) const;
};


/*!
 * \brief A board for the cannibalistic version of Clobber10
 *
 * Also own pieces can be clobbered.
 * Introduced by Ingo Althöfer, Germany 2003.
 *
 * \sa ClobberBoard
 */
class LIB_EXPORT CannibalClobber10Board : public CannibalClobberBoard
{
	public:
		CannibalClobber10Board();
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
};

} // namespace Chess
#endif // CLOBBERBOARD_H
