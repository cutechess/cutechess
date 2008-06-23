#include <iostream>
#include <QStringList>
#include "chessboard.h"
#include "util.h"
#include "notation.h"


/* Convert a piece type from character into an integer.  */
static int getPieceTypeInt(const QChar& pieceTypeChar)
{
	switch (pieceTypeChar.toAscii()) {
	case 'P':
		return Chessboard::Pawn;
	case 'N':
		return Chessboard::Knight;
	case 'B':
		return Chessboard::Bishop;
	case 'R':
		return Chessboard::Rook;
	case 'Q':
		return Chessboard::Queen;
	case 'K':
		return Chessboard::King;
	default:
		return 0;
	}
}

/* Convert a piece type from integer into a character.  */
QChar getPieceTypeChar(int pieceTypeInt)
{
	switch (pieceTypeInt) {
	case Chessboard::Pawn:
		return 'P';
	case Chessboard::Knight:
		return 'N';
	case Chessboard::Bishop:
		return 'B';
	case Chessboard::Rook:
		return 'R';
	case Chessboard::Queen:
		return 'Q';
	case Chessboard::King:
		return 'K';
	default:
		return '\0';
	}
}

/* Convert a chess board file letter ('a' to 'h') into an integer of 0 to 7.  */
static int getFileInt(const QChar& fileChar)
{
	char c = fileChar.toAscii();
	if (c >= 'a' && c <= 'h')
		return (int)(c - 'a');
	return -1;
}

/* Convert a chess board file (0 to 7) into a character ('a' to 'h').  */
static QChar getFileChar(int fileInt)
{
	if (fileInt >= 0 && fileInt <= 7)
		return QChar('a' + fileInt);
	return '\0';
}

/* Convert a chess board rank number ('1' to '8') into an integer of 0 to 7.  */
static int getRankInt(const QChar& rankChar)
{
	char c = rankChar.toAscii();
	if (c >= '1' && c <= '8')
		return 7 - (int)(c - '1');
	return -1;
}

/* Convert a chess board rank (0 to 7) into a character ('1' to '8').  */
static QChar getRankChar(int rankInt)
{
	if (rankInt >= 0 && rankInt <= 7)
		return QChar('1' + (7 - rankInt));
	return '\0';
}

/* Convert a promotion character into int format.  */
static int getPromotionInt(const QChar& promotionChar)
{
	switch (promotionChar.toAscii()) {
	case 'r':
		return Chessboard::Rook;
	case 'b':
		return Chessboard::Bishop;
	case 'n':
		return Chessboard::Knight;
	case 'q':
		return Chessboard::Queen;
	default:
		return -1;
	}
}

/* Convert a promotion from integer to character.  */
static QChar getPromotionChar(int promotionInt)
{
	switch (promotionInt) {
	case Chessboard::Knight:
		return 'N';
	case Chessboard::Bishop:
		return 'B';
	case Chessboard::Rook:
		return 'R';
	case Chessboard::Queen:
		return 'Q';
	default:
		return '\0';
	}
}

/* Convert a chess square from string (eg. "e4") to int format (0 to 63).  */
static int getSquareFromString(const QString& squareString)
{
	int file;
	int rank;
	
	/* Strings shorter than 2 character aren't accepted, but longer ones
	   are because the string is often a part of a bigger string.  */
	if (squareString.length() < 2)
		return -1;

	file = getFileInt(squareString[0]);
	rank = getRankInt(squareString[1]);

	if (file == -1 || rank == -1)
		return -1;

	return (rank * 8) + file;
}

/* Convert an enpassant square from string to int format (0 to 63).  */
static int getEnpassantSquareInt(const QString& squareString)
{
	int square;
	
	/* Enpassant capture is not possible.  */
	if (squareString[0] == '-')
		return 0;

	square = getSquareFromString(squareString);
	if (square == -1)
		return -1;

	return square;
}

/* Convert a chess game move count from string to int format.  */
static int getMoveCountInt(const QString& moveCountString)
{
	int nmoves;
	
	if (moveCountString.isEmpty())
		return -1;

	nmoves = moveCountString.toInt();
	if (nmoves < 0 || (nmoves * 2) >= MAX_NMOVES_PER_GAME)
		return -1;

	return nmoves;
}

/* Converts the color from a character ('w' or 'b')
   into integer format (White or Black).  */
