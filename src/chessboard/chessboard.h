#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QtGlobal>
#include <QVector>
#include <QString>
#include "chess.h"
#include "chessmove.h"


/*!
 * \brief An internal chessboard.
 *
 * Board's main purposes are:
 * - keeping track of the game, and not just piece positions, but also castling
 *   rights, en passant square, played moves, repetitions, etc.
 * - verifying the legality of moves played
 * - parsing and generating strings for moves and positions in notations
 *   such as FEN, SAN, and Coordinate notation
 *
 * \internal
 * The board representation is (width + 2) x (height + 4), so a
 * traditional 8x8 board would be 10x12, and stored in a one-dimensional
 * vector with 10 * 12 = 120 elements.
 */
class Chess::Board
{
	public:
		/*! Creates a new Board of a specific chess variant. */
		explicit Board(Chess::Variant variant = Chess::StandardChess);
		
		/*!
		* Sets the board position according to a FEN string.
		*
		* The \a fen string can be in standard FEN, X-FEN or
		* Shredder FEN format.
		*
		* FEN: http://en.wikipedia.org/wiki/Forsyth-Edwards_Notation
		* \n X-FEN: http://en.wikipedia.org/wiki/X-FEN
		*/
		bool setBoard(const QString& fen);
		
		/*!
		 * Returns the FEN string of the current board position.
		 *
		 * The FEN string is in X-FEN format, which is fully
		 * compatible with standard FEN.
		 */
		QString fenString() const;
		
		/*! Makes a chess move on the board. */
		void makeMove(const Chess::Move& move);
		
		/*! Reverses the last move. */
		void undoMove();
		
		/*! Returns true if \a move is legal. */
		bool isLegalMove(const Chess::Move& move);
		
		/*!
		 * Converts a Move into a string.
		 *
		 * Supported notations are Long Algebraic notation and
		 * Standard Algebraic notation (SAN).
		 *
		 * \sa moveFromString()
		 */
		QString moveString(const Chess::Move& move,
		                   Chess::MoveNotation notation);
		
		/*!
		* Converts a move string into a Move.
		*
		* The notation is automatically detected, and can be either
		* Long Algebraic notation or Standard Algebraic notation (SAN).
		*
		* \sa moveString()
		*/
		Chess::Move moveFromString(const QString& str);
		
		/*! Returns the board's chess variant. */
		Chess::Variant variant() const;
		
		/*! Returns the result of the game. */
		Result result();
		
		/*! Prints an ASCII version of the board. */
		void print() const;
		
	private:
		/*! The castling side of a castling move. */
		enum CastlingSide
		{
			QueenSide,	//!< Left side.
			KingSide	//!< Right side.
		};
		
		/*!
		* \brief A struct for castling rights.
		*/
		struct CastlingRights
		{
			/*!
			* The rook squares for both castling sides for both players.
			*
			* The format is: \c rookSquare[ChessSide][CastlingSide]
			* A value of 0 means no castling rights.
			*
			* Example:
			* If the value of \c rookSquare[White][QueenSide] is 0, then
			* castling is not possible for White on QueenSide.
			*/
			int rookSquare[2][2];
		};
		
		/*!
		* \brief A struct for storing the move history of a game.
		*
		* After each move, a Board object adds a MoveData object to
		* its history. This allows all moves to be completely reversible,
		* something that couldn't be done with plain Move objects.
		*/
		struct MoveData
		{
			//! A chess move.
			Chess::Move move;
			//! Piece type of the captured piece.
			int capture;
			//! Target square of an en-passant capture before the move.
			int enpassantSquare;
			//! Castling rights before the move.
			CastlingRights castlingRights;
			//! Hash key before the move.
			quint64 key;
			//! Number of successive reversible moves before the move.
			int reversibleMoveCount;
		};
		
		/*!
		* \brief Temporary storage for FEN position data.
		*
		* When parsing a FEN string, the data related to the position
		* goes here. If all parts of it turn out to be valid, the data
		* will be copied to a Board object and the FenData object
		* can be discarded.
		*/
		class FenData
		{
			public:
				//! Initializes (zeroes) the FEN data.
				FenData();
				
