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

#ifndef SEIRAWANBOARD_H
#define SEIRAWANBOARD_H

#include "westernboard.h"

namespace Chess {

/*!
 * \brief A board for S-Chess (a.k.a. Seirawan Chess)
 *
 * S-Chess is a variant of standard chess which adds two piece types and
 * is played on a regular 8x8 board.
 *
 * The variant was introduced by Bruce Harper and Yasser Seirawan (USA, 2007)
 * using the piece types of Capablanca chess under different names.
 *
 * The additional pieces are called Elephant (moves like Rook + Knight),
 * and Hawk (Bishop + Knight). These pieces are kept in reserve at first.
 * They may enter the board on any square of the own base rank immediately
 * after this square has been vacated for the first time. Squares that have
 * been vacant before can not be used for entry, nor squares where captures
 * removed the original pieces. Only one piece may enter per move. There is
 * no obligation to use the reserve pieces.
 *
 * \note Rules: http://en.wikipedia.org/wiki/Seirawan_chess
 *
 */
class LIB_EXPORT SeirawanBoard : public WesternBoard
{
	public:
		/*! Creates a new SeirawanBoard object. */
		SeirawanBoard();

		// Inherited from WesternBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;

	protected:
		/*! Special piece types for Seirawan variants. */
		enum SeirawanPieceType
		{
			Hawk = King + 1, //!< Hawk = Princess (knight + bishop)
			Elephant	 //!< Elephant = Empress (knight + rook)
		};

		// Inherited from WesternBoard
		virtual bool variantHasDrops() const;
		virtual bool variantHasChanneling(Side side, int square) const;
		virtual bool variantHasOptionalPromotions() const;
		virtual QList< Piece > reservePieceTypes() const;
		virtual void addPromotions(int sourceSquare,
					   int targetSquare,
					   QVarLengthArray<Move>& moves) const;
		virtual bool vSetFenString(const QStringList& fen);
		virtual bool parseCastlingRights(QChar c);
		virtual QString vFenString(FenNotation notation) const;
		virtual QString lanMoveString(const Move& move);
		virtual QString sanMoveString(const Move& move);
		virtual Move moveFromSanString(const QString& str);
		virtual Move moveFromLanString(const QString& str);
		virtual void vMakeMove(const Move& move,
				       BoardTransition* transition);
		virtual void vUndoMove(const Move& move);
		virtual void generateMovesForPiece(QVarLengthArray<Move>& moves,
						   int pieceType,
						   int square) const;
	private:
		QMap<int, int> m_squareMap;
		void insertIntoSquareMap(int square, int count = 0);
		void updateSquareMap(const Move& move, int increment);
		enum direction { forward, backward };
		/*
		 * This method to converts reserve piece types into/from
		 * corresponding "virtual" helper piece types. Used for
		 * disambiguation of channeling after castling towards GUI.
		 */
		static int rookSquareChanneling(int promotion, direction d);
};

} // namespace Chess
#endif // SEIRAWANBOARD_H
