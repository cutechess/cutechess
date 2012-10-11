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

#ifndef GAVIOTATABLEBASE_H
#define GAVIOTATABLEBASE_H

#include <QFlags>
#include <QList>
#include <QPair>
#include "result.h"
#include "square.h"
#include "piece.h"
class QStringList;

/*!
 * \brief A wrapper for probing Gaviota endgame tablebases.
 *
 * Gaviota tablebases are heavily compressed tables that contain the
 * expected result and "distance to mate" information for positions
 * where only 3-5 pieces are left on the board. The tablebases can be
 * used to adjudicate games or to provide perfect play in endgame
 * positions. Gaviota tablebases can only be used in standard chess
 * and Fischer Random chess.
 */
class LIB_EXPORT GaviotaTablebase
{
	public:
		/*! Castling rights in Gaviota EGTB format. */
		enum CastlingFlag
		{
			NoCastling = 0x0,	//!< No castling rights
			WhiteKingSide  = 0x8,	//!< White can castle on king's side
			WhiteQueenSide = 0x4,	//!< White can castle on queen's side
			BlackKingSide  = 0x2,	//!< Black can castle on king's side
			BlackQueenSide = 0x1	//!< Black can castle on queen's side
		};
		Q_DECLARE_FLAGS(Castling, CastlingFlag)

		/*! Synonym for QList< QPair<Chess::Square, Chess::Piece> >. */
		typedef QList< QPair<Chess::Square, Chess::Piece> > PieceList;

		/*!
		 * Initializes the tablebases.
		 *
		 * Returns true if successfull; otherwise returns false.
		 *
		 * The tablebases should be located in the directories listed
		 * in \a paths.
		 */
		static bool initialize(const QStringList& paths);
		/*! Cleans up when the tablebases aren't needed any more. */
		static void cleanup();
		/*!
		 * Returns the expected game result for the positions specified
		 * by \a side, \a enpassantSq, \a castling and \a pieces.
		 *
		 * If the position isn't found in the tablebases, a null result
		 * is returned.
		 *
		 * \sa Chess::Board::tablebaseResult()
		 */
		static Chess::Result result(const Chess::Side& side,
					    const Chess::Square& enpassantSq,
					    Castling castling,
					    const PieceList& pieces);

	private:
		GaviotaTablebase();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(GaviotaTablebase::Castling)

#endif // GAVIOTATABLEBASE_H
