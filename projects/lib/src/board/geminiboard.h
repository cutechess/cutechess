/*
    This file is part of Cute Chess.
    Copyright (C) 2008-2020 Cute Chess authors

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

#ifndef GEMINIBOARD_H
#define GEMINIBOARD_H

#include "westernboard.h"

namespace Chess {

/*!
 * \brief A board for Gemini chess
 *
 * Gemini chess is a variant of standard chess which adds one
 * piece type and is played on a 10x8 board.
 *
 * \note Rules: https://musketeerchess.net/games/gemini/rules/rules.php
 */
class LIB_EXPORT GeminiBoard : public WesternBoard
{
	public:
        /*! Creates a new GeminiBoard object. */
        GeminiBoard();

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual int width() const;
		virtual QString defaultFenString() const;

	protected:
        /*! Special piece types for Gemini variants. */
        enum GeminiPieceType
		{
			Archbishop = 7,	//!< Archbishop (knight + bishop)
		};

		// Inherited from WesternBoard
		virtual void addPromotions(int sourceSquare,
					   int targetSquare,
					   QVarLengthArray<Move>& moves) const;
};

} // namespace Chess
#endif // GEMINIBOARD_H
