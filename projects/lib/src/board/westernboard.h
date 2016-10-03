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

#ifndef WESTERNBOARD_H
#define WESTERNBOARD_H

#include "board.h"

namespace Chess {

class WesternZobrist;


/*!
 * \brief A board for western chess variants
 *
 * WesternBoard serves as the overclass for all western variants.
 * In addition to possibly unique pieces, a western variant has
 * the same pieces as standard chess, the same rules for castling,
 * en-passant capture, promotion, etc.
 *
 * WesternBoard implements the rules of standard chess, including
 * check, checkmate, stalemate, promotion, 3-fold repetition,
 * 50 move rule and draws by insufficient material.
 */
class LIB_EXPORT WesternBoard : public Board
{
	public:
		/*! Basic piece types for western variants. */
		enum WesternPieceType
		{
			Pawn = 1,	//!< Pawn
			Knight,		//!< Knight
			Bishop,		//!< Bishop
			Rook,		//!< Rook
			Queen,		//!< Queen
			King		//!< King
		};

		/*! Creates a new WesternBoard object. */
		WesternBoard(WesternZobrist* zobrist);

		// Inherited from Board
		virtual int width() const;
		virtual int height() const;
		virtual Result result();
		virtual int reversibleMoveCount() const;

	protected:
		/*! The king's castling side. */
		enum CastlingSide
		{
			QueenSide,	//!< Queen side (O-O-O)
			KingSide,	//!< King side (O-O)
			NoCastlingSide	//!< No castling side
		};

		/*! Movement mask for Knight moves. */
		static const unsigned KnightMovement = 2;
		/*! Movement mask for Bishop moves. */
		static const unsigned BishopMovement = 4;
		/*! Movement mask for Rook moves. */
		static const unsigned RookMovement = 8;

		/*! Types of Pawn moves. */
		enum StepType: const char
		{
			 Free = (char) 1,      //!< May move if target is empty
			 Capture = (char) 2,   //!< Capture opposing piece only
			 /* FreeOrCapture = Free|Capture, //!< like King or Sergeant */
			 None =(char) 0        //!< Cannot move here
		} stepTypeEnum;

		/*!
		 * Movement mask for Pawn moves.
		 * Forward: Left diagonal, straight, right diagonal.
		 * \sa BerolinaBoard
		 *
		 * Default: A pawn can step straight ahead onto a free square or
		 * capture diagonally forward.
		 */
		struct PawnStep { StepType type; int file; };
		QList<PawnStep>  pawnSteps = { {Capture, -1}, {Free, 0}, {Capture, 1} }; 
		/*! Helper function for Pawn moves. Returns true if two or more
		 * moves of the given \a type are specified in pawnSteps. */
		bool pawnAmbiguity( StepType type = StepType::Free ) const;

		/*!
		 * Returns true if the king can capture opposing pieces.
		 * The default value is true.
		 * \sa AtomicBoard
		 */
		virtual bool kingCanCapture() const;
		/*!
		 * Adds pawn promotions to a move list.
		 *
		 * This function is called when a pawn can promote by
		 * moving from \a sourceSquare to \a targetSquare.
		 * This function generates all the possible promotions
		 * and adds them to \a moves.
		 */
		virtual void addPromotions(int sourceSquare,
					   int targetSquare,
					   QVarLengthArray<Move>& moves) const;
		/*! Returns the king square of \a side. */
		int kingSquare(Side side) const;
		/*! Returns the current en-passant square. */
		int enpassantSquare() const;
		/*!
		 * Returns true if \a side has a right to castle on \a castlingSide;
		 * otherwise returns false.
		 *
		 * \note Even if this function returns true, castling may not be
		 * a legal move in the current position.
		 */
		bool hasCastlingRight(Side side, CastlingSide castlingSide) const;
		/*!
		 * Removes castling rights at \a square.
		 *
		 * If one of the players has a rook at \a square, the rook can't
		 * be used for castling. This function should be called when a
		 * capture happens at \a square.
		 */
		void removeCastlingRights(int square);
		/*!
		 * Returns true if \a side is under attack at \a square.
		 * If \a square is 0, then the king square is used.
		 */
		virtual bool inCheck(Side side, int square = 0) const;

		// Inherited from Board
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
		virtual bool vIsLegalMove(const Move& move);
		virtual bool isLegalPosition();
		virtual int captureType(const Move& move) const;

	private:
		struct CastlingRights
		{
			// Usage: 'rookSquare[Side][CastlingSide]'
			// A value of zero (square 0) means no castling rights
			int rookSquare[2][2];
		};

		// Data for reversing/unmaking a move
		struct MoveData
		{
			Piece capture;
			int enpassantSquare;
			int enpassantTarget;
			CastlingRights castlingRights;
			CastlingSide castlingSide;
			int reversibleMoveCount;
		};

		void generateCastlingMoves(QVarLengthArray<Move>& moves) const;
		void generatePawnMoves(int sourceSquare,
				       QVarLengthArray<Move>& moves) const;

		bool canCastle(CastlingSide castlingSide) const;
		QString castlingRightsString(FenNotation notation) const;
		bool parseCastlingRights(QChar c);
		CastlingSide castlingSide(const Move& move) const;
		void setEnpassantSquare(int square, int target=0);
		void setCastlingSquare(Side side,
				       CastlingSide cside,
				       int square);
		/*! Helper for Pawn moves. Returns square offset for the
		 *  given \a step with orientation \a sign. */
		inline int getPawnPush( const PawnStep& ps, int sign ) const;

		int m_arwidth;
		int m_sign;
		int m_kingSquare[2];
		int m_enpassantSquare;
		int m_enpassantTarget;
		int m_reversibleMoveCount;
		bool m_kingCanCapture;
		bool m_pawnAmbiguous;
		QVector<MoveData> m_history;
		CastlingRights m_castlingRights;
		int m_castleTarget[2][2];
		const WesternZobrist* m_zobrist;

		QVarLengthArray<int> m_knightOffsets;
		QVarLengthArray<int> m_bishopOffsets;
		QVarLengthArray<int> m_rookOffsets;
};


} // namespace Chess
#endif // WESTERNBOARD_H
