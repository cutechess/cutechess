#include "chessboard.h"
#include "util.h"
#include "magicmoves.h"


// A mask of files B to H
#define FILE_B_H 0xFEFEFEFEFEFEFEFE
// A mask of files A to G
#define FILE_A_G 0x7F7F7F7F7F7F7F7F

/* Shift a bitboard's pieces forward, forward left or forward right.
   Naturally white's forward is black's backward and vice versa.  */
#define FWD(mask, color) ((color) == White ? (mask) >> 8 : (mask) << 8)
#define FWD_LEFT(mask, color) ((color) == White ? (mask) >> 9 : (mask) << 7)
#define FWD_RIGHT(mask, color) ((color) == White ? (mask) >> 7 : (mask) << 9)

/* Macros for building a Move from parts.  */
#define SET_FROM(square)	(square)
#define SET_TO(square)	((square) << 6)
#define SET_PC(piece)	((piece) << 12)
#define SET_CAPT(piece)	((piece) << 15)
#define SET_PROM(piece)	((piece) << 18)
#define SET_EPSQ(square)	((square) << 21)


/* Pre-calculated move bitmasks.  */
typedef struct _MoveMasks
{
    quint64 knight[64];
    quint64 king[64];
    quint64 pawnCapture[2][64];
} MoveMasks;

/* Pre-calculated move bitmasks.  */
static MoveMasks moveMasks;
/* X-ray attacks for rooks.  */
static quint64 rookXray[64];
/* X-ray attacks for bishops.  */
static quint64 bishopXray[64];

static const quint64 seventhRank[2] = { 0x000000000000FF00, 0x00FF000000000000 };


/* A mask of a straight vertical, horizontal or diagonal line
   between two squares. The two squares are also included in the mask.
   If there is no such line between the squares the mask is empty.  */
static quint64 connectMask[64][64];
/* Just like <connectMask>, except that the line from A to B continues
   through B until it reaches the edge of the board.  */
static quint64 pinMask[64][64];


/* Initialize <connectMask> and <pinMask>.  */
static void
initConnectAndPinMasks(void)
{
	int square1;
	int square2;
	int file1;
	int file2;
	int rank1;
	int rank2;
	int dir;
	quint64 mask;
	
	for (square1 = 0; square1 < 64; square1++) {
		for (square2 = 0; square2 < 64; square2++) {
			int i;
			
			connectMask[square1][square2] = 0;
			pinMask[square1][square2] = 0;

			file1 = SQ_FILE(square1);
			file2 = SQ_FILE(square2);
			rank1 = SQ_RANK(square1);
			rank2 = SQ_RANK(square2);
			
			/* Check if <square1> and <square2> are two different squares
			   that can be connected horizontally, vertically
			   or diagonally.  */
			if (square1 == square2)
				continue;
			if (file1 != file2 && rank1 != rank2
			&&  abs(file1 - file2) != abs(rank1 - rank2))
				continue;

			/* Get the direction of the line.  */
			dir = 0;
			if (file1 < file2)
				dir = 1;
			else if (file1 > file2)
				dir = -1;
			if (rank1 < rank2)
				dir += 8;
			else if (rank1 > rank2)
				dir -= 8;

			/* Get <connectMask>.  */
			mask = 0;
			for (i = square1 + dir; i != square2; i += dir)
				mask |= bit64[i];
			connectMask[square1][square2] = mask | bit64[square2];

			/* Get <pinMask>.  */
			mask = 0;
			for (i = square1 + dir; i >= 0 && i < 64; i += dir) {
				mask |= bit64[i];
				if (SQ_FILE(i) == 0
				&&  (dir == -1 || dir == 7 || dir == -9))
					break;
				if (SQ_FILE(i) == 7
				&&  (dir == 1 || dir == -7 || dir == 9))
					break;
			}
			pinMask[square1][square2] = mask;
		}
	}
}