static int getColorInt(const QChar& colorChar)
{
	switch (colorChar.toAscii()) {
	case 'w':
		return Chessboard::White;
	case 'b':
		return Chessboard::Black;
	default:
		return -1;
	}
}

/* Returns true if <word> is a move string (in coordinate notation).
   It doesn't have to be a legal or even a pseudo-legal move though.  */
bool isMoveString(const QString& word)
{
	int from;
	int to;
	int len;

	len = word.length();
	if (len < 4 || len > 5)
		return false;
	
	from = getSquareFromString(word.mid(0, 2));
	if (from == -1)
		return false;

	to = getSquareFromString(word.mid(2, 2));
	if (to == -1)
		return false;

	if (len > 4) {
		int promotion = getPromotionInt(word[4]);
		if (promotion == -1)
			return false;
	}
	
	return true;
}

/* Convert a move string (in coordinate notation) into a move.  */
quint32 Chessboard::moveFromCoord(const QString& moveString)
{
	int i;
	int piece;
	int from;
	int to;
	int promotion = 0;
	int nmoves;
	MoveList moveList;

	from = getSquareFromString(moveString.mid(0, 2));
	if (from == -1)
		return MOVE_ERROR;

	to = getSquareFromString(moveString.mid(2, 2));
	if (to == -1)
		return MOVE_ERROR;

	if (moveString.length() > 4) {
		promotion = getPromotionInt(moveString[4]);
		if (promotion == -1)
			return MOVE_ERROR;
	}

	piece = m_mailbox[from];
	generateMovesForPieceType(&moveList, piece, to);
	nmoves = moveList.count();
	for (i = 0; i < nmoves; i++) {
		quint32 move = moveList[i];

		if ((int)GET_FROM(move) == from && (int)GET_PROM(move) == promotion)
			return move;
	}

	return NULLMOVE;
}

/* Convert a move into a move string (in coordinate notation).  */
QString Chessboard::coordMoveString(quint32 move) const
{
	int promotion;
	QChar moveString[6];
	QChar *pos = moveString;

	promotion = GET_PROM(move);
	*pos++ = getFileChar(SQ_FILE(GET_FROM(move)));
	*pos++ = getRankChar(SQ_RANK(GET_FROM(move)));
	*pos++ = getFileChar(SQ_FILE(GET_TO(move)));
	*pos++ = getRankChar(SQ_RANK(GET_TO(move)));
	if (promotion != 0)
		*pos++ = getPromotionChar(promotion).toLower();
	*pos++ = '\0';
	
	return QString(moveString, 6);
}

/* Find out how much detail is needed to describe a move by <piece> to square <to>
   so that it can't be mixed up with any other legal moves.  */
#define SAN_UNIQUE_MOVE 00
#define SAN_FILE_NEEDED 01
#define SAN_RANK_NEEDED 02
unsigned Chessboard::neededMoveDetails(int piece, int from, int to)
{
	bool unique = true;
	bool uniqueRank = true;
	bool uniqueFile = true;
	unsigned ret = SAN_UNIQUE_MOVE;
	int i;
	int nmoves;
	MoveList moveList;
	
	generateMovesForPieceType(&moveList, piece, to);
	nmoves = moveList.count();
	for (i = 0; i < nmoves; i++) {
		int from2 = GET_FROM(moveList[i]);

		if (from2 != from) {
			unique = false;
			if (SQ_FILE(from2) == SQ_FILE(from))
				uniqueFile = false;
			if (SQ_RANK(from2) == SQ_RANK(from))
				uniqueRank = false;
		}
	}
	if (!unique) {
		if (!uniqueRank || uniqueFile)
			ret |= SAN_FILE_NEEDED;
		if (!uniqueFile)
			ret |= SAN_RANK_NEEDED;
	}

	return ret;
}

/* Find out whether a move is a normal move, a check, or a mate.  */
MoveType Chessboard::getMoveType(quint32 move)
{
	MoveType moveType;
	MoveList moveList;

	if (!IS_CHECK(move))
		return MT_NORMAL;

	moveType = MT_CHECK;
	makeMove(move);

	Q_ASSERT(isSideToMoveInCheck());
	generateMoves(&moveList);
	if (moveList.count() == 0)
		moveType = MT_MATE;

	undoMove();

	return moveType;
}

