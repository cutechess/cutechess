#include "chessboard.h"
#include "util.h"
#include "zobrist.h"


/* Returns true if the position before the move can be later reached again.  */
static bool
is_reversible(quint32 move)
{
	if (GET_PC(move) == Chessboard::Pawn || GET_CAPT(move) || IS_CASTLING(move))
		return false;
	return true;
}

void Chessboard::makePawnMove(quint32 move)
{
	int color;
	int from;
	int to;
	int enpassantSquare;
	int promotion;
	quint64 *key;

	color = m_color;
	key = &m_posp->key;
	from = GET_FROM(move);
	to = GET_TO(move);
	promotion = GET_PROM(move);
	Q_ASSERT(!promotion || (promotion >= Knight && promotion <= Queen));
	enpassantSquare = GET_EPSQ(move);
	Q_ASSERT(isOnBoard(enpassantSquare));

	m_pieces[color][Pawn] ^= bit64[from];
	if (promotion != 0) {
		m_mailbox[to] = promotion;
		m_pieces[color][promotion] ^= bit64[to];
		*key ^= zobrist.piece[color][promotion][to];
	} else {
		m_mailbox[to] = Pawn;
		m_pieces[color][Pawn] ^= bit64[to];
		*key ^= zobrist.piece[color][Pawn][to];
	}

	/* It's an enpassant move.  */
	if (enpassantSquare) {
		m_mailbox[enpassantSquare] = 0;
		m_pieces[!color][Pawn] ^= bit64[enpassantSquare];
		m_pieces[!color][AllPieces] ^= bit64[enpassantSquare];
		*key ^= zobrist.piece[!color][Pawn][enpassantSquare];
	}

	/* The pawn is pushed two squares ahead, which means that the
	   opponent can make an enpassant capture on his turn.  */
	if (SIGN(color)*(to - from) == -16) {
		enpassantSquare = to + SIGN(color)*8;
		m_posp->enpassantSquare = enpassantSquare;
		*key ^= zobrist.enpassant[enpassantSquare];
	}
}

void Chessboard::makeRookMove(quint32 move)
{
	int color;
	int from;
	unsigned *castleRights;
	quint64 *key;

	color = m_color;
	key = &m_posp->key;
	from = GET_FROM(move);
	castleRights = &m_posp->castleRights;
	
	/* Make sure the player loses his castling rights in the corner
	   the rook moves from, if any.  */
	if (from == castling.rookSquares[color][C_KSIDE][C_FROM]
	&& *castleRights & castling.rights[color][C_KSIDE]) {
		*key ^= zobrist.castle[color][C_KSIDE];
		*castleRights &= ~castling.rights[color][C_KSIDE];
	} else if (from == castling.rookSquares[color][C_QSIDE][C_FROM]
	&& *castleRights & castling.rights[color][C_QSIDE]) {
		*key ^= zobrist.castle[color][C_QSIDE];
		*castleRights &= ~castling.rights[color][C_QSIDE];
	}
}

void Chessboard::makeKingMove(quint32 move)
{
	int color;
	unsigned *castleRights;
	quint64 *key;

	color = m_color;
	key = &m_posp->key;
	castleRights = &m_posp->castleRights;

	/* Take away the castling rights.  */
	if (*castleRights & castling.allRights[color]) {
		if (*castleRights & castling.rights[color][C_KSIDE])
			*key ^= zobrist.castle[color][C_KSIDE];
		if (*castleRights & castling.rights[color][C_QSIDE])
			*key ^= zobrist.castle[color][C_QSIDE];
		*castleRights &= ~castling.allRights[color];
	}
	m_kingSquares[color] = GET_TO(move);

	if (IS_CASTLING(move)) {
		int castle;
		int rookFromSquare;
		int rookToSquare;
		quint64 rookMask;
		
		castle = GET_CASTLE(move);
		Q_ASSERT(castle == C_KSIDE || castle == C_QSIDE);
		
		rookFromSquare = castling.rookSquares[color][castle][C_FROM];
		rookToSquare = castling.rookSquares[color][castle][C_TO];
		rookMask = bit64[rookFromSquare] | bit64[rookToSquare];

		m_mailbox[rookFromSquare] = 0;
		m_mailbox[rookToSquare] = Rook;
		m_pieces[color][Rook] ^= rookMask;
		m_pieces[color][AllPieces] ^= rookMask;
		*key ^= zobrist.piece[color][Rook][rookFromSquare];
		*key ^= zobrist.piece[color][Rook][rookToSquare];
	}
}

