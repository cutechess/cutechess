#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QtGlobal>
#include <QObject>
#include <QVector>
#include <QString>
#include <QStringList>
#include "chess.h"
#include "piece.h"
#include "square.h"
#include "result.h"
#include "variant.h"
#include "chessmove.h"

class GenericMove;
namespace Chess {

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
class LIB_EXPORT Board : public QObject
{
	Q_OBJECT
	
	public:
		/*! Creates a new Board of a specific chess variant. */
		explicit Board(Variant variant = Variant::Standard,
		               QObject* parent = 0);
		
		/*! The copy constructor. */
		explicit Board(const Board& other);
		
		/*!
		 * Sets the chess variant to \a variant.
		 *
		 * The board's contents will be reset to an unusable state,
		 * so a call to setBoard() is needed after the variant has
		 * been changed.
		 */
		void setVariant(Variant variant);
		
		/*!
		 * Sets the board position according to a FEN string.
		 *
		 * The \a fen string can be in standard FEN, X-FEN or
		 * Shredder FEN format.
		 *
		 * FEN: http://en.wikipedia.org/wiki/Forsyth-Edwards_Notation
		 * \n X-FEN: http://en.wikipedia.org/wiki/X-FEN
		 *
		 * Returns true if successfull.
		 */
		bool setBoard(const QString& fen);
		
		/*!
		 * Sets the board position to the default starting position
		 * of the current chess variant.
		 *
		 * Returns true if successfull.
		 */
		bool setBoard();
		
		/*!
		 * Returns the FEN string of the current board position.
		 *
		 * The FEN string is in X-FEN format, which is fully
		 * compatible with standard FEN.
		 */
		QString fenString(FenNotation notation = XFen) const;
		
		/*! Returns the FEN string of the game's starting position. */
		QString startingFen() const;
		
		/*! Returns the side to move. */
		Side sideToMove() const;
		
		/*!
		 * Makes a chess move on the board.
		 * \param sendSignal If true, the \a moveMade signal is emitted
		 * once, and the \a squareChanged signal for every changed square.
		 *
		 * \sa moveMade()
		 * \sa squareChanged()
		 */
		void makeMove(const Move& move, bool sendSignal = false);
		
		/*! Reverses the last move. */
		void undoMove();
		
		/*! Returns true if \a move is legal. */
		bool isLegalMove(const Move& move);
		
		/*!
		 * Returns true if \a move repeats a position that was
		 * reached earlier in the game.
		 */
		bool isRepeatMove(const Move& move);
		
		/*!
		 * Converts a Move into a string.
		 *
		 * \note The board must be in a position where \a move can be made.
		 * \sa moveFromString()
		 */
		QString moveString(const Move& move, MoveNotation notation);
		
		/*!
		 * Converts a move string into a Move.
		 *
		 * The notation is automatically detected, and can be either
		 * Long Algebraic notation or Standard Algebraic notation (SAN).
		 *
		 * \note The board must be in a position where the move can be made.
		 * \sa moveString()
		 */
		Move moveFromString(const QString& str);
		
		/*! Converts an opening book move into a Move. */
		Move moveFromBook(const GenericMove& bookMove) const;
		
		/*! Returns a vector of played moves. */
		QVector<Move> moveHistory() const;
		
		/*! Returns the board's chess variant. */
		Variant variant() const;
		
		/*! Returns the result of the game. */
		Result result();
		
		/*! Returns the board's width in squares. */
		int width() const;
		
		/*! Returns the board's height in squares. */
		int height() const;
		
		/*! Returns the chess piece at \a square. */
		Piece pieceAt(const Square& square) const;
		
		/*! Prints an ASCII version of the board. */
		void print() const;
		
		/*! Returns the Zobrist key. */
		quint64 key() const;
		
		/*! Converts a square index into a Square object. */
		Square chessSquare(int index) const;
		
		/*! Returns a vector of legal chess moves. */
		QVector<Move> legalMoves();


		/*!
		 * Returns a Unicode version of a SAN move string.
		 *
		 * \param side The side that makes the move
		 * \param sanMove The move string in Standard Algebraic Notation
		 */
		static QString unicodeSanMove(Side side, const QString& sanMove);
	
	signals:
		/*!
		 * This signal is emitted when a move is made on the board.
		 * \sa makeMove()
		 */
		void moveMade(const Chess::Square& source,
			      const Chess::Square& target);
		/*!
		 * This signal is emitted for every changed square when
		 * a move is made on the board.
		 * \sa makeMove()
		 */
		void squareChanged(const Chess::Square& square);
		/*!
		 * This signal is emitted when the board's contents are reset.
		 * \sa setBoard()
		 */
		void boardReset();
		
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
			Move move;
			//! The captured piece.
			Piece capture;
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
				QVector<Piece> squares;
				//! Square indexes for the king on both sides.
				int kingSquare[2];
				//! Is support for random starting positions required?
				bool isRandom;
		};
		
		
		/*! Converts a Square object into a square index. */
		int squareIndex(const Square& square) const;
		
		/*! Returns true if \a square is on the board. */
		bool isValidSquare(const Square& square) const;
		
		/*! Generates moves for a hopping piece at \a sourceSquare. */
		void generateHoppingMoves(int sourceSquare,
		                          const QVector<int>& offsets,
		                          QVector<Move>& moves) const;
		
		/*! Generates moves for a sliding piece at \a sourceSquare. */
		void generateSlidingMoves(int sourceSquare,
		                          const QVector<int>& offsets,
		                          QVector<Move>& moves) const;
		
		/*! Generates castling moves. */
		void generateCastlingMoves(QVector<Move>& moves) const;
		
		/*! Generates moves for a pawn at \a sourceSquare. */
		void generatePawnMoves(int sourceSquare,
		                       QVector<Move>& moves) const;
		
		/*!
		 * Generates pseudo-legal moves for pieces of \a type.
		 *
		 * \note If type is NoPiece (default), moves are generated
		 * for every piece type.
		 * \sa legalMoves()
		 */
		void generateMoves(QVector<Move>& moves,
				   Piece::Type type = Piece::NoPiece) const;
		
		/*!
		 * Adds pawn promotions (from \a sourceSquare to \a targetSquare)
		 * to a vector (\a moves).
		 */
		void addPromotions(int sourceSquare,
		                   int targetSquare,
		                   QVector<Move>& moves) const;
		
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
		QString castlingRightsString(FenNotation notation) const;
		
		/*! Converts a Move into a string in Long Algebraic notation. */
		QString longAlgebraicMoveString(const Move& move) const;
		
		/*! Converts a Move into a string in Standard Algebraic Notation. */
		QString sanMoveString(const Move& move);
		
		/*! Converts a string in Long Algebraic Notation into a Move. */
		Move moveFromLongAlgebraicString(const QString& str) const;
		
		/*! Converts a string in Standard Algebraic Notation into a Move. */
		Move moveFromSanString(const QString& str);
		
		/*! Returns the number of times the current position was reached. */
		int repeatCount() const;

		
		Variant m_variant;
		int m_width;
		int m_height;
		int m_arwidth;
		int m_side;
		int m_sign;
		int m_kingSquare[2];
		int m_enpassantSquare;
		int m_reversibleMoveCount;
		QVector<Piece> m_squares;
		QVector<MoveData> m_history;
		CastlingRights m_castlingRights;
		int m_castleTarget[2][2];
		quint64 m_key;
		QString m_startFen;
		
		QVector<int> m_knightOffsets;
		QVector<int> m_bishopOffsets;
		QVector<int> m_rookOffsets;
};

} // namespace Chess
#endif // CHESSBOARD
