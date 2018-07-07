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

#ifndef OUKBOARD_H
#define OUKBOARD_H

#include "makrukboard.h"

namespace Chess {

/*!
 * \brief A board for Ouk Chatrang (Ouk Khmer, Cambodian Chess)
 *
 * Ouk Chatrang is a variant of chess popular and with a long tradition
 * in Cambodia. It is closely related to Chaturanga and Makruk.
 *
 * Cambodian Chess (Ouk) and Thai Chess (Makruk) are essentially the same game,
 * but the pieces are named differently.
 *
 * Ouk has additional moves, which have been abandoned in Makruk:
 *
 * The King has the option to make an initial leap sideways like a Horse
 * (Knight), but only if not in check. The Advisor (Maiden, Neang) may leap
 * straight forward two squares on its initial move. These moves cannot capture.
 *
 * \note Rules: http://history.chess.free.fr/cambodian/Cambodian%20Chess%20Games.htm
 *              http://www.khmerinstitute.com/culture/ok.html
 *
 * \sa MakrukBoard
 * \sa ShatranjBoard
 *
 */
class LIB_EXPORT OukBoard : public MakrukBoard
{
	public:
		/*! Creates a new OukBoard object. */
		OukBoard();

		// Inherited from MakrukBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;

	protected:
		/*! Piece types for ouk variants. */
		enum OukPieceType
		{
			Fish = Pawn,	//!< Trey, Trei
			Horse = Knight,	//!< Ses, Se
			Pillar = Bishop,//!< Kol, Koul
			Boat = Rook,	//!< Tuuk
			Maiden = Queen,	//!< Neang (Met)
			King		//!< Sdaach, Ang
		};

		// Inherited from MakrukBoard
		virtual CountingRules countingRules() const;
		virtual bool parseCastlingRights(QChar c);
		virtual QString vFenString(FenNotation notation) const;
		virtual bool vSetFenString(const QStringList& fen);
		virtual bool inCheck(Side side, int square = 0) const;
		virtual void generateMovesForPiece(QVarLengthArray<Move>& moves,
						   int pieceType,
						   int square) const;
		virtual void vMakeMove(const Move& move,
				       BoardTransition* transition);
		virtual void vUndoMove(const Move& move);

	private:
		void updateCounter(Move m, int increment);
		const QMap<OukPieceType, int>m_initialSquare[2];
		const struct
		{
			OukPieceType type;
			int offset;
		} m_initialOffsets[3];
		QMap<OukPieceType, int> m_moveCount[2];
};


/*!
 * \brief A board for Kar Ouk
 *
 * Kar Ouk is a variant of Ouk Chatrang. It has been popular in Cambodia.
 * In this variant the side that gives check to their opponent wins.
 *
 * \sa OukBoard
 *
 */

class LIB_EXPORT KarOukBoard : public OukBoard
{
	public:
		/*! Creates a new KarOukBoard object. */
		KarOukBoard();

		// Inherited from OukBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
		virtual Result result();
};

} // namespace Chess
#endif // OUKBOARD_H
