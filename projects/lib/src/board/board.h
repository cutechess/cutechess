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

#ifndef BOARD_H
#define BOARD_H

#include <QString>
#include <QVector>
#include <QVarLengthArray>
#include <QSharedPointer>
#include <QDebug>
#include <QCoreApplication>
#include "square.h"
#include "piece.h"
#include "move.h"
#include "genericmove.h"
#include "zobrist.h"
#include "result.h"
class QStringList;


namespace Chess {

class BoardTransition;

/*!
 * \brief An internal chessboard class.
 *
 * This is the base class for all chess variants. Board's main purposes are:
 * - keeping track of the game, and not just piece positions, but also castling
 *   rights, en passant square, played moves, repetitions, etc.
 * - verifying the legality of moves played
 * - parsing and generating strings for moves and positions in notations
 *   such as FEN, SAN, and Coordinate notation
 * - providing information about a game to a graphical board widget
 *
 * \internal
 * The board representation is (width + 2) x (height + 4), so a
 * traditional 8x8 board would be 10x12, and stored in a one-dimensional
 * vector with 10 * 12 = 120 elements.
 */
class LIB_EXPORT Board
{
	Q_DECLARE_TR_FUNCTIONS(Board)

	public:
		/*! Coordinate system for the notation of the squares. */
		enum CoordinateSystem
		{
			/*!
			 * Normal/traditional coordinates used by most chess
			 * variants.
			 *
			 * The file is denoted by a letter, starting with file
			 * 'A' on the left.
			 * The rank is denoted by a number, starting with rank
			 * '1' at the bottom.
			 */
			NormalCoordinates,
			/*!
			 * Inverted coordinates used by some eastern variants like
			 * Shogi.
			 *
			 * The file is denoted by a number, starting with file
			 * '1' on the right.
			 * The rank is denoted by a letter, starting with rank
			 * 'A' at the top.
			 */
			InvertedCoordinates
		};
		/*! Notation for move strings. */
		enum MoveNotation
		{
			StandardAlgebraic,	//!< Standard Algebraic notation (SAN).
			LongAlgebraic		//!< Long Algebraic/Coordinate notation.
		};
		/*! Notation for FEN strings. */
		enum FenNotation
		{
			/*!
			 * X-FEN notation.
			 * \note Specs: http://en.wikipedia.org/wiki/X-FEN
			 */
			XFen,
			/*! Shredder FEN notation. */
			ShredderFen
		};

		/*!
		 * Creates a new Board object.
		 *
		 * \param zobrist Zobrist keys for quickly identifying
		 * and comparing positions. The Board class takes ownership of
		 * the zobrist object and takes care of deleting it.
		 */
		Board(Zobrist* zobrist);
		/*! Destructs the Board object. */
		virtual ~Board();
		/*! Creates and returns a deep copy of this board. */
		virtual Board* copy() const = 0;

		/*! Returns the name of the chess variant. */
		virtual QString variant() const = 0;
		/*!
		 * Returns true if the variant uses randomized starting positions.
		 * The default value is false.
		 */
		virtual bool isRandomVariant() const;
		/*!
		 * Returns true if the variant allows piece drops.
		 * The default value is false.
		 *
		 * \sa CrazyhouseBoard
		 */
		virtual bool variantHasDrops() const;
		/*!
		 * Returns true if the variant allows to skip a promotion (or a
		 * move treated as promotion) and make a normal move instead.
		 * The default value is false, i.e. mandatory promotions.
		 *
		 * \sa GrandBoard
		 * \sa SeirawanBoard
		 */
		virtual bool variantHasOptionalPromotions() const;
		/*!
		 * Returns true if the board accepts wall squares, else false.
		 * The default value is false.
		 */
		virtual bool variantHasWallSquares() const;
		/*!
		 * Returns a list of piece types that can be in the reserve,
		 * ie. captured pieces that can be dropped on the board.
		 *
		 * The default implementation returns an empty list.
		 */
		virtual QList<Piece> reservePieceTypes() const;
		/*! Returns the coordinate system used in the variant. */
		virtual CoordinateSystem coordinateSystem() const;
		/*! Returns the width of the board in squares. */
		virtual int width() const = 0;
		/*! Returns the height of the board in squares. */
		virtual int height() const = 0;
		/*! Returns the variant's default starting FEN string. */
		virtual QString defaultFenString() const = 0;
		/*! Returns the zobrist key for the current position. */
		quint64 key() const;
		/*!
		 * Initializes the board.
		 * This function must be called before a game can be started
		 * on the board.
		 */
		void initialize();
		
		/*! Returns true if \a square is on the board. */
		bool isValidSquare(const Square& square) const;