/* Initialize bitmasks for rooks' and bishops' xray attacks.  */
static void
initXrays(void)
{
	int square;
	int file;
	int rank;
	int x;
	quint64 tmp1;
	quint64 tmp2;

	const quint64 file_a = 0x00000000000000FF;
	const quint64 rank_1 = 0x0101010101010101;
	const quint64 diag_1 = 0x8040201008040201;
	const quint64 diag_2 = 0x0102040810204080;
	
	for (square = 0; square < 64; square++) {
		int i;
		file = SQ_FILE(square);
		rank = SQ_RANK(square);

		/* rook x-rays */
		rookXray[square] = (rank_1 << (unsigned)file) |
		                (file_a << (unsigned)(rank * 8));

		/* bishop x-rays */
		tmp1 = diag_1;
		tmp2 = diag_2;
		x = file - rank;
		if (x > 0) {
			for (i = 7; (int)i > 7 - x; i--)
				tmp1 &= ~(rank_1 << (unsigned)i);
			tmp1 = tmp1 << (unsigned)x;
		} else if (-x > 0) {
			for (i = 0; i < -x; i++)
				tmp1 &= ~(rank_1 << (unsigned)i);
			tmp1 = tmp1 >> (unsigned)-x;
		}
		x = rank - (7 - file);
		if (x > 0) {
			for (i = 7; (int)i > 7 - x; i--)
				tmp2 &= ~(rank_1 << (unsigned)i);
			tmp2 = tmp2 << (unsigned)x;
		} else if (-x > 0) {
			for (i = 0; i < -x; i++)
				tmp2 &= ~(rank_1 << (unsigned)i);
			tmp2 = tmp2 >> (unsigned)-x;
		}
		bishopXray[square] = tmp1 | tmp2;
	}
}

/* Initialize bitmasks for knight moves.  */
static void
initKnightMoves(void)
{
	int square;
	int to[8];
	quint64 mask;

	for (square = 0; square < 64; square++) {
		int j;
		mask = 0;
		to[0] = square + 10;  /* 1 down, 2 right */
		to[1] = square + 6;   /* 1 down, 2 left  */
		to[2] = square + 17;  /* 2 down, 1 right */
		to[3] = square + 15;  /* 2 down, 1 left  */
		to[4] = square - 10;  /* 1 up, 2 left    */
		to[5] = square - 6;   /* 1 up, 2 right   */
		to[6] = square - 17;  /* 2 up, 1 left    */
		to[7] = square - 15;  /* 2 up, 1 right   */

		for (j = 0; j < 8; j++) {
			if (isOnBoard(to[j])
			&&  abs(SQ_FILE(square) - SQ_FILE(to[j])) <= 2)
				mask |= bit64[to[j]];
		}
		moveMasks.knight[square] = mask;
	}
}

/* Initialize bitmasks for king moves.  */
static void
initKingMoves(void)
{
	int square;
	int to[8];
	quint64 mask;

	for (square = 0; square < 64; square++) {
		int j;
		mask = 0;
		to[0] = square + 8;   /* up         */
		to[1] = square - 8;   /* down       */
		to[2] = square + 9;   /* up-left    */
		to[3] = square - 9;   /* down-right */
		to[4] = square + 1;   /* left       */
		to[5] = square - 1;   /* right      */
		to[6] = square + 7;   /* up-right   */
		to[7] = square - 7;   /* down-left  */

		for (j = 0; j < 8; j++) {
			if (isOnBoard(to[j])
			&& abs(SQ_FILE(square) - SQ_FILE(to[j])) <= 1)
				mask |= bit64[to[j]];
		}
		moveMasks.king[square] = mask;
	}
}

/* Initialize bitmasks for pawn captures.  */
static void
initPawnCaptures(void)
{
	const int edge[2] = { Chessboard::H8, Chessboard::A1 };
	int color;
	int to[2];
	quint64 mask;

	for (color = Chessboard::White; color <= Chessboard::Black; color++) {
		int i;
		int sign = SIGN(color);
		for (i = 0; i < 64; i++) {
			mask = 0;
			to[0] = i - sign*7; /* forward-right/forward-left */
			to[1] = i - sign*9; /* forward-left/forward-right */

			if (sign*i > sign*edge[color]) {
				int j;
				for (j = 0; j < 2; j++) {
					int from_file = SQ_FILE(i);
					int to_file = SQ_FILE(to[j]);
					if (to[j] < 0 || to[j] > 63
					||  abs(to_file - from_file) > 1)
						continue;
					mask |= bit64[to[j]];
				}
			}
			moveMasks.pawnCapture[color][i] = mask;
		}
	}
}

