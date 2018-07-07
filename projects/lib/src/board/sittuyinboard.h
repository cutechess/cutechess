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

#ifndef SITTUYINBOARD_H
#define SITTUYINBOARD_H

#include "makrukboard.h"

namespace Chess {

/*!
 * \brief A board for Sit-tu-yin (Myanmar Traditional Chess, Burmese Chess)
 *
 * Sittuyin is the traditional variant of chess in Myanmar.
 * It is an old game (9th century) closely related to Makruk and Chaturanga.
 * There has been a wide variety of local or temporary rules.
 *
 * Each side begins with four Pawns on the left side on their third rank and
 * four Pawns on the right side on their fourth rank.
 *
 * In the initial phase of the game the red side places their pieces behind
 * their Pawns. After that the black side places their pieces. Rooks must be
 * placed on the back rank. After all pieces have been placed the red side
 * makes their first move.
 *
 * Sittuyin pieces move like Makruk (Thai Chess) pieces: The movements of
 * King, Rook, Knight, and Pawn are the same as in Makruk, Shatranj, and
 * chess. There are no "modern" moves: no pawn double steps, and no castling.
 * The General moves one square diagonally. The Elephant has the same move-
 * ment but can also make a step straight forward.
 *
 * If a side has no General then a Pawn can be promoted to General. This
 * can only be done by a side's last Pawn or within the opponent's half from
 * a square that is part of a main diagonal. Promotion counts as a move of
 * its own. When promoting the Pawn either remains on its current square or
 * makes a General's move. The promotion move must not give check or capture
 * an opponent piece. Pawn promotion is optional, not oligatory: The Pawn can
 * also make a normal move or capture.
 *
 * Checkmating the opponent King wins. Stalemating is not allowed. A draw can
 * be claimed if no pawns have been moved and no captures have been made for
 * fifty consecutive moves. A game will be drawn if it is impossible for
 * the remaining pieces to give mate. There is no 3-fold repetition rule.
 *
 * Endgames with King and Rook, or King, Elephant and General, or King,
 * Knight and General against a lone King must be won within a defined number
 * of moves, else the game is drawn.
 *
 * \note Rules: The Official Rules by the Myanmar Chess Federation
 * (as described by Maung Maung Lwin).
 * \note: Set-up phase with alternating red and black piece drops.
 * \note: Board will adjudicate against side giving stalemate.
 *
 * \sa MakrukBoard
 * \sa ShatranjBoard
 *
 */
class LIB_EXPORT SittuyinBoard : public MakrukBoard
{
	public:
		/*! Creates a new SittuyinBoard object. */
		SittuyinBoard();

		// Inherited from MakrukBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
		virtual bool variantHasDrops() const;
		virtual bool variantHasOptionalPromotions() const;

	protected:
		/*! Piece types for sittuyin variants. */
		enum SittuyinPieceType
		{
			Pawn = 1,          //!< Ne: Feudal Lord
			Knight,            //!< Myin: Horse
			Elephant = Bishop, //!< Sin: Elephant
			Rook,              //!< Yahhta: Chariot
			General = Queen,   //!< Sit-ke: General
			King               //!< Min-gyi: King
		};

		// Inherited from MakrukBoard
		virtual QList< Piece > reservePieceTypes() const;
		virtual bool kingsCountAssertion(int whiteKings, int blackKings) const;
		virtual void generatePawnMoves(int sourceSquare,
					       QVarLengthArray<Move>& moves) const;
		virtual void generateMovesForPiece(QVarLengthArray<Move>& moves,
						   int pieceType,
						   int square) const;
		virtual void addPromotions(int sourceSquare,
					   int targetSquare,
					   QVarLengthArray< Move >& moves) const;
		virtual bool vSetFenString(const QStringList& fen);
		virtual void vMakeMove(const Move& move,
				       BoardTransition* transition);
		virtual void vUndoMove(const Move& move);
		virtual int promotionRank(int file = 0) const;
		virtual bool vIsLegalMove(const Move& move);
		virtual bool isLegalPosition();
		virtual int countingLimit() const;
		virtual CountingRules countingRules() const;
		virtual Result result();

	private:
		bool m_inSetUp;
		bool inSetup() const;
};

} // namespace Chess
#endif // SITTUYINBOARD_H
