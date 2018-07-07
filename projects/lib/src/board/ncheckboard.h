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

#ifndef NCHECKBOARD_H
#define NCHECKBOARD_H

#include "standardboard.h"

namespace Chess {

/*!
 * \brief A board for N-Check Chess
 *
 * N-Check chess is a variant of standard chess with an additional rule.
 * A player also wins when he gives check n times in a game.
 *
 * \note Rules: http://en.wikipedia.org/wiki/Three-check_chess
 *
 * NCheckBoard uses Polyglot-compatible zobrist position keys,
 * so N-Check opening books in Polyglot format could be used.
 *
 * \note Standard Chess Rules: http://www.fide.com/component/handbook/?id=124&view=article
 * \sa PolyglotBook
 */
class LIB_EXPORT NCheckBoard : public StandardBoard
{
	protected:
		/*!
		 * Creates a new NCheckBoard object.
		 * Giving the \a n -th check wins.
		 * Defaults to Three-Check (variant: 3Check)
		 */
		NCheckBoard(int n = 3);
	public:
		// Inherited from StandardBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
		virtual Result result();

		/*! Returns number of checks yet needed for \a side to win */
		int checksToWin(Side side) const;

	protected:
		// Inherited from StandardBoard
		virtual void vInitialize();
		virtual QString vFenIncludeString(FenNotation notation) const;
		virtual bool vSetFenString(const QStringList& fen);
		virtual void vMakeMove(const Move& move,
				       BoardTransition* transition);
		virtual void vUndoMove(const Move& move);

		/*! Returns total number of checks necessary to win */
		int checkLimit() const;

	private:
		int m_checkLimit;
		int m_checksToWin[2];
		void setChecksToWin(int whiteCount, int blackCount);
		void updateCheckCounters(Side side, int d);
};

/*!
 * \brief A board for Three-Check Chess (Wild 25)
 *
 * Three-Check chess is a variant of standard chess with an additional rule.
 * A player also wins when he gives check three times in a game.
 * This is a trivial subclass of NCheckboard (variant: 3Check)
 */
class LIB_EXPORT ThreeCheckBoard : public NCheckBoard
{
	public:
		ThreeCheckBoard();
		virtual Board* copy() const;
};

/*!
 * \brief A board for Five-Check Chess
 *
 * Five-Check chess is a variant of standard chess with an additional rule.
 * A player also wins when he gives check five times in a game.
 * This is a trivial subclass of NCheckboard (variant: 5Check)
 */
class LIB_EXPORT FiveCheckBoard : public NCheckBoard
{
	public:
		FiveCheckBoard();
		virtual Board* copy() const;
};

} // namespace Chess
#endif // NCHECKBOARD_H
