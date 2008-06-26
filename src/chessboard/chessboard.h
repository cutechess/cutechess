#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QtGlobal>
#include <QString>
#include <QVector>
#include "notation.h"
#include "movegen.h"


#define MAX_NMOVES_PER_GAME 1024	// max. num. of halfmoves per game

class ChessMove;

/**
 * The Chessboard class is used for the internal chessboard of SloppyGUI.
 *
 * Chessboard's main purposes are:
 * - keeping track of the game, and not just piece positions, but also castling
 *   rights, en-passant square, played moves, etc.
 * - verifying moves played by humans and chess engines
 * - parsing and generating strings for moves and positions in notations
 *   such as FEN, SAN, and Coordinate Notation
 *
 * Chessboard::initialize() must be called before the class can be used
 */
class Chessboard
{
public:
	enum ChessSide
	{
		White,
		Black,
		NoSide,
		SideError
	};

	enum ChessSquare
	{
		A8, B8, C8, D8, E8, F8, G8, H8,
		A7, B7, C7, D7, E7, F7, G7, H7,
		A6, B6, C6, D6, E6, F6, G6, H6,
		A5, B5, C5, D5, E5, F5, G5, H5,
		A4, B4, C4, D4, E4, F4, G4, H4,
		A3, B3, C3, D3, E3, F3, G3, H3,
		A2, B2, C2, D2, E2, F2, G2, H2,
		A1, B1, C1, D1, E1, F1, G1, H1,
		NoSquare, SquareError
	};

	enum ChessPiece
	{
		AllPieces, // all pieces, only to be used as an array index
		Pawn,
		Knight,
		Bishop,
		Rook,
		Queen,
		King,
		BQ,	// bishops and queens, also just an array index
		RQ,	// rooks and queens
		NoPiece
	};

	/**
	 * Constructor that doesn't set the board to any specific position.
	 * No moves can be made on such a board before setFenString() is called.
	 * @see setFenString()
	 */
	Chessboard();
	
	/**
	 * Constructor that sets the board to a position.
	 * @param fen a board position in FEN notation.
	 * @see Chessboard()
	 */
	Chessboard(const QString& fen);

	/**
	 * Sets the board according to a FEN string.
	 * @param fen a board position in FEN notation.
	 */
	void setFenString(const QString& fen);
	
	/**
	 * Gets the piece type at a specific square.
	 * @param square a square on the chessboard.
	 * @return the type of the piece.
	 */
	ChessPiece pieceAt(ChessSquare square) const;

	/**
	 * Gets the side to move.
	 * @return the side to move.
	 */
	ChessSide side() const;

	/**
	 * Gets the position in UCI format, with moves played.
	 * @return the current position in UCI format (FEN + moves played).
	 * @see fenString()
	 */
	//QString uciPosition();

	/**
	 * Gets the result of the game (in the current position).
	 * @return the result, or NoResult if the game hasn't ended.
	 */
	//ChessResult result();
	
	/**
	 * Gets the number of halfmoves played (in the current position).
	 * @return the number of halfmoves played.
	 */
	int moveCount() const;

	/**
	 * @return true, if the side to move is in check.
	 */
	bool isSideToMoveInCheck() const;

	/**
	 * Makes a move.
	 * @param move a move in integer (bitmask) format.
	 * @see undoMove()
	 */
	void makeMove(quint32 move);

	/**
	 * Makes a move.
	 * @param move a chessmove.
	 * @see undoMove()
	 */
	void makeMove(const ChessMove& move);

	/**
	 * Makes a move.
	 * If the move is illegal, an exception is raised.
	 * @param moveString a move either in SAN or in Coordinate notation.
	 * @see undoMove()
	 */
	void makeMove(const QString& moveString);

	/**
	 * Undo the last move.
	 * @see makeMove()
	 */
	void undoMove();

	/**
	 * Gets the FEN string of the current position.
	 * @return the board position in FEN.
	 * @see setFenString()
	 */
	QString fenString() const;

	/**
	 * Gets a move in integer format.
	 * @param from the source square.
	 * @param to the target square.
	 * @param promotion the promotion piece
	 * @return a move in integer format.
	 */
	quint32 intMove(ChessSquare from, ChessSquare to, ChessPiece promotion);