/* Initialize the move generators and masks.  */
void Chessboard::initialize(void)
{
	initZobristKeys();

	initmagicmoves();
	initXrays();
	initConnectAndPinMasks();
	initPawnCaptures();
	initKnightMoves();
	initKingMoves();
}

/* Returns true if the side to move is in check. This function is mainly
   needed only for debugging, because we encode an IS_CHECK bit
   in the move structure.  */
bool Chessboard::isSideToMoveInCheck(void) const
{
	int kingSquare;
	const quint64 *opPieces = &m_pieces[!m_color][AllPieces];

	kingSquare = m_kingSquares[m_color];

	if ((moveMasks.pawnCapture[m_color][kingSquare] & opPieces[Pawn])
	||  (moveMasks.knight[kingSquare] & opPieces[Knight])
	||  (B_MAGIC(kingSquare, m_allPieces) & opPieces[BQ])
	||  (R_MAGIC(kingSquare, m_allPieces) & opPieces[RQ]))
	//||  (moveMasks.king[kingSquare] & opPieces[King]))
		return true;
	return false;
}

/* Get a check threat mask, which is a mask of squares where the opposing king
   can't move to without being in check.  */
quint64 Chessboard::getThreatMask(int color) const
{
	int square;
	quint64 mask;
	quint64 attacks;
	quint64 pcs;
	
	/* Create a mask of all pieces except for the opposing king. This
	   allows the sliding attacks to go through him.  */
	pcs = m_allPieces ^ m_pieces[!color][King];

	/* Pawn threats.  */
	attacks = (FWD_LEFT(m_pieces[color][Pawn], color) & FILE_A_G) |
	          (FWD_RIGHT(m_pieces[color][Pawn], color) & FILE_B_H);

	/* Knight threats.  */
	mask = m_pieces[color][Knight];
	while (mask) {
		square = popLsb(&mask);
		attacks |= moveMasks.knight[square];
	}
	/* Bishop (and queen) threats.  */
	mask = m_pieces[color][BQ];
	while (mask) {
		square = popLsb(&mask);
		attacks |= B_MAGIC(square, pcs);
	}
	/* Rook (and queen) threats.  */
	mask = m_pieces[color][RQ];
	while (mask) {
		square = popLsb(&mask);
		attacks |= R_MAGIC(square, pcs);
	}
	/* King threats.  */
	attacks |= moveMasks.king[m_kingSquares[color]];
	
	return attacks;
}

/* How we know if a move is a check:
   1. Create a rook+bishop checkmask of attacks against the opposing king.
   2. For non-sliding pieces it's trivial to detect a direct check.
      Enpassant moves and promotions are annoying exceptions though,
      but since they're rare they don't affect performance that much.
   2. For sliders, if the <to> square is in the checkmask the move is a check.

   Discovered checks:
   1. Queens can't give discovered checks.
   2. Create a pinmask for the opponent's king where the pinned pieces
      are the attackers who may give the discovered check.
   3. If a move's <from> square is in the pinmask and the <to> square is
      not in pinMask[kingSquare][from], the move is a check.

   This detection shouldn't consume too many CPU cycles:
   -  Direct checks need only 2 simple masks and 1 AND per piece.
   -  Discovered checks need the pinmask and a couple of ANDs.  */