/* Convert a move into a move string (in SAN notation).  */
QString Chessboard::sanMoveString(quint32 move)
{
	MoveType moveType;
	int piece;
	int from;
	int to;
	QChar sanMove[10];
	QChar *pos = sanMove;

	Q_ASSERT(move != NULLMOVE);

	moveType = getMoveType(move);

	if (IS_CASTLING(move)) {
		QString tmp;
		if (GET_CASTLE(move) == C_KSIDE)
			tmp = "O-O";
		else
			tmp = "O-O-O";

		if (moveType == MT_CHECK)
			tmp += "+";
		else if (moveType == MT_MATE)
			tmp += "#";

		return tmp;
	}

	piece = GET_PC(move);
	from = GET_FROM(move);
	to = GET_TO(move);
	if (piece != Pawn) {
		unsigned nmd;

		*pos++ = getPieceTypeChar(piece);
		nmd = neededMoveDetails(piece, from, to);
		if (nmd & SAN_FILE_NEEDED)
			*pos++ = getFileChar(SQ_FILE(from));
		if (nmd & SAN_RANK_NEEDED)
			*pos++ = getRankChar(SQ_RANK(from));
	}

	if (GET_CAPT(move)) {
		if (piece == Pawn)
			*pos++ = getFileChar(SQ_FILE(from));
		*pos++ = 'x';
	}

	*pos++ = getFileChar(SQ_FILE(to));
	*pos++ = getRankChar(SQ_RANK(to));
	
	if (GET_PROM(move)) {
		*pos++ = '=';
		*pos++ = getPromotionChar(GET_PROM(move));
	}

	if (moveType == MT_CHECK)
		*pos++ = '+';
	else if (moveType == MT_MATE)
		*pos++ = '#';

	*pos++ = '\0';
	return QString(sanMove, pos - sanMove);
}

/* Convert a move string (in SAN notation) into a move.  */
quint32 Chessboard::moveFromSan(const QString& sanMove)
{
	int i;
	int to;
	int piece;
	int nmoves;
	MoveList moveList;
	
	/* In the SAN format every move must start with a letter.  */
	//if (!isalpha(*san_move))
	if (sanMove.length() < 2 || !sanMove[0].isLetter())
		return NULLMOVE;

	/* Castling moves.  */
	//if (!strncmp(san_move, "O-O-O", 5)) {
	if (sanMove.startsWith("O-O-O")) {
		piece = King;
		to = castling.kingSquares[m_color][C_QSIDE][C_TO];
	//} else if (!strncmp(san_move, "O-O", 3)) {
	} else if (sanMove.startsWith("O-O")) {
		piece = King;
		to = castling.kingSquares[m_color][C_KSIDE][C_TO];
	} else {
		//const char *lastc = san_move + strlen(san_move) - 1;
		const QChar *lastc = sanMove.data() + sanMove.length() - 1;

		/* Ignore the possible check or checkmate symbol
		   at the end of the move string.  */
		if (*lastc == '#' || *lastc == '+')
			lastc--;

		piece = getPieceTypeInt(sanMove[0]);
		if (piece == 0)
			piece = Pawn;
		/* Ignore the possible promotion.  */
		if (piece == Pawn && getPieceTypeInt(*lastc) != 0)
			lastc -= 2;

		to = getSquareFromString(QString(lastc - 1, 2));
		if (to == -1)
			return NULLMOVE;
	}

	generateMovesForPieceType(&moveList, piece, to);
	nmoves = moveList.count();
	if (nmoves == 1)
		return moveList[0];
	for (i = 0; i < nmoves; i++) {
		if (sanMoveString(moveList[i]) == sanMove)
			return moveList[i];
	}

	return NULLMOVE;
}

/* Convert castling rights from a part of a FEN string,
   into our own format.  */
static int getCastleRights(const QString& fen)
{
	unsigned castleRights = 0;
	int len;
	
	len = fen.length();
	if (len < 1 || len > 4)
		return -1;

	if (fen[0] == '-' && len == 1)
		return 0;

	foreach (QChar c, fen) {
		switch (c.toAscii()) {
		case 'K':
			castleRights |= castling.rights[Chessboard::White][C_KSIDE];
			break;
		case 'Q':
			castleRights |= castling.rights[Chessboard::White][C_QSIDE];
			break;
		case 'k':
			castleRights |= castling.rights[Chessboard::Black][C_KSIDE];
			break;
		case 'q':
			castleRights |= castling.rights[Chessboard::Black][C_QSIDE];
			break;
		default:
			return -1;
		}
	}

	return (int)castleRights;
}