		/*!
		 * Returns list of the pieces of \a side in current position.
		 */
		QStringList pieceList(Side side) const;
		/*!
		 * Returns the FEN string of the current board position in
		 * X-Fen or Shredder FEN notation
		 */
		QString fenString(FenNotation notation = XFen) const;
		/*!
		 * Returns the FEN string of the starting position.
		 * \note This is not always the same as \a defaultFenString().
		 */
		QString startingFenString() const;
		/*!
		 * Sets the board position according to a FEN string.
		 *
		 * The \a fen string can be in standard FEN, X-FEN or
		 * Shredder FEN notation.
		 *
		 * Returns true if successful; otherwise returns false.
		 */
		bool setFenString(const QString& fen);
		/*!
		 * Sets the board position to the default starting position
		 * of the chess variant.
		 */
		void reset();

		/*!
		 * Returns the side whose pieces are denoted by uppercase letters.
		 * The default value is White.
		 */
		virtual Side upperCaseSide() const;
		/*! Returns the side to move. */
		Side sideToMove() const;
		/*! Returns the side that made/makes the first move. */
		Side startingSide() const;
		/*! Returns the piece at \a square. */
		Piece pieceAt(const Square& square) const;
		/*! Returns the number of halfmoves (plies) played. */
		int plyCount() const;
		/*!
		 * Returns the number of times the current position was
		 * reached previously in the game.
		 */
		int repeatCount() const;
		/*!
		 * Returns the number of consecutive reversible moves made.
		 *
		 * The default implementation always returns -1.
		 */
		virtual int reversibleMoveCount() const;
		/*!
		 * Returns the number of reserve pieces of type \a piece.
		 *
		 * On variants that don't have piece drops this function
		 * always returns 0.
		 */
		int reserveCount(Piece piece) const;
		/*! Converts \a piece into a piece symbol. */
		QString pieceSymbol(Piece piece) const;
		/*! Converts \a pieceSymbol into a Piece object. */
		Piece pieceFromSymbol(const QString& pieceSymbol) const;
		/*! Returns the internationalized name of \a pieceType. */
		QString pieceString(int pieceType) const;
		/*! Returns symbol for graphical representation of \a piece. */
		QString representation(Piece piece) const;

		/*!
		 * Makes a chess move on the board.
		 *
		 * All details about piece movement, promotions, captures,
		 * drops, etc. are stored in \a transition. These details are
		 * useful mainly for updating a graphical representation of
		 * the board.
		 */
		void makeMove(const Move& move, BoardTransition* transition = nullptr);
		/*! Reverses the last move. */
		void undoMove();

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
		 * \note Returns a null move if \a move is illegal.
		 * \note Notation is automatically detected, and can be anything
		 * that's specified in MoveNotation.
		 * \sa moveString()
		 */
		Move moveFromString(const QString& str);
		/*!
		 * Converts a GenericMove into a Move.
		 *
		 * \note The board must be in a position where \a move can be made.
		 * \sa genericMove()
		 */
		Move moveFromGenericMove(const GenericMove& move) const;
		/*!
		 * Converts a Move into a GenericMove.
		 *
		 * \note The board must be in a position where \a move can be made.
		 * \sa moveFromGenericMove()
		 */
		GenericMove genericMove(const Move& move) const;

		/*! Returns true if \a move is legal in the current position. */
		bool isLegalMove(const Move& move);
		/*!
		 * Returns true if \a move repeats a position that was
		 * reached earlier in the game.
		 */
		bool isRepetition(const Move& move);
		/*! Returns a vector of legal moves in the current position. */
		QVector<Move> legalMoves();
		/*!
		 * Returns the result of the game, or Result::NoResult if
		 * the game is in progress.
		 */
		virtual Result result() = 0;
		/*!
		 * Returns the expected game result according to endgame tablebases.
		 *
		 * If the position is a win for either player, \a dtm is
		 * set to the distance to mate, ie. the number of plies it
		 * takes to force a mate.
		 *
		 * The default implementation always returns a null result.
		 */
		virtual Result tablebaseResult(unsigned int* dtm = nullptr) const;

	protected:
		/*!
		 * Initializes the variant.
		 *
		 * This function is called by initialize(). Subclasses shouldn't
		 * generally call it by themselves.
		 */
		virtual void vInitialize() = 0;

