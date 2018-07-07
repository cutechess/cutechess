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

#ifndef MAKRUKBOARD_H
#define MAKRUKBOARD_H

#include "shatranjboard.h"

namespace Chess {

/*!
 * \brief A board for Makruk (Thai Chess)
 *
 * Makruk Chess is a variant of chess popular and with a long tradition
 * in Thailand. It is very similar to Ouk Cha Trang (Cambodian Chess).
 *
 * The Pawn, Knight, Rook, Ferz, and King are all traditional pieces with
 * movements known from Chaturanga and Shatranj. In Makruk these pieces
 * are named differently (with other meanings).
 *
 * The Elephant (Gaja, Alfil) was replaced by the Khon (Base) which moves
 * like Shogi's Silver General: 1 square diagonally or 1 square forward.
 * Met is the Thai name of the Ferz which only moves 1 square diagonally.
 *
 * Makruk has no Pawn double step option and no castling. When reaching the
 * own sixth rank a Pawn (Bia, shell) is promoted to Ferz (Met, grain).
 *
 * Like in standard chess giving mate wins, stalemate is a draw, and two bare
 * kings is a draw. If there is (obviously) not enough material to force a win,
 * the game will be drawn.
 *
 * There is no fifty-move rule. When the last Pawn on the board is captured
 * or promoted the game must be won within 64 moves else the game is drawn.
 *
 * A new counting overrides when there are no Pawns on the board and a side
 * is left with no pieces besides their "bare" King. The starting count equals
 * 1 plus the number of pieces.
 *
 * The counting limit depends on the stronger side:
 * If they have 2 Rooks (with or without other pieces) it is 8 moves,
 * 1 Rook: 16 moves, 2 Khons: 22 moves, 2 Knights: 32 moves, 1 Khon: 44 moves,
 * else 64 moves. When the count *exceeds* the limit the game is drawn.
 * There is no counting as long as there are any unpromoted pawns on the board.
 *
 * Example: For KRRNP vs K the weaker side will count their moves 7 (= 1 + 6)
 * and 8 (the limit defined by 2 Rooks), and if they have a legal move at
 * count 9 the game will end in a draw.
 *
 * \note Rules: http://en.wikipedia.org/wiki/Makruk_chess
 *
 * \sa ShatranjBoard
 * \sa OukBoard
 *
 * \note This implementation can read and write positions in FEN standard
 * notation and per default an adapted FEN notation where the fields have
 * Makruk/Ouk specific meanings to support the counting rules:
 * The castling field is present but unused in Makruk - it is used for initial
 * move status in Ouk. The en passant field "-" is replaced by the active
 * Makruk counting limit in plies. The half move count gives a ply count
 * according to endgame counting rules instead of the reversible ply count
 * of standard chess.
 */
class LIB_EXPORT MakrukBoard : public ShatranjBoard
{
	public:
		/*! Creates a new MakrukBoard object. */
		MakrukBoard();

		// Inherited from ShatranjBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
		virtual Result result();

	protected:
		/*! Piece types for Makruk */
		enum MakrukPieceType
		{
			Bia    = Pawn,	  //!< Shell, Chip
			Ma     = Knight,  //!< Horse
			Khon   = Bishop,  //!< Base, replaces Alfil
			Rua    = Rook,	  //!< Boat
			Met    = Queen,	  //!< Grain: Mantri, Ferz, Advisor
			Khun   = King	  //!< Leader, Chief
		};

		/*! Khon moves like Silver General. */
		static const unsigned SilverGeneralMovement = 256;
		/*!
		 * Returns the relative rank of pawn promotions counting from
		 * 0 for the first rank. The default returns 5 for promotions
		 * on the sixth rank. The formal \a file parameter is unused.
		 * Override this method if the promotion rank is not uniform.
		 *
		 * \sa SittuyinBoard
		 */
		virtual int promotionRank(int file = 0) const;
		/*!
		 * Returns the number of pieces of \a side and of \a pieceType.
		 * NoPiece as \a pieceType counts all pieces of \a side.
		 * This is the default. If \a side is set to NoSide then the
		 * count of all pieces of \a pieceType is given.
		 * A call without arguments returns the total piece count.
		 */
		int pieceCount(Side side = Side::NoSide,
			       int pieceType = Piece::NoPiece) const;
		/*!
		 * Returns maximum count of plies allowed to finish the game:
		 * Limit for Board's Honour or Pieces' Honour counting.
		 */
		virtual int countingLimit() const;
		/*!
		 * Rule sets of Thai and Cambodian Chess
		 */
		enum CountingRules {Makruk, BareKing};
		/*!
		 * Returns type of counting rules to apply. Default: Makruk.
		 * \sa OukBoard
		 */
		virtual CountingRules countingRules() const;
		/*!
		 * Initialize counter history
		 */
		void initHistory();
		/*!
		 * Counts all pieces on the board and updates all counters
		 * of last MoveData entry in move history.
		 */
		void setAllPieceCounters();
		/*!
		 * Returns game result based on specific counting rules
		 */
		Result resultFromCounting() const;
		/*!
		 * Appends generated pseudo-legal pawn moves to \a moves
		 */
		virtual void generatePawnMoves(int sourceSquare,
				       QVarLengthArray<Move>& moves) const;
		/*!
		 * Returns true if material on board is known to be not
		 * sufficient to enforce mate, else false.
		 */
		virtual bool insufficientMaterial() const;

		// Inherited from ShatranjBoard
		virtual void vInitialize();
		virtual QString vFenString(FenNotation notation) const;
		virtual bool vSetFenString(const QStringList& fen);
		virtual bool inCheck(Side side, int square = 0) const;
		virtual void vMakeMove(const Move& move,
				       BoardTransition* transition);
		virtual void vUndoMove(const Move& move);
		virtual void generateMovesForPiece(QVarLengthArray<Move>& moves,
						   int pieceType,
						   int square) const;

	private:
		QVarLengthArray<int> m_silverGeneralOffsets[2];
		int m_promotionRank;
		enum CountingRules m_rules;
		bool m_useWesternCounting;
		// Data for reversing/unmaking a move
		struct MoveData
		{
			bool piecesHonour;
			int countingLimit;
			int plyCount;
			int totalPlies;
			int pieceCount[King + 1][2];
		};
		QVarLengthArray<MoveData> m_history;

		int material() const;
};

} // namespace Chess
#endif // MAKRUKBOARD_H