/* Returns true if <move> checks the opposing king.  */
bool Chessboard::moveIsCheck(quint32 move, MoveData *md) const
{
	int color;
	int piece;
	int from;
	int to;
	int kingSquare;
	
	Q_ASSERT(move != NULLMOVE);
	Q_ASSERT(md != NULL);

	color = m_color;
	piece = GET_PC(move);
	from = GET_FROM(move);
	to = GET_TO(move);
	kingSquare = m_kingSquares[!color];
	
	switch (piece) {
	case Pawn:
		/* Direct check.  */
		if (!GET_PROM(move)
		&& (moveMasks.pawnCapture[color][to] & bit64[kingSquare]))
			return true;
		/* Discovered check.  */
		if ((bit64[from] & md->discoveredCheckers)
		&& !(bit64[to] & pinMask[kingSquare][from]))
			return true;
		/* Discovered check by enpassant move.  */
		if (GET_EPSQ(move)) {
			int ep = GET_EPSQ(move);
			quint64 allPieces = m_allPieces;
			allPieces ^= bit64[ep] | bit64[from] | bit64[to];
			if (B_MAGIC(kingSquare, allPieces) & m_pieces[color][BQ])
				return true;
			if (R_MAGIC(kingSquare, allPieces) & m_pieces[color][RQ])
				return true;
		/* Direct check by promotion.  */
		} else if (GET_PROM(move)) {
			quint64 allPieces = m_allPieces ^ bit64[from];
			switch (GET_PROM(move)) {
			case Knight:
				if (moveMasks.knight[to] & bit64[kingSquare])
					return true;
				break;
			case Bishop:
				if (B_MAGIC(to, allPieces) & bit64[kingSquare])
					return true;
				break;
			case Rook:
				if (R_MAGIC(to, allPieces) & bit64[kingSquare])
					return true;
				break;
			case Queen:
				if (B_MAGIC(to, allPieces) & bit64[kingSquare])
					return true;
				if (R_MAGIC(to, allPieces) & bit64[kingSquare])
					return true;
				break;
			}
		}
		break;
	case Knight:
		/* Direct check.  */
		if (moveMasks.knight[to] & bit64[kingSquare])
			return true;
		/* Discovered check.  */
		if (bit64[from] & md->discoveredCheckers)
			return true;
		break;
	case Bishop:
		/* Direct check.  */
		if (md->bishopCheckMask & bit64[to])
			return true;
		/* Discovered check.  */
		if (bit64[from] & md->discoveredCheckers)
			return true;
		break;
	case Rook:
		/* Direct check.  */
		if (md->rookCheckMask & bit64[to])
			return true;
		/* Discovered check.  */
		if (bit64[from] & md->discoveredCheckers)
			return true;
		break;
	case Queen:
		/* Direct check.  */
		if ((md->bishopCheckMask | md->rookCheckMask) & bit64[to])
			return true;
		break;
	case King:
		/* Discovered check.  */
		if ((bit64[from] & md->discoveredCheckers)
		&& !(bit64[to] & pinMask[kingSquare][from]))
			return true;
		/* Direct(?) check by a castling move.  */
		if (IS_CASTLING(move)) {
			int castle;
			int rookSquare;
			quint64 allPieces;
			
			castle = GET_CASTLE(move);
			rookSquare = castling.rookSquare[color][castle][C_TO];
			allPieces = m_allPieces ^ m_pieces[color][King];
			if (R_MAGIC(rookSquare, allPieces) & bit64[kingSquare])
				return true;
		}
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	
	return false;
}

/* Form a new move and add it to a move list.  */
#define CHECK_BIT 04000000000
void Chessboard::addMove(MoveData *md, MoveList *moveList) const
{
	const unsigned castleBits[2] = { BIT(27U), BIT(28U) | BIT(27U) };
	int piece;
	int capture;
	quint32 move;
	
	Q_ASSERT(md != NULL);
	Q_ASSERT(moveList != NULL);
	Q_ASSERT(moveList->count() < MAX_NMOVES);
	Q_ASSERT(isOnBoard(md->from));
	Q_ASSERT(isOnBoard(md->to));
	Q_ASSERT(!md->promotion || (md->promotion >= Knight && md->promotion <= Queen));

	piece = m_mailbox[md->from];
	if (md->enpassantSquare != 0)
		capture = Pawn;
	else
		capture = m_mailbox[md->to];
	move = SET_FROM(md->from) | SET_TO(md->to) | SET_PC(piece) |
	       SET_CAPT(capture) | SET_PROM(md->promotion) | SET_EPSQ(md->enpassantSquare);
	if (md->castle != -1)
		move |= castleBits[md->castle];
	if (moveIsCheck(move, md))
		move |= CHECK_BIT;
	
	*moveList += move;
}

/* Make sure a pawn capture is legal, then call addMove() to add it to the
   move list. If the capture is also a promotion then all the possible
   promotions will be added. */
void Chessboard::addPawnCapture(MoveData *md, MoveList *moveList) const
{
	int color;
	int kingSquare;

	Q_ASSERT(md != NULL);
	Q_ASSERT(moveList != NULL);

	color = m_color;
	kingSquare = m_kingSquares[color];

	if ((bit64[md->from] & md->pins)
	&& !(bit64[md->to] & pinMask[kingSquare][md->from]))
		return;

	if (md->to && m_posp->enpassantSquare == md->to) {
		md->enpassantSquare = md->to + SIGN(color)*8;
		if (SQ_RANK(md->from) == SQ_RANK(kingSquare)) {
			quint64 allPieces = m_allPieces;
			allPieces ^=  bit64[md->enpassantSquare] | bit64[md->from];
			if (R_MAGIC(kingSquare, allPieces) & m_pieces[!color][RQ])
				return;
		}
	} else
		md->enpassantSquare = 0;

	if (bit64[md->from] & seventhRank[color]) {
		for (md->promotion = Queen; md->promotion >= Knight; md->promotion--)
			addMove(md, moveList);
	} else {
		md->promotion = 0;
		addMove(md, moveList);
	}
}

/* Generate pawn captures.  */
void Chessboard::generatePawnCaptures(MoveData *md, MoveList *moveList) const
{
	int color;
	int side;	/* capture side, 0 = left, 1 = right */
	int enpassantSquare;
	quint64 target;
	static const int direction[2][2] = { {9, 7}, {-7, -9} };

	Q_ASSERT(md != NULL);
	Q_ASSERT(moveList != NULL);

	color = m_color;
	enpassantSquare = m_posp->enpassantSquare;
	target = m_pieces[!color][AllPieces] & md->target;
	
	md->castle = -1;
	
	/* Enpassant capture is possible.  */
	if (enpassantSquare && (bit64[enpassantSquare + SIGN(color)*8] & target))
		target |= bit64[enpassantSquare];

	for (side = 0; side < 2; side++) {
		quint64 mask = target;
		quint64 my_pawns = m_pieces[color][Pawn];

		if (side == 0)
			mask &= FWD_LEFT(my_pawns, color) & FILE_A_G;
		else
			mask &= FWD_RIGHT(my_pawns, color) & FILE_B_H;

		while (mask) {
			md->to = popLsb(&mask);
			md->from = md->to + direction[color][side];
			addPawnCapture(md, moveList);
		}
	}
}

/* Make sure a pawn move is legal, then call addMove() to add it to the
   move list. If the move is a promotion then all the possible
   promotions will be added. */
void Chessboard::addPawnMove(MoveData *md, MoveList *moveList) const
{
	int color;
	int kingSquare;

	Q_ASSERT(md != NULL);
	Q_ASSERT(moveList != NULL);

	color = m_color;
	kingSquare = m_kingSquares[color];

	if ((bit64[md->from] & md->pins)
	&& !(bit64[md->to] & pinMask[kingSquare][md->from]))
		return;

	if (bit64[md->from] & seventhRank[color]) {
		for (md->promotion = Queen; md->promotion >= Knight; md->promotion--)
			addMove(md, moveList);
	} else {
		md->promotion = 0;
		addMove(md, moveList);
	}
}

/* Generate pawn moves (non-captures).  */
void Chessboard::generatePawnMoves(MoveData *md, MoveList *moveList) const
{
	int color;
	int sign;
	quint64 target1;	/* 1 square forward target */
	quint64 target2;	/* 2 squares forward target */
	static const quint64 rank_4[] = { 0x000000FF00000000, 0x00000000FF000000 };

	Q_ASSERT(md != NULL);
	Q_ASSERT(moveList != NULL);

	color = m_color;
	sign = SIGN(color);
	md->enpassantSquare = 0;
	md->castle = -1;

	/* 1 square forward.  */
	target1 = FWD(m_pieces[color][Pawn], color) & ~m_allPieces;
	/* 2 squares forward.  */
	target2 = FWD(target1, color) & (~m_allPieces & rank_4[color]);

	target1 &= md->target;
	target2 &= md->target;

	while (target1) {
		md->to = popLsb(&target1);
		md->from = md->to + sign*8;
		addPawnMove(md, moveList);
	}
	while (target2) {
		md->to = popLsb(&target2);
		md->from = md->to + sign*16;
		addPawnMove(md, moveList);
	}
}

/* Generate knight moves.  */
void Chessboard::generateKnightMoves(MoveData *md, MoveList *moveList) const
{
	int color;
	quint64 mask;
	quint64 target;

	Q_ASSERT(md != NULL);
	Q_ASSERT(moveList != NULL);
	
	color = m_color;
	md->promotion = 0;
	md->enpassantSquare = 0;
	md->castle = -1;
	target = ~m_pieces[color][AllPieces] & md->target;
	mask = m_pieces[color][Knight] & ~md->pins;

	while (mask) {
		quint64 attacks;

		md->from = popLsb(&mask);
		attacks = (moveMasks.knight[md->from] & target);
		while (attacks) {
			md->to = popLsb(&attacks);
			addMove(md, moveList);
		}
	}
}

/* Generate bishop moves.  */
void Chessboard::generateBishopMoves(MoveData *md, MoveList *moveList) const
{
	int color;
	int kingSquare;
	quint64 mask;
	quint64 target;

	Q_ASSERT(md != NULL);
	Q_ASSERT(moveList != NULL);
	
	color = m_color;
	md->promotion = 0;
	md->enpassantSquare = 0;
	md->castle = -1;
	kingSquare = m_kingSquares[color];
	mask = m_pieces[color][BQ];

	target = ~m_pieces[color][AllPieces] & md->target;
	while (mask) {
		quint64 attacks;

		md->from = popLsb(&mask);
		attacks = B_MAGIC(md->from, m_allPieces) & target;
		if (bit64[md->from] & md->pins)
			attacks &= pinMask[kingSquare][md->from];
		while (attacks) {
			md->to = popLsb(&attacks);
			addMove(md, moveList);
		}
	}
}

/* Generate rook moves.  */
void Chessboard::generateRookMoves(MoveData *md, MoveList *moveList) const
{
	int color;
	int kingSquare;
	quint64 mask;
	quint64 target;

	Q_ASSERT(md != NULL);
	Q_ASSERT(moveList != NULL);

	color = m_color;
	md->promotion = 0;
	md->enpassantSquare = 0;
	md->castle = -1;
	kingSquare = m_kingSquares[color];
	mask = m_pieces[color][RQ];

	target = ~m_pieces[color][AllPieces] & md->target;
	while (mask) {
		quint64 attacks;

		md->from = popLsb(&mask);
		attacks = R_MAGIC(md->from, m_allPieces) & target;
		if (bit64[md->from] & md->pins)
			attacks &= pinMask[kingSquare][md->from];
		while (attacks) {
			md->to = popLsb(&attacks);
			addMove(md, moveList);
		}
	}
}

/* Generate king moves (non-captures).  */
void Chessboard::generateKingMoves(MoveData *md, MoveList *moveList) const
{
	int color;
	int i;
	quint64 attacks;
	quint64 threats;
	quint64 target;
	
	/* A mask of squares that mustn't be checked by
	   the opponent when castling.  */
	static const quint64 castleCheckMask[2][2] =
	{
		{ 0x7000000000000000, 0x1C00000000000000 },
		{ 0x0000000000000070, 0x000000000000001C }
	};
	/* A mask of squares that have to be empty for a castling move
	   to be legal.  */
	static const quint64 castleEmptyMask[2][2] = {
		{ 0x6000000000000000, 0x0e00000000000000 },
		{ 0x0000000000000060, 0x000000000000000e }
	};

	Q_ASSERT(md != NULL);
	Q_ASSERT(moveList != NULL);

	color = m_color;
	md->promotion = 0;
	md->enpassantSquare = 0;
	md->castle = -1;
	md->from = m_kingSquares[color];
	threats = getThreatMask(!color);
	target = ~threats & md->target;

	/* Normal moves (not castling moves).  */
	attacks = moveMasks.king[md->from] & target;
	while (attacks) {
		md->to = popLsb(&attacks);
		addMove(md, moveList);
	}

	/* Castling moves

	   If the king is not in square E1 (white) or E8 (black), if it
	   doesn't have any castling rights left, or if the king is in
	   check, castling isn't possible.  */
	if (bit64[md->from] & threats
	|| md->from != castling.kingSquares[color][C_KSIDE][C_FROM]
	|| !(m_posp->castleRights & castling.allRights[color]))
		return;

	for (i = 0; i < 2; i++) {
		int rookSquare = castling.rookSquare[color][i][C_FROM];
		md->to = castling.kingSquares[color][i][C_TO];
		if ((m_posp->castleRights & castling.rights[color][i])
		&& !(m_allPieces & castleEmptyMask[color][i])
		&& !(threats & castleCheckMask[color][i])
		&& (bit64[md->to] & target)
		&& (m_pieces[color][Rook] & bit64[rookSquare])) {
			md->castle = i;
			addMove(md, moveList);
		}
	}
}

/* Get a mask of <pinned_color>'s pieces that are pinned to <color>'s king.
   <color> and <pinned_color> may or may not be the same. If they're not, then
   the pieces aren't really pinned, but they can give a discovered check.  */
quint64 Chessboard::getPins(int color, int pinned_color) const
{
	int kingSquare;
	quint64 pinners;
	
	kingSquare = m_kingSquares[color];
	pinners = (bishopXray[kingSquare] & m_pieces[!color][BQ]) |
	          (rookXray[kingSquare] & m_pieces[!color][RQ]);

	if (pinners) {
		quint64 pins = 0;
		quint64 b1 = m_allPieces;
		quint64 b3;
		
		quint64 b2 = B_MAGIC(kingSquare, b1);
		pinners &= ~b2;
		b2 &= m_pieces[pinned_color][AllPieces];
		while (b2) {
			b3 = b2 & -b2;
			b2 ^= b3;
			if (B_MAGIC(kingSquare, b1 ^ b3) & pinners) pins |= b3;
		}

		b2 = R_MAGIC(kingSquare, b1);
		pinners &= ~b2;
		b2 &= m_pieces[pinned_color][AllPieces];
		while (b2) {
			b3 = b2 & -b2;
			b2 ^= b3;
			if (R_MAGIC(kingSquare, b1 ^ b3) & pinners) pins |= b3;
		}
		
		return pins;
	}
	
	return 0;
}

/* Get a mask of opponent's pieces that check the king, and all
   the other squares where pieces (not the king) can move to
   evade the check.  */
quint64 Chessboard::getCheckMask(void) const
{
	int kingSquare;
	const quint64 *opPieces = &m_pieces[!m_color][AllPieces];
	quint64 sliders;
	quint64 checkMask;

	kingSquare = m_kingSquares[m_color];

	checkMask =
		(moveMasks.pawnCapture[m_color][kingSquare] & opPieces[Pawn]) |
		(moveMasks.knight[kingSquare] & opPieces[Knight]);
	sliders =
		(B_MAGIC(kingSquare, m_allPieces) & opPieces[BQ]) |
		(R_MAGIC(kingSquare, m_allPieces) & opPieces[RQ]);

	if (checkMask == 0 && sliders == 0)
		return 0;

	while (sliders)
		checkMask |= connectMask[kingSquare][popLsb(&sliders)];

	return checkMask;
}

/* Generate some bitmasks for move generation.  */
void Chessboard::generateMovegenMasks(MoveData *md) const
{
	int color;
	int kingSquare;
	
	Q_ASSERT(md != NULL);
	
	color = m_color;
	kingSquare = m_kingSquares[!color];
	
	md->bishopCheckMask = B_MAGIC(kingSquare, m_allPieces);
	md->rookCheckMask = R_MAGIC(kingSquare, m_allPieces);
	md->pins = getPins(color, color);
	md->discoveredCheckers = getPins(!color, color);
}

/* Generate all legal moves.  */
void Chessboard::generateMoves(MoveList *moveList) const
{
	int color;
	MoveData md;

	Q_ASSERT(moveList != NULL);

	color = m_color;
	moveList->clear();

	md.target = ~m_pieces[color][AllPieces];
	generateMovegenMasks(&md);

	generateKingMoves(&md, moveList);
	if (m_posp->inCheck) {
		quint64 checkMask = getCheckMask();
		Q_ASSERT(checkMask != 0);
		Q_ASSERT(is_check());
		/* In a double check we only generate king moves
		   because everything else is illegal.  */
		if (popcount(checkMask & m_pieces[!color][AllPieces]) > 1)
			return;
		md.target &= checkMask;
	}
	
	generatePawnMoves(&md, moveList);
	generatePawnCaptures(&md, moveList);
	generateKnightMoves(&md, moveList);
	generateBishopMoves(&md, moveList);
	generateRookMoves(&md, moveList);
}

/* Generate moves for a specific piece type with a specific <to> square.  */
void Chessboard::generateMovesForPieceType(MoveList *moveList, int piece, int to)
{
	int color;
	MoveData md;
	quint64 tmpMask;

	Q_ASSERT(moveList != NULL);
	Q_ASSERT(piece >= Pawn && piece <= King);
	Q_ASSERT(isOnBoard(to));

	color = m_color;
	moveList->clear();

	if (piece == Pawn && to != 0 && to == m_posp->enpassantSquare)
		to += SIGN(color)*8;

	md.target = ~m_pieces[color][AllPieces] & bit64[to];
	generateMovegenMasks(&md);

	if (piece == King) {
		generateKingMoves(&md, moveList);
		return;
	}

	if (m_posp->inCheck) {
		quint64 checkMask = getCheckMask();
		Q_ASSERT(checkMask != 0);
		Q_ASSERT(is_check());

		/* In a double check we return without any moves.  */
		if (popcount(checkMask & m_pieces[!color][AllPieces]) > 1)
			return;
		md.target &= checkMask;
	}

	switch (piece) {
	case Pawn:
		generatePawnMoves(&md, moveList);
		generatePawnCaptures(&md, moveList);
		break;
	case Knight:
		generateKnightMoves(&md, moveList);
		break;
	case Bishop:
		tmpMask = m_pieces[color][BQ];
		m_pieces[color][BQ] = m_pieces[color][Bishop];
		generateBishopMoves(&md, moveList);
		m_pieces[color][BQ] = tmpMask;
		break;
	case Rook:
		tmpMask = m_pieces[color][RQ];
		m_pieces[color][RQ] = m_pieces[color][Rook];
		generateRookMoves(&md, moveList);
		m_pieces[color][RQ] = tmpMask;
		break;
	case Queen:
		tmpMask = m_pieces[color][BQ];
		m_pieces[color][BQ] = m_pieces[color][Queen];
		generateBishopMoves(&md, moveList);
		m_pieces[color][BQ] = tmpMask;

		tmpMask = m_pieces[color][RQ];
		m_pieces[color][RQ] = m_pieces[color][Queen];
		generateRookMoves(&md, moveList);
		m_pieces[color][RQ] = tmpMask;
		break;
	default:
		Q_ASSERT(false);
		break;
	}
}

