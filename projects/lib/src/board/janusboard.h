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

#ifndef JANUSBOARD_H
#define JANUSBOARD_H

#include "westernboard.h"
#include "westernzobrist.h"

namespace Chess {

/*!
 * \brief A board for Janus Chess
 *
 * Janus Chess is a variant of standard chess which adds the Janus
 * piece type (Bishop + Knight). Like Capablanca Chess it is played
 * on a 10x8 board.
 *
 * Initially the Jani are positioned between Rooks and Knights. The
 * King castles from E-file to either B- or I-file, the corresponding
 * rook to C- or H-file.
 *
 * Introduced by Werner Schöndorf, Germany, 1978.
 *
 * \note Rules: https://en.wikipedia.org/wiki/Janus_chess
 *
 * \sa CapablancaBoard
 */
class LIB_EXPORT JanusBoard : public WesternBoard
{
	public:
		/*! Creates a new JanusBoard object. */
		JanusBoard();

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual int width() const;
		virtual QString defaultFenString() const;

	protected:
		/*! Special piece types for Capablanca variants. */
		enum JanusPieceType
		{
			Janus = 7,	//!< Janus = Princess (knight + bishop)
		};

		// Inherited from WesternBoard
		virtual void addPromotions(int sourceSquare,
					   int targetSquare,
					   QVarLengthArray<Move>& moves) const;
		virtual int castlingFile(CastlingSide castlingSide) const;
		virtual QString sanMoveString(const Move& move);
		virtual Move moveFromSanString(const QString& str);
};

} // namespace Chess
#endif // JANUSBOARD_H