		/*!
		 * Defines a piece type used in the variant.
		 * If the piece isn't already defined, it's gets added here.
		 * Unlike other initialization which happens in vInitialize(),
		 * all piece types should be defined in the constructor.
		 *
		 * \param type Type of the piece in integer format
		 * \param name The piece's name (internationalized string)
		 * \param symbol Short piece name or piece symbol
		 * \param movement A bit mask for the kinds of moves the
		 *        piece can make.
		 * \param gsymbol Select the piece's graphical representation.
		 *	  If not set the \a symbol will be used (default).
		 */
		void setPieceType(int type,
				  const QString& name,
				  const QString& symbol,
				  unsigned movement = 0,
				  const QString & gsymbol = QString());
		/*! Returns true if \a pieceType can move like \a movement. */
		bool pieceHasMovement(int pieceType, unsigned movement) const;

		/*!
		 * Makes \a move on the board.
		 *
		 * This function is called by makeMove(), and should take care
		 * of everything except changing the side to move and updating
		 * the move history.
		 *
		 * Details about piece movement, promotions, captures, drops,
		 * etc. should be stored in \a transition. If \a transition is
		 * 0 then it should be ignored.
		 */
		virtual void vMakeMove(const Move& move,
				       BoardTransition* transition) = 0;
		/*!
		 * Reverses \a move on the board.
		 *
		 * This function is called by undoMove() after changing the
		 * side to move to the side that made it.
		 *
		 * \note Unlike vMakeMove(), this function doesn't require
		 * subclasses to update the zobrist position key.
		 */
		virtual void vUndoMove(const Move& move) = 0;

		/*! Converts a square index into a Square object. */
		Square chessSquare(int index) const;
		/*! Converts a string into a Square object. */
		Square chessSquare(const QString& str) const;
		/*! Converts a Square object into a square index. */
		int squareIndex(const Square& square) const;
		/*! Converts a string into a square index. */
		int squareIndex(const QString& str) const;
		/*! Converts a square index into a string. */
		QString squareString(int index) const;
		/*! Converts a Square object into a string. */
		QString squareString(const Square& square) const;

		/*!
		 * Converts a Move object into a string in Long
		 * Algebraic Notation (LAN)
		 */
		virtual QString lanMoveString(const Move& move);
		/*!
		 * Converts a Move object into a string in Standard
		 * Algebraic Notation (SAN).
		 *
		 * \note Specs: http://en.wikipedia.org/wiki/Algebraic_chess_notation
		 */
		virtual QString sanMoveString(const Move& move) = 0;
		/*! Converts a string in LAN format into a Move object. */
		virtual Move moveFromLanString(const QString& str);
		/*! Converts a string in SAN format into a Move object. */
		virtual Move moveFromSanString(const QString& str) = 0;
		/*! Returns the maximal length of a piece symbol */
		virtual int maxPieceSymbolLength() const;

		/*!
		 * Returns the latter part of the current position's FEN string.
		 *
		 * This function is called by fenString(). The board state, side to
		 * move and hand pieces are handled by the base class. This function
		 * returns the rest of it, if any.
		 */
		virtual QString vFenString(FenNotation notation) const = 0;
		/*!
		 * Sets the board according to a FEN string.
		 *
		 * This function is called by setFenString(). The board state, side
		 * to move and hand pieces are handled by the base class. This
		 * function reads the rest of the string, if any.
		 */
		virtual bool vSetFenString(const QStringList& fen) = 0;