	/**
	 * Gets a move string in coordinate notation.
	 * @param move a move in integer format.
	 * @return a move string in coordinate notation.
	 * @see sanMoveString()
	 */
	QString coordMoveString(quint32 move) const;
	
	/**
	 * Gets a move string in SAN (Standard Algebraic Notation)
	 * @param move a move in integer format.
	 * @return a move string in SAN.
	 * @see coordMoveString()
	 */
	QString sanMoveString(quint32 move);

	/**
	 * Gets a move from a SAN move string.
	 * @param sanMove a move in SAN.
	 * @return a move in integer format.
	 * @see moveFromCoord()
	 */
	quint32 moveFromSan(const QString& sanMove);
	
	/**
	 * Gets a move from a string in coordinate notation.
	 * @param coordMove a move in coordinate notation.
	 * @return a move in integer format.
	 * @see moveFromSan()
	 */
	quint32 moveFromCoord(const QString& coordMove);
	
	/** 
	 * Gets a vector of 64 elements (squares) representing the legality
	 * of each square as a target for the piece at 'from'.
	 * For example, if targetSquares(H1)[H8] is 'true', it means that the
	 * piece at H1 can move to the square H8.
	 * @param from the square on which the piece to move is.
	 * @return a vector of 64 boolean values.
	 */
	QVector<bool> targetSquares(ChessSquare from) const;
	
	/**
	 * Prints the board in ASCII format.
	 * This member function is for debugging purposes.
	 */
	void print() const;

	/**
	 * Runs perft to test the move generator, makeMove() and undoMove().
	 * @param depth the search depth in plies
	 * @return the number of leaf nodes in the perft search tree.
	 */
	quint64 perft(int depth);


	/**
	 * Initializes the move generator.
	 * This function must be called before any instances of the class
	 * are created.
	 */
	static void initialize();

private:
	typedef struct _PosInfo
	{
		unsigned castleRights;	// castle rights mask
		int enpassantSquare;	// enpassant square
		int fifty;		// num. of reversible moves played in a row
		bool inCheck;		// if true, the side to move is in check
		quint32 move;		// move that was played to reach this position
		quint64 key;		// zobrist key
	} PosInfo;

	int m_nmoves;		// num. of half moves made
	int m_color;		// the side to move
	int m_kingSquares[2];	// king's squares
	int m_mailbox[64];	// board in mailbox format, side not encoded
	quint64 m_allPieces;	// mask of all pieces on board
	quint64 m_pieces[2][9];	// masks of all piece types for both sides
	PosInfo *m_posp;	// pointer to PosInfo of current pos
	PosInfo m_pos[MAX_NMOVES_PER_GAME]; // PosInfo of each reached position

	// Initialize the Zobrist keys
	static void initZobristKeys();
	// Compute the Zobrist key for the current chess position
	void computeZobristKey();
	
	// makemove
	void makePawnMove(quint32 move);
	void makeRookMove(quint32 move);
	void makeKingMove(quint32 move);
	void undoPawnMove(quint32 move);
	void undoKingMove(quint32 move);
	int getRepeatCount(int maxRepeats=3) const;

	// movegen

	// Generate all legal moves
	void generateMoves(MoveList *moveList) const;
	// Generate moves for a specific piece type with a specific 'to' square
	void generateMovesForPieceType(MoveList *moveList, int piece, int to);

	unsigned neededMoveDetails(int piece, int from, int to);
	MoveType getMoveType(quint32 move);

	quint64 getThreatMask(int color) const;
	bool moveIsCheck(quint32 move, MoveData *md) const;
	void addMove(MoveData *md, MoveList *moveList) const;
	void addPawnCapture(MoveData *md, MoveList *moveList) const;
	void generatePawnCaptures(MoveData *md, MoveList *moveList) const;
	void addPawnMove(MoveData *md, MoveList *moveList) const;
	void generatePawnMoves(MoveData *md, MoveList *moveList) const;
	void generateKnightMoves(MoveData *md, MoveList *moveList) const;
	void generateBishopMoves(MoveData *md, MoveList *moveList) const;
	void generateRookMoves(MoveData *md, MoveList *moveList) const;
	void generateKingMoves(MoveData *md, MoveList *moveList) const;
	quint64 getPins(int color, int pinned_color) const;
	quint64 getCheckMask() const;
	void generateMovegenMasks(MoveData *md) const;

	void setSquares(const int *mailbox);
};
#endif // CHESSBOARD_H