/* Convert a FEN board into mailbox format.  */
static int mailboxFromFenString(const QString fen, int *mailbox)
{
	int square = 0;
	int rankEndSquare = 0;	/* last square of the previous rank */
	
	Q_ASSERT(mailbox != NULL);

	if (fen.length() < 15)
		return -1;

	foreach (QChar c, fen) {
		int piece = 0;

		/* Move to the next rank.  */
		if (c == '/') {
			/* Reject the FEN string if the rank didn't
			   have exactly 8 squares.  */
			if (square - rankEndSquare != 8)
				return -1;
			rankEndSquare = square;
			continue;
		}
		/* Add 1-8 empty squares.  */ 
		if (c.isDigit()) {
			int j;
			int nempty = c.digitValue();

			if (nempty < 1 || nempty > 8 || square + nempty > 64)
				return -1;
			for (j = 0; j < nempty; j++)
				mailbox[square++] = 0;
			continue;
		}
		/* Add a white piece.  */
		else if (c.isUpper())
			piece = getPieceTypeInt(c);
		/* Add a black piece.  */
		else if (c.isLower())
			piece = -getPieceTypeInt(c.toUpper());
		if (piece == 0 || square > 63)
			return -1;
		mailbox[square++] = piece;
	}
	/* The board must have exactly 64 squares (0 to 63) and each rank
	   must have exactly 8 squares.  */
	if (square != 64 || square - rankEndSquare != 8)
		return -1;

	return 0;
}

/* Set piece masks in a board according to a mailbox encoding.  */
void Chessboard::setSquares(const int *mailbox)
{
	int color;
	
	Q_ASSERT(mailbox != NULL);

	m_allPieces = 0;
	for (color = White; color <= Black; color++) {
		int i;
		int sign = SIGN(color);
		quint64 *my_pcs = &m_pieces[color][AllPieces];
		for (i = AllPieces; i <= King; i++)
			my_pcs[i] = 0;
		for (i = 0; i < 64; i++) {
			int piece = abs(mailbox[i]);
			m_mailbox[i] = piece;
			if (sign*mailbox[i] > 0) {
				my_pcs[piece] |= bit64[i];
				my_pcs[AllPieces] |= bit64[i];
			}
		}
		my_pcs[BQ] = my_pcs[Bishop] | my_pcs[Queen];
		my_pcs[RQ] = my_pcs[Rook] | my_pcs[Queen];
		m_allPieces |= my_pcs[AllPieces];
		m_kingSquares[color] = getLsb(my_pcs[King]);
	}
}

/* Set the board to position <fen> which uses the Forsyth-Edwards notation:
   http://en.wikipedia.org/wiki/Forsyth-Edwards_Notation  */
void Chessboard::setFenString(const QString& fen)
{
	int mailbox[64];
	int color;
	int castleRights;
	int enpassantSquare;
	int fifty;
	int nmoves;
	int i = 0;


	QStringList fenItems = fen.split(' ');
	if (fenItems.count() < 4)
		throw -1;

	/* Get squares and piece positions.  */
	if (mailboxFromFenString(fenItems[i++], mailbox) != 0)
		throw -1;

	/* Get side to move.  */
	color = getColorInt(fenItems[i++][0]);
	if (color == -1)
		throw -1;

	/* Get castling rights.  */
	castleRights = getCastleRights(fenItems[i++]);
	if (castleRights == -1)
		throw -1;

	/* Get enpassant square.  */
	enpassantSquare = getEnpassantSquareInt(fenItems[i++]);
	if (enpassantSquare == -1)
		throw -1;

	/* Get move count for the fifty-move rule.  */
	if (fenItems.count() > i)
		fifty = getMoveCountInt(fenItems[i++]);
	else
		fifty = 0;
	if (fifty < 0 || fifty > 99)
		throw -1;

	/* Get move number of the current full move.  */
	if (fenItems.count() > i)
		nmoves = (getMoveCountInt(fenItems[i++]) - 1) * 2;
	else
		nmoves = 0;
	if (color == Black)
		nmoves++;
	if (nmoves < 0 || nmoves >= MAX_NMOVES_PER_GAME)
		throw -1;

	m_color = color;
	m_nmoves = nmoves;

	/* Clear game history in case we're not at move no. 1.  */
	for (i = 0; i < nmoves; i++) {
		PosInfo *tmp_pos = &m_pos[i];
		
		tmp_pos->key = 0;
		tmp_pos->move = NULLMOVE;
		tmp_pos->castleRights = 0;
		tmp_pos->enpassantSquare = 0;
		tmp_pos->fifty = 0;
		tmp_pos->inCheck = false;
	}

	m_posp = &m_pos[nmoves];
	m_posp->castleRights = (unsigned)castleRights;
	m_posp->enpassantSquare = enpassantSquare;
	m_posp->fifty = fifty;
	m_posp->move = NULLMOVE;
	
	setSquares(mailbox);

	if (isSideToMoveInCheck())
		m_posp->inCheck = true;
	else
		m_posp->inCheck = false;

	computeZobristKey();
}