void Chessboard::makeMove(quint32 move)
{
	int color;
	int from;
	int to;
	int piece;
	int capture;

	quint64 *my_pcs;
	quint64 *opPieces;
	quint64 from_to_mask;
	quint64 *key;
	PosInfo *pos;

	Q_ASSERT(move != NULLMOVE);

	color = m_color;
	from = GET_FROM(move);
	Q_ASSERT(from >= 0 && from <= 63);
	to = GET_TO(move);
	Q_ASSERT(to >= 0 && to <= 63);
	piece = GET_PC(move);
	Q_ASSERT(piece >= Pawn && piece <= King);
	capture = GET_CAPT(move);
	Q_ASSERT(!capture || (capture >= Pawn && capture <= Queen));

	my_pcs = &m_pieces[color][AllPieces];
	opPieces = &m_pieces[!color][AllPieces];
	from_to_mask = bit64[from] | bit64[to];

	/* Initialize the new position.  */
	(m_posp)++;
	pos = m_posp;
	*pos = *(pos - 1);
	key = &pos->key;
	pos->move = move;
	
	if (IS_CHECK(move))
		pos->inCheck = true;
	else
		pos->inCheck = false;
	if (is_reversible(move))
		pos->fifty = (pos - 1)->fifty + 1;
	else
		pos->fifty = 0;

	*key ^= zobrist.piece[color][piece][from];
	
	/* En passant capture at pos->enpassantSquare isn't possible anymore.  */
	if (pos->enpassantSquare) {
		*key ^= zobrist.enpassant[pos->enpassantSquare];
		pos->enpassantSquare = 0;
	}

	if (piece == Pawn)
		makePawnMove(move);
	else {
		m_mailbox[to] = piece;
		my_pcs[piece] ^= from_to_mask;
		if (piece == King)
			makeKingMove(move);
		else if (piece == Rook)
			makeRookMove(move);
		*key ^= zobrist.piece[color][piece][to];
	}
	*my_pcs ^= from_to_mask;
	m_mailbox[from] = 0;

	/* En passant captures are handled by makePawnMove().
	   This is for all other captures.  */
	if (capture && !GET_EPSQ(move)) {
		opPieces[capture] ^= bit64[to];
		*opPieces ^= bit64[to];
		*key ^= zobrist.piece[!color][capture][to];
		opPieces[BQ] = opPieces[Bishop] | opPieces[Queen];
		opPieces[RQ] = opPieces[Rook] | opPieces[Queen];
	}
	
	my_pcs[BQ] = my_pcs[Bishop] | my_pcs[Queen];
	my_pcs[RQ] = my_pcs[Rook] | my_pcs[Queen];
	m_allPieces = *my_pcs | *opPieces;
	*key ^= zobrist.color;

	m_color = !color;
	(m_nmoves)++;
	Q_ASSERT(m_nmoves <= MAX_NMOVES_PER_GAME);
}