		/*!
		 * Generates pseudo-legal moves for pieces of type \a pieceType.
		 *
		 * \note If \a pieceType is Piece::NoPiece (default), moves are generated
		 * for every piece type.
		 * \sa legalMoves()
		 */
		void generateMoves(QVarLengthArray<Move>& moves,
				   int pieceType = Piece::NoPiece) const;
		/*!
		 * Generates piece drops for pieces of type \a pieceType.
		 *
		 * \note If \a pieceType is Piece::NoPiece, moves are generated
		 * for every piece type.
		 * \sa generateMoves()
		 */
		void generateDropMoves(QVarLengthArray<Move>& moves, int pieceType) const;
		/*!
		 * Generates pseudo-legal moves for a piece of \a pieceType
		 * at square \a square.
		 *
		 * \note It doesn't matter if \a square doesn't contain a piece of
		 * \a pieceType, the move generator ignores it.
		 */
		virtual void generateMovesForPiece(QVarLengthArray<Move>& moves,
						   int pieceType,
						   int square) const = 0;
		/*!
		 * Generates hopping moves for a piece.
		 *
		 * \param sourceSquare The source square of the hopping piece
		 * \param offsets An array of offsets for the target square
		 * \note The generated \a moves include captures
		 */
		void generateHoppingMoves(int sourceSquare,
					  const QVarLengthArray<int>& offsets,
					  QVarLengthArray<Move>& moves) const;
		/*!
		 * Generates sliding moves for a piece.
		 *
		 * \param sourceSquare The source square of the sliding piece
		 * \param offsets An array of offsets for the target square
		 * \note The generated \a moves include captures
		 */
		void generateSlidingMoves(int sourceSquare,
					  const QVarLengthArray<int>& offsets,
					  QVarLengthArray<Move>& moves) const;
		/*!
		 * Returns true if the current position is a legal position.
		 * If the position isn't legal it usually means that the last
		 * move was illegal.
		 */
		virtual bool isLegalPosition() = 0;
		/*!
		 * Returns true if \a move is a legal move.
		 *
		 * This function is called by isLegalMove() after making sure
		 * that there is a pseudo-legal move same as \a move. This
		 * function shouldn't check for the existence of \a move by
		 * generating moves.
		 *
		 * The default implementation only checks if the position
		 * after \a move is legal.
		 */
		virtual bool vIsLegalMove(const Move& move);
		/*!
		 * Returns the type of piece captured by \a move.
		 * Returns Piece::NoPiece if \a move is not a capture.
		 */
		virtual int captureType(const Move& move) const;
		/*! Updates the zobrist position key with \a key. */
		void xorKey(quint64 key);
		/*!
		 * Returns true if a pseudo-legal move \a move exists.
		 * \sa isLegalMove()
		 */
		bool moveExists(const Move& move) const;
		/*! Returns true if the side to move has any legal moves. */
		bool canMove();
		/*!
		 * Returns the size of the board array, including the padding
		 * (the inaccessible wall squares).
		 */
		int arraySize() const;
		/*! Returns the piece at \a square. */
		Piece pieceAt(int square) const;
		/*!
		 * Sets \a square to contain \a piece.
		 *
		 * This function also updates the zobrist position key, so
		 * subclasses shouldn't mess with it directly.
		 */
		void setSquare(int square, Piece piece);
		/*! Returns the last move made in the game. */
		const Move& lastMove() const;
		/*!
		 * Returns the reserve piece type corresponding to \a pieceType.
		 *
		 * The returned value is the type of piece a player receives
		 * (in variants that have piece drops) when he captures a piece of
		 * type \a pieceType.
		 *
		 * The default value is \a pieceType.
		 */
		virtual int reserveType(int pieceType) const;
		/*! Adds \a count pieces of type \a piece to the reserve. */
		void addToReserve(const Piece& piece, int count = 1);
		/*! Removes a piece of type \a piece from the reserve. */
		void removeFromReserve(const Piece& piece);

	private:
		struct PieceData
		{
			QString name;
			QString symbol;
			unsigned movement;
			QString representation;
		};
		struct MoveData
		{
			Move move;
			quint64 key;
		};
		friend LIB_EXPORT QDebug operator<<(QDebug dbg, const Board* board);

		bool m_initialized;
		int m_width;
		int m_height;
		Side m_side;
		Side m_startingSide;
		QString m_startingFen;
		int m_maxPieceSymbolLength;
		quint64 m_key;
		Zobrist* m_zobrist;
		QSharedPointer<Zobrist> m_sharedZobrist;
		QVarLengthArray<PieceData> m_pieceData;
		QVarLengthArray<Piece> m_squares;
		QVector<MoveData> m_moveHistory;
		QVector<int> m_reserve[2];
};


extern LIB_EXPORT QDebug operator<<(QDebug dbg, const Board* board);

inline int Board::arraySize() const
{
	return m_squares.size();
}

inline Side Board::sideToMove() const
{
	return m_side;
}

inline Side Board::startingSide() const
{
	return m_startingSide;
}

inline QString Board::startingFenString() const
{
	return m_startingFen;
}

inline quint64 Board::key() const
{
	return m_key;
}

inline void Board::xorKey(quint64 key)
{
	m_key ^= key;
}

inline Piece Board::pieceAt(int square) const
{
	return m_squares[square];
}

inline void Board::setSquare(int square, Piece piece)
{
	Piece& old = m_squares[square];
	if (old.isValid())
		xorKey(m_zobrist->piece(old, square));
	if (piece.isValid())
		xorKey(m_zobrist->piece(piece, square));

	old = piece;
}

inline int Board::plyCount() const
{
	return m_moveHistory.size();
}

inline const Move& Board::lastMove() const
{
	return m_moveHistory.last().move;
}

inline bool Board::pieceHasMovement(int pieceType, unsigned movement) const
{
	Q_ASSERT(pieceType != Piece::NoPiece);
	Q_ASSERT(pieceType < m_pieceData.size());

	return (m_pieceData[pieceType].movement & movement);
}

} // namespace Chess
#endif // BOARD_H