/* Convert a board into a board string (in FEN notation).  */
QString Chessboard::fenString() const
{
	QChar buf[128];
	QChar *fen;
	int square;
	int nempty = 0;	/* num. of successive empty squares on a rank */
	int enpassantSquare;	/* enpassant square */
	int fifty;	/* num. of successive reversible moves */
	unsigned castleRights;
	
	fen = buf;
	castleRights = m_posp->castleRights;
	enpassantSquare = m_posp->enpassantSquare;
	fifty = m_posp->fifty;

	for (square = 0; square < 64; square++) {
		/* Add a slash character between the ranks.  */
		if (square > 0 && SQ_FILE(square) == 0)
			*fen++ = '/';
		/* There's a piece on <square>.  */
		if (m_mailbox[square] != 0) {
			QChar piece = getPieceTypeChar(m_mailbox[square]);

			/* Add the num. of empty squares before <square>.  */
			if (nempty > 0) {
				*fen++ = QChar('0' + nempty);
				nempty = 0;
			}

			if (m_pieces[White][AllPieces] & bit64[square])
				*fen++ = piece;
			else
				*fen++ = piece.toLower();
		/* When the H file is reached we add the number of
		   successive empty squares, and reset the counter.  */
		} else if (SQ_FILE(square) == 7) {
			*fen++ = QChar('0' + (nempty + 1));
			nempty = 0;
		/* <square> is empty, so increase the counter.  */
		} else
			nempty++;
	}
	*fen++ = ' ';
	
	/* Add the side to move.  */
	if (m_color == White)
		*fen++ = 'w';
	else
		*fen++ = 'b';
	*fen++ = ' ';

	/* Add castling rights.  */
	if (!(castleRights & castling.allRights[White])
	&&  !(castleRights & castling.allRights[Black]))
		*fen++ = '-';
	else {
		if (castleRights & castling.rights[White][C_KSIDE])
			*fen++ = 'K';
		if (castleRights & castling.rights[White][C_QSIDE])
			*fen++ = 'Q';
		if (castleRights & castling.rights[Black][C_KSIDE])
			*fen++ = 'k';
		if (castleRights & castling.rights[Black][C_QSIDE])
			*fen++ = 'q';
	}
	*fen++ = ' ';

	/* Add enpassant square.  */
	if (enpassantSquare != 0) {
		*fen++ = getFileChar(SQ_FILE(enpassantSquare));
		*fen++ = getRankChar(SQ_RANK(enpassantSquare));
	} else
		*fen++ = '-';
	*fen++ = ' ';
	
	return QString(buf, fen - buf) + QString::number(fifty) + ' ' + QString::number((m_nmoves / 2) + 1);
}

/* Display an ASCII version of the board.  */
void Chessboard::print(void) const
{
	QChar c;
	int i;
	
	for (i = 0; i < 64; i++) {
		if (m_mailbox[i]) {
			if (m_pieces[White][AllPieces] & bit64[i])
				c = getPieceTypeChar(m_mailbox[i]);
			else
				c = getPieceTypeChar(m_mailbox[i]).toLower();
		} else
			c = '.';
		if (!SQ_FILE(i))
			std::cout << std::endl;
		if (SQ_FILE(i + 1))
			std::cout << c.toAscii() << " ";
		else
			std::cout << c.toAscii();
	}
	std::cout << std::endl << std::endl;

	// Print the position in FEN notation
	std::cout << "Fen: " << fenString().toStdString() << std::endl;
}