void Chessboard::undoPawnMove(quint32 move)
{
	int color;
	int from;
	int to;
	int enpassantSquare;
	int promotion;

	color = !m_color;
	from = GET_FROM(move);
	to = GET_TO(move);
	promotion = GET_PROM(move);
	Q_ASSERT(!promotion || (promotion >= Knight && promotion <= Queen));
	enpassantSquare = GET_EPSQ(move);
	Q_ASSERT(enpassantSquare >= 0 && enpassantSquare <= 63);

	m_pieces[color][Pawn] ^= bit64[from];
	if (promotion)
		m_pieces[color][promotion] ^= bit64[to];
	else
		m_pieces[color][Pawn] ^= bit64[to];

	if (enpassantSquare) {
		m_mailbox[enpassantSquare] = Pawn;
		m_pieces[!color][Pawn] ^= bit64[enpassantSquare];
		m_pieces[!color][AllPieces] ^= bit64[enpassantSquare];
	}
}

void Chessboard::undoKingMove(quint32 move)
{
	int color;
	
	color = !m_color;
	m_kingSquares[color] = GET_FROM(move);

	if (IS_CASTLING(move)) {
		int castle;
		int rookFromSquare;
		int rookToSquare;
		quint64 rookMask;

		castle = GET_CASTLE(move);
		Q_ASSERT(castle == C_KSIDE || castle == C_QSIDE);

		rookFromSquare = castling.rookSquares[color][castle][C_FROM];
		rookToSquare = castling.rookSquares[color][castle][C_TO];
		rookMask = bit64[rookFromSquare] | bit64[rookToSquare];

		m_mailbox[rookToSquare] = 0;
		m_mailbox[rookFromSquare] = Rook;
		m_pieces[color][Rook] ^= rookMask;
		m_pieces[color][AllPieces] ^= rookMask;
	}
}

void Chessboard::undoMove(void)
{
	int color;
	int from;
	int to;
	int piece;
	int capture;
	quint32 move;
	quint64 *my_pcs;
	quint64 *opPieces;
	quint64 mask;

	move = m_posp->move;
	color = !m_color;
	from = GET_FROM(move);
	Q_ASSERT(from >= 0 && from <= 63);
	to = GET_TO(move);
	Q_ASSERT(to >= 0 && to <= 63);
	piece = GET_PC(move);
	Q_ASSERT(piece >= Pawn && piece <= King);
	capture = GET_CAPT(move);
	Q_ASSERT(!capture || (capture >= Pawn && capture <= Queen));

	my_pcs = &m_pieces[color][AllPieces];
	opPieces = &m_pieces[!color][AllPieces];
	mask = bit64[from] | bit64[to];

	if (piece == Pawn)
		undoPawnMove(move);
	else {
		my_pcs[piece] ^= mask;
		if (piece == King)
			undoKingMove(move);
	}
	*my_pcs ^= mask;

	m_mailbox[from] = piece;
	m_mailbox[to] = 0;
	
	if (capture && !GET_EPSQ(move)) {
		m_mailbox[to] = capture;
		opPieces[capture] ^= bit64[to];
		*opPieces ^= bit64[to];
		opPieces[BQ] = opPieces[Bishop] | opPieces[Queen];
		opPieces[RQ] = opPieces[Rook] | opPieces[Queen];
	}

	my_pcs[BQ] = my_pcs[Bishop] | my_pcs[Queen];
	my_pcs[RQ] = my_pcs[Rook] | my_pcs[Queen];
	m_allPieces = *my_pcs | *opPieces;

	m_color = color;

	(m_posp)--;
	(m_nmoves)--;

	Q_ASSERT(m_nmoves >= 0);
}

/* Returns the number of times the current position has been reached
   in the game.  */
int Chessboard::getRepeatCount(int maxRepeats) const
{
	int i;
	int nrepeats;

	Q_ASSERT(m_nmoves >= m_posp->fifty);
	Q_ASSERT(m_posp->fifty >= 0);

	/* If the num. of reversible moves in a row is less than 4, then
	   there's no way we could already have a repetition.  */
	if (m_posp->fifty < 4)
		return 0;

	nrepeats = 0;
	for (i = 1; i <= m_posp->fifty; i++) {
		if ((m_posp - i)->key == m_posp->key) {
			nrepeats++;
			if (nrepeats >= maxRepeats)
				return nrepeats;
		}
	}

	return nrepeats;
}

