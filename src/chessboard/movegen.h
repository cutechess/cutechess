#ifndef MOVEGEN_H
#define MOVEGEN_H

#define MAX_NMOVES 128			// max. num. of moves per position

class MoveList
{
public:
	MoveList();
	/* Returns the move at 'index'.  */
	quint32 operator[](int index);
	/* Add a move to the list.  */
	void operator+=(quint32 move);
	/* Returns the number of moves in the list.  */
	int count();
	/* Returns true if 'move' exists in the list.  */
	bool moveExists(quint32& move);
	/* Clears the move list.  */
	void clear();
private:
	int m_nmoves;
	quint32 m_moves[MAX_NMOVES];
};

typedef struct _MoveData
{
	int from;		/* from square */
	int to;			/* to square */
	int enpassantSquare;		/* enpassant square */
	int promotion;		/* promotion piece */
	int castle;		/* castling (none, kingside or queenside) */
	quint64 rookCheckMask;		/* check mask for bishops (and queens) */
	quint64 bishopCheckMask;		/* check mask for rooks (and queens) */
	quint64 pins;		/* pinned pieces */
	quint64 discoveredCheckers;		/* pieces able to give a discovered check */
	quint64 target;		/* target mask for the moving piece */
} MoveData;

#endif // MOVEGEN_H

