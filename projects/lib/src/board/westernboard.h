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
	Q_OBJECT

	public:
		/*! Creates a new WesternBoard object. */
		WesternBoard(WesternZobrist* zobrist,
			     QObject* parent);

		// Inherited from Board
		virtual int width() const;
		virtual int height() const;
		virtual Result result();

	protected:
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

		/*! Movement mask for Knight moves. */
		static const unsigned KnightMovement = 2;
		/*! Movement mask for Bishop moves. */
		static const unsigned BishopMovement = 4;
		/*! Movement mask for Rook moves. */
		static const unsigned RookMovement = 8;

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
		/*! Returns the number of consecutive reversible moves made. */
		int reversibleMoveCount() const;
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
				       QVarLengthArray<int>& changedSquares);
		virtual void vUndoMove(const Move& move);
		virtual void generateMovesForPiece(QVarLengthArray<Move>& moves,
						   int pieceType,
						   int square) const;
		virtual bool vIsLegalMove(const Move& move);
		virtual bool isLegalPosition();
		virtual int captureType(const Move& move) const;

	private:
		enum CastlingSide
		{
			QueenSide,
			KingSide,
			NoCastlingSide
		};

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
		void setEnpassantSquare(int square);
		void setCastlingSquare(Side side,
				       CastlingSide cside,
				       int square);

		int m_arwidth;
		int m_sign;
		int m_kingSquare[2];
		int m_enpassantSquare;
		int m_reversibleMoveCount;
		bool m_kingCanCapture;
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
