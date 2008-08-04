#include "chessboard.h"
#include "util.h"
#include "notation.h"
#include "chessmove.h"

MoveList::MoveList(void)
{
	clear();
}

quint32 MoveList::operator[](int index)
{
	Q_ASSERT(index >= 0 && index < m_nmoves);
	return m_moves[index];
}

void MoveList::add(quint32 move)
{
	Q_ASSERT(m_nmoves < MAX_NMOVES);
	m_moves[m_nmoves++] = move;
}

int MoveList::count(void)
{
	return m_nmoves;
}

bool MoveList::moveExists(quint32& move)
{
	int i;
	for (i = 0; i < m_nmoves; i++) {
		quint32 tmp = m_moves[i];
		if (GET_FROM(move) == GET_FROM(tmp)
		&&  GET_TO(move) == GET_TO(tmp)
		&&  GET_PROM(move) == GET_PROM(tmp)) {
			move = tmp;
			return true;
		}
	}
	
	return false;
}

void MoveList::clear(void)
{
	m_nmoves = 0;
}

Chessboard::Chessboard(void)
{
	m_nmoves = -1;
	m_color = NoSide;
}

Chessboard::Chessboard(const QString& fen)
{
	setFenString(fen);
}

int Chessboard::moveCount(void) const
{
	return m_nmoves;
}

void Chessboard::makeMove(const ChessMove& move)
{
	quint32 fullMove = intMove(move.sourceSquare(), move.targetSquare(), move.promotion());
	Q_ASSERT(fullMove != NULLMOVE);
	makeMove(fullMove);
}

void Chessboard::makeMove(const QString& moveString)
{
	quint32 move;
	if (isMoveString(moveString))
		move = moveFromCoord(moveString);
	else
		move = moveFromSan(moveString);
	
	if (move == NULLMOVE || move == MOVE_ERROR)
		throw -1;
	makeMove(move);
}

bool Chessboard::isLegalMove(const ChessMove& move)
{
	quint32 fullMove = intMove(move.sourceSquare(), move.targetSquare(), move.promotion());
	if (fullMove == NULLMOVE)
		return false;
	return true;
}

QVector<bool> Chessboard::targetSquares(Chessboard::ChessSquare from) const
{
	int i;
	MoveList moveList;
	QVector<bool> targetSquares(64, 0);
	
	if (m_mailbox[from] == 0)
		return targetSquares;
	
	generateMoves(&moveList);
	for (i = 0; i < moveList.count(); i++) {
		quint32 move = moveList[i];
		if ((Chessboard::ChessSquare)GET_FROM(move) != from)
			break;
		targetSquares[GET_TO(move)] = true;
	}
	
	return targetSquares;
}

Chessboard::ChessPiece Chessboard::pieceAt(Chessboard::ChessSquare square) const
{
	if (!isOnBoard(square))
		return NoPiece;
	return (Chessboard::ChessPiece)m_mailbox[square];
}

Chessboard::ChessSide Chessboard::side(void) const
{
	return (ChessSide)m_color;
}

/* Returns true if both sides have insufficient mating material.  */
bool Chessboard::insufficientMaterial() const
{
	const quint64* whites = &m_pieces[White][AllPieces];
	const quint64* blacks = &m_pieces[Black][AllPieces];
	
	if (popcount(*whites) <= 2
	&&  *whites == (whites[King] | whites[Knight] | whites[Bishop])
	&&  popcount(*blacks) <= 2
	&&  *blacks == (blacks[King] | blacks[Knight] | blacks[Bishop]))
		return true;
	
	return false;
}

Chessboard::ChessResult Chessboard::getMateType()
{
	MoveList moveList;

	generateMoves(&moveList);
	if (moveList.count() > 0)
		return NoResult;

	if (m_posp->inCheck) {
		if (m_color == Black)
			return WhiteMates;
		return BlackMates;
	}

	return Stalemate;
}

/* Print the result of the game and return true if the game is over.
   Otherwise just return false.  */
Chessboard::ChessResult Chessboard::result()
{
	ChessResult mateType = getMateType();
	if (mateType != NoResult)
		return mateType;

	if (getRepeatCount() >= 2)
		return DrawByRepetition;
	else if (insufficientMaterial())
		return DrawByMaterial;
	else if (m_posp->fifty >= 100)
		return DrawByFiftyMoves;

	return NoResult;
}

quint64 Chessboard::perft(int depth)
{
	int i;
	int nmoves;
	quint64 nnodes = 0;
	MoveList moveList;
	
	Q_ASSERT(depth >= 0);

	if (depth == 0)
		return 1;

	generateMoves(&moveList);
	nmoves = moveList.count();
	if (depth == 1 || nmoves == 0)
		return nmoves;

	for (i = 0; i < nmoves; i++) {
		quint32 move = moveList[i];

		makeMove(move);
		nnodes += perft(depth - 1);
		undoMove();
	}

	return nnodes;
}

