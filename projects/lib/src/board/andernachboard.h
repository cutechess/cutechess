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

#ifndef ANDERNACHBOARD_H
#define ANDERNACHBOARD_H

#include "standardboard.h"

namespace Chess {

/*!
 * \brief A board for Andernach Chess
 *
 * Andernach chess is a variant of standard chess with modified captures.
 * Capturing pieces - except for kings - change sides. A pawn capturing on
 * the final rank is promoted first and then changes sides.
 *
 * The game was introduced in 1993 during a tournament in Andernach, Germany.
 * It was suggested by H. P. Rehm and evolved from Double Tibetian Chess.
 * It is a popular variant in problem chess.
 *
 * \note Rules: https://en.wikipedia.org/wiki/Andernach_chess
 *
 * AndernachBoard uses Polyglot-compatible zobrist position keys,
 * so appropriate books in Polyglot format can be used.
 *
 * \note Standard Chess Rules: http://www.fide.com/component/handbook/?id=124&view=article
 * \sa PolyglotBook
 * \sa AntiAndernachBoard
 */
class LIB_EXPORT AndernachBoard : public StandardBoard
{
	public:
		/*! Creates a new AndernachBoard object. */
		AndernachBoard();

		// Inherited from StandardBoard
		virtual Board* copy() const;
		virtual QString variant() const;

	protected:
		/*!
		 * Returns true if \a move switches sides of moved piece.
		 * True for a capture move not by king.
		 */
		virtual bool switchesSides(const Move& move) const;

		// Inherited from StandardBoard
		virtual Move moveFromSanString(const QString& str);
		virtual QString sanMoveString(const Move& move);
		virtual void vMakeMove(const Move& move,
				       BoardTransition* transition);
		virtual void vUndoMove(const Move &move);
};


/*!
 * \brief A board for Anti-Andernach Chess
 *
 * Anti-Andernach chess is standard chess but non-capturing moves imply a
 * change of color (except for Kings). Used in problem chess.
 * \sa AndernachBoard
 */
class LIB_EXPORT AntiAndernachBoard : public AndernachBoard
{
	public:
		/*! Creates a new AntiAndernachBoard object. */
		AntiAndernachBoard();

		// Inherited from AndernachBoard
		virtual Board* copy() const;
		virtual QString variant() const;

	protected:
		/*!
		 * Returns true if \a move switches sides of moved piece.
		 * True for a non-capture move not by king.
		 */
		virtual bool switchesSides(const Move& move) const;
};

/*!
 * \brief A board for Super-Andernach Chess
 *
 * All moves except for King moves imply a change of color in
 * Super-Andernach chess. This variant is used in problem chess.
 * \sa AndernachBoard
 */
class LIB_EXPORT SuperAndernachBoard : public AndernachBoard
{
	public:
		/*! Creates a new SuperAndernachBoard object. */
		SuperAndernachBoard();

		// Inherited from AndernachBoard
		virtual Board* copy() const;
		virtual QString variant() const;

	protected:
		/*!
		 * Returns true if \a move switches sides of moved piece.
		 * True for any move not by king.
		 */
		virtual bool switchesSides(const Move& move) const;
};

} // namespace Chess
#endif // ANDERNACHBOARD_H
