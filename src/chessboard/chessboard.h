#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QtGlobal>
#include <QString>
#include <QVector>
#include "notation.h"
#include "movegen.h"


#define MAX_NMOVES_PER_GAME 1024	// max. num. of halfmoves per game

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
		AllPieces,	/* all pieces, only to be used as an array index */
		Pawn,
		Knight,
		Bishop,
		Rook,
		Queen,
		King,
		BQ,	/* bishops and queens, also just an array index */
		RQ,	/* rooks and queens */
		NoPiece
	};

	Chessboard();
	Chessboard(const QString& fen);

	// Set the board according to a FEN string
	void setFenString(const QString& fen);
	
	// Get piece type at 'square'
	ChessPiece pieceAt(ChessSquare square) const;

	// Get side to move
	ChessSide side() const;

	// Get the position in UCI format, with moves played
	//QString uciPosition();

	// Returns the result of the game (in the last position)
	// The result can be NoResult, WhiteMates, BlackMates, DrawByRepetition,
	// DrawByFiftyMoves, or DrawByMaterial
	//ChessResult result();
	
	// Get the number of moves played
	int moveCount() const;

	bool isSideToMoveInCheck() const;

	// Make a move
	void makeMove(quint32 move);
	// moveString can be in either SAN or Coordinate format
	void makeMove(const QString& moveString);

	// Undo the last move
	void undoMove();

	// Get the FEN string of the position
	QString fenString() const;

	// Get a move string in coordinate notation
	QString coordMoveString(quint32 move) const;
	
	// Get a move string in standard algebraic notation (SAN)
	QString sanMoveString(quint32 move);

	// Get a move from a string in SAN notation
	quint32 moveFromSan(const QString& sanMove);
	
	// Get a move from a string in coordinate notation
	quint32 moveFromString(const QString& moveString);
	
	// Get a vector of 64 elements (squares) representing the legality
	// of each square as a target for the piece at 'from'
	QVector<bool> targetSquares(ChessSquare from) const;
	
	// Print the board in ASCII format
	void print() const;

	// Run perft to test the move generator, makeMove and undoMove
	quint64 perft(int depth);


	// Initialize the move generator
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

