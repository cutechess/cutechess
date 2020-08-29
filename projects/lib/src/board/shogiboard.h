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

#ifndef SHOGIBOARD_H
#define SHOGIBOARD_H

#include "board.h"

namespace Chess {

/*!
 * \brief A board for Shogi
 *
 * Shōgi is a Japanese form of chess where captured
 * pieces can be brought back ("dropped") into the game.
 *
 * It is played on a 9x9 board. The piece set traditionally consists of
 * flat five-sided pieces with their names written on them. Such sets
 * have been in use at least since the 11th century AD.
 *
 * Shōgi emerged from the earlier game of Shō Shōgi by the reinsertion
 * of captured pieces as own pieces (15th century AD) and removal of the
 * Drunk Elephant piece by imperial order (16th century AD).
 *
 * Shōgi has eight types of original pieces. Either side starts with a
 * set of 9 x Pawn (on the third rank), 2 x Lance, 2 x Knight, 2 x Silver
 * (General), 2 x Gold (General), 1 x Bishop, 1 x Rook, 1 x King.
 * The latter three types move like in standard chess. The Pawn moves and
 * captures 1 square straight forward. The Lance moves and captures like
 * a Rook, but only forwards. Similarly, the Knight leaps like a chess
 * Knight, but only in the two forward directions. The Silver goes one
 * square diagonally or forward (5 moves). The Gold goes like a King but
 * not diagonally backward (6 moves).
 *
 * All these pieces except King and Gold General have the option to promote
 * if their move starts or ends in the opponent's third of the board.
 * A Pawn or Lance reaching the highest rank must promote. A Knight must
 * promote when reaching the highest two ranks. All promoted Pawns, Lances,
 * Knights, and Silvers move like Gold Generals. A promoted Bishop moves
 * like Bishop or King, a promoted Rook like Rook or King.
 *
 * A captured piece switches sides and goes into the hand of the capturing
 * side. Promoted pieces change back to their original types. A player may
 * drop a piece in hand onto an empty square of the board. This counts as a
 * move. Pawns, Lances, and Knights must not be dropped onto the highest
 * ranks where they would be unable to move. Pawns must not be dropped onto
 * files with own Pawns and must not be dropped to give mate immediately.
 *
 * A side must avert an attack on their King (check, ote 王手) immediately.
 * It is forbidden to enter check. If there is no legal move to avert check
 * then the side in check loses by checkmate (tsumi 詰み).
 *
 * When a game position occurs four times with the same player to move and
 * the same pieces in hand for each player the game is drawn by repetition
 * (sennichite, 千日手). If the situation has arisen from a perpetual check
 * then the side giving check loses.
 *
 * When both Kings have entered their opponent's third of the board it may
 * be difficult to checkmate. To break an Impassé (持将棋 jishōgi), the
 * 27-points rule is used: A player not in check having at least ten pieces
 * in their promotion zone besides their King is awarded one point for each
 * of those pieces, but no points for the King and five points for every
 * Bishop or Rook (also when in their promoted states). Points are awarded
 * in the same way for the pieces in hand. A player wins if they have
 * *more* than 27 points. The Second Player (Gote) also wins having just
 * 27 points.
 *
 * \note Rules: http://en.wikipedia.org/wiki/Shogi
 *              http://www.shogi.net/fesa/pdf/FESA%20rules.pdf
 */
class LIB_EXPORT ShogiBoard : public Board
{
	public:
		/*! Creates a new ShogiBoard object. */
		ShogiBoard();

		// Inherited from Board
		virtual Board* copy() const;
		virtual QList<Piece> reservePieceTypes() const;
		virtual QString variant() const;
		virtual bool variantHasDrops() const;
		virtual bool variantHasOptionalPromotions() const;
		virtual QString defaultFenString() const;
		virtual CoordinateSystem coordinateSystem() const;
		virtual int width() const;
		virtual int height() const;
		virtual Result result();