				//! Castling rights.
				CastlingRights cr;
				//! The contents of the board.
				QVector<int> squares;
				//! Square indexes for the king on both sides.
				int kingSquare[2];
				//! Is support for random starting positions required?
				bool isRandom;
		};
		
		
		/*! Converts a square index into a Chess::Square object. */
		Chess::Square chessSquare(int index) const;
		
		/*! Converts a Chess::Square object into a square index. */
		int squareIndex(const Chess::Square& square) const;
		
		/*! Returns true if \a square is on the board. */
		bool isValidSquare(const Chess::Square& square) const;
		
		/*! Generates moves for a hopping piece at \a sourceSquare. */
		void generateHoppingMoves(int sourceSquare,
		                          const QVector<int>& offsets,
		                          QVector<Chess::Move>& moves) const;
		
		/*! Generates moves for a sliding piece at \a sourceSquare. */
		void generateSlidingMoves(int sourceSquare,
		                          const QVector<int>& offsets,
		                          QVector<Chess::Move>& moves) const;
		
		/*! Generates castling moves. */
		void generateCastlingMoves(QVector<Chess::Move>& moves) const;
		
		/*! Generates moves for a pawn at \a sourceSquare. */
		void generatePawnMoves(int sourceSquare,
		                       QVector<Chess::Move>& moves) const;
		
		/*!
		 * Generates pseudo-legal moves.
		 * \sa legalMoves()
		 */
		void generateMoves(QVector<Chess::Move>& moves) const;
		
		/*!
		 * Adds pawn promotions (from \a sourceSquare to \a targetSquare)
		 * to a vector (\a moves).
		 */
		void addPromotions(int sourceSquare,
		                   int targetSquare,
		                   QVector<Chess::Move>& moves) const;
		
		/*! Returns true if side to move can castle to \a castlingSide. */
		bool canCastle(int castlingSide) const;
		
		/*!
		 * Returns true if player \a side is under attack in \a square.
		 *
		 * If \a square is 0, then the player's king square is used.
		 */
		bool inCheck(int side, int square = 0) const;
		
		/*!
		 * Returns true if the position is legal.
		 *
		 * This method only checks if the side to move can capture the opposing
		 * king, or if the last move (if any) was an illegal castling.
		 * So this is not sufficient for verifying FEN strings.
		 */
		bool isLegalPosition() const;
		
		/*! Calculates a Zobrist key for the current position. */
		void initZobristKey();
		
		/*!
		 * Parses the castling rights part of a FEN/X-FEN string and stores
		 * the data in \a fd.
		 */
		bool parseCastlingRights(FenData& fd, QChar c) const;
		
		/*!
		 * Returns the castling rights part of the current position's
		 * FEN/X-FEN string.
		 */
		QString castlingRightsString() const;
		
		/*! Converts a Move into a string in Long Algebraic notation. */
		QString longAlgebraicMoveString(const Chess::Move& move) const;
		
		/*! Converts a Move into a string in Standard Algebraic Notation. */
		QString sanMoveString(const Chess::Move& move);
		
		/*! Converts a string in Long Algebraic Notation into a Move. */
		Chess::Move moveFromLongAlgebraicString(const QString& str) const;
		
		/*! Converts a string in Standard Algebraic Notation into a Move. */
		Chess::Move moveFromSanString(const QString& str);
		
		/*! Returns a vector of legal chess moves. */
		QVector<Chess::Move> legalMoves();
		
		/*! Returns the Zobrist key for the current position. */
		quint64 key() const;
		
		/*! Returns the number of times the current position was reached. */
		int repeatCount() const;
		
		Chess::Variant m_variant;
		bool m_isRandom;
		int m_width;
		int m_height;
		int m_arwidth;
		int m_side;
		int m_sign;
		int m_kingSquare[2];
		int m_enpassantSquare;
		int m_reversibleMoveCount;
		QVector<int> m_squares;
		QVector<MoveData> m_history;
		CastlingRights m_castlingRights;
		int m_castleTarget[2][2];
		quint64 m_key;
		
		QVector<int> m_knightOffsets;
		QVector<int> m_bishopOffsets;
		QVector<int> m_rookOffsets;
};


#endif // CHESSBOARD