	protected:
		/*!
		 * Piece types for Shogi.
		 */
		enum ShogiPieceType
		{
			Pawn = 1,		//!< Foot Soldier P (Fuhyō) FU
			Lance,			//!< Incense Chariot L (Kyōsha) KY
			Knight,			//!< Cassia Horse N (Keima) KE
			SilverGeneral,		//!< Silver General S (Ginshō) GI
			GoldGeneral,            //!< Gold General G (Kinshō) KI
			Bishop,			//!< Angle Mover B (Kakugyō) KA
			Rook,			//!< Flying Chariot R (Hisha) HI
			King,			//!< King General K (Ōshō) OU
						//!< Jeweled General K (GyoKushō) OU
			/*!
			 * Promoted piece types for Shogi.
			 * When they are captured they get demoted back.
			 */
			PromotedPawn,		//!< Reaches Gold +P (Tokin) TO
			PromotedLance,		//!< Promoted Incense +L (Narikyō) NY
			PromotedKnight,		//!< Promoted Cassia +N (Narikei) NK
			PromotedSilver,		//!< Promoted Silver +S (Narigin) NG
			PromotedBishop,		//!< Dragon *Horse* +B (Ryūma) UM
			PromotedRook		//!< *Dragon* King +R (Ryūō) RY
		};

		/*! Movement mask for Knight moves. */
		static const unsigned KnightMovement = 2;
		/*! Movement mask for Bishop moves. */
		static const unsigned BishopMovement = 4;
		/*! Movement mask for Rook moves. */
		static const unsigned RookMovement = 8;
		/*! Movement mask for Ferz move pattern. */
		static const unsigned FerzMovement = 16;
		/*! Movement mask for Wazir move pattern. */
		static const unsigned WazirMovement = 64;
		/*! Movement mask for Lance moves. */
		static const unsigned LanceMovement = 128;
		/*! Movement mask for Silver General moves. */
		static const unsigned SilverMovement = 256;
		/*! Movement mask for Gold General moves. */
		static const unsigned GoldMovement = 512;

		/*!
		 * Returns promoted piece type corresponding to normal \a type.
		 */
		virtual int promotedPieceType(int type) const;
		/*!
		 * Returns the lowest (relative) rank of the promotion zone.
		 */
		virtual int promotionRank() const;
		/*!
		 * Returns original piece type corresponding to promoted \a type.
		 */
		virtual int normalPieceType(int type) const;
		/*!
		 * Returns whether the current position has no pieces on
		 * forbidden ranks.
		 */
		virtual bool ranksAreAllowed() const;
		/*!
		 * Returns true if the impassé point rule is active else false.
		 */
		virtual bool hasImpassePointRule() const;
		/*!
		 * Criteria of impasse rule limit given material value of
		 * \a points and the number of \a pieces of the side to move
		 * in the promotion zone.
		 */
		virtual Result impassePointRule(int points, int pieces) const;

		// Inherited from Board
		virtual int reserveType(int pieceType) const;
		virtual void vInitialize();
		virtual QString vFenString(FenNotation notation) const;
		virtual bool vSetFenString(const QStringList& fen);
		virtual QString lanMoveString(const Move& move);
		virtual QString sanMoveString(const Move& move);
		virtual Move moveFromLanString(const QString& str);
		virtual Move moveFromSanString(const QString& str);
		virtual void vMakeMove(const Move& move,
				       BoardTransition* transition);
		virtual void vUndoMove(const Move& move);
		virtual void generateMovesForPiece(QVarLengthArray<Move>& moves,
						   int pieceType,
						   int square) const;
		virtual bool isLegalPosition();
		virtual bool vIsLegalMove(const Move& move);
		virtual bool inCheck(Side side, int square = 0) const;

	private:
		// Data for reversing/unmaking a move
		struct MoveData
		{
			Piece capture;
		};

		bool rankIsAllowed(int pieceType, int square) const;
		bool fileIsAllowed(int pieceType, int square) const;
		bool inPromotionZone(int square) const;
		Result resultFromImpassePointRule() const;

		int m_kingSquare[2];
		int m_promotionRank;
		int m_minIndex;
		int m_maxIndex;
		int m_plyOffset;
		bool m_multiDigitNotation;
		bool m_hasImpassePointRule;
		int m_checks[2];

		QVarLengthArray<int> m_bishopOffsets;
		QVarLengthArray<int> m_rookOffsets;
		QVarLengthArray<int> m_lanceOffsets[2];
		QVarLengthArray<int> m_knightOffsets[2];
		QVarLengthArray<int> m_silverGeneralOffsets[2];
		QVarLengthArray<int> m_goldGeneralOffsets[2];
		QVector<MoveData> m_history;
};

} // namespace Chess
#endif // SHOGIBOARD_H
