#include <QChar>
#include <iostream>
#include "chessboard.h"
#include "chesspiece.h"
#include "completechessmove.h"
#include "fen.h"
#include "zobrist.h"


Chessboard::Chessboard(int width, int height)
{
	m_width = width;
	m_height = height;
	m_boardSize = m_width * m_height;
	m_arraySize = (m_width + 2) * m_height;
	m_moveCount = 0;
	m_history.append(MoveData());
}

Chessboard::~Chessboard()
{
	clear();
}

Chessboard::ChessSide Chessboard::side() const
{
	return m_side;
}

int Chessboard::width() const
{
	return m_width;
}

int Chessboard::height() const
{
	return m_height;
}

int Chessboard::arrayWidth() const
{
	return m_width + 2;
}

ChessPiece* Chessboard::pieceAt(int square) const
{
	Q_ASSERT(isValidSquare(square));
	return m_squares[square];
}

void Chessboard::setSquare(int square, ChessPiece* piece)
{
	Q_ASSERT(isValidSquare(square));
	
	ChessPiece* old = m_squares[square];
	if (old != 0)
		updateZobristKey(Zobrist::piece(old->side(), old->type(), square));
	if (piece != 0)
		updateZobristKey(Zobrist::piece(piece->side(), piece->type(), square));
	m_squares[square] = piece;
}

bool Chessboard::isValidSquare(int square) const
{
	if (square < 0 || square >= m_arraySize)
		return false;
	
	int file = square % (m_width + 2);
	if (file == 0 || file == m_width + 1)
		return false;
	
	return true;
}

int Chessboard::squareToIndex(int square) const
{
	int rank = square / m_width;
	int file = (square % m_width) + 1;
	return (rank * (m_width + 2)) + file;
}

void Chessboard::clear()
{
	while (m_history.count() > 1)
		m_history.removeLast();
	for (int side = White; side <= Black; side++)
	{
		QLinkedList<ChessPiece*>* list = &m_pieces[side];
		QLinkedList<ChessPiece*>::iterator i = list->begin();
		while (i != list->end())
		{
			Q_ASSERT(*i != 0);
			delete *i;
			i = list->erase(i);
		}
	}
	
	for (int i = 0; i < m_arraySize; i++)
		m_squares[i] = 0;
}

QString Chessboard::fenString() const
{
	return Fen::fenString(this);
}

void Chessboard::addPiece(ChessPiece* piece)
{
	Q_ASSERT(piece != 0);
	Q_ASSERT(isValidSquare(piece->square()));

	setSquare(piece->square(), piece);
	
	m_pieces[piece->side()].append(piece);
	if (piece->type() == ChessPiece::PT_King)
		m_king[piece->side()] = piece;
}

void Chessboard::removePiece(ChessPiece* piece)
{
	Q_ASSERT(piece != 0);
	
	bool success = m_pieces[piece->side()].removeOne(piece);
	Q_ASSERT(success);
}

bool Chessboard::canSideAttackSquare(Chessboard::ChessSide side, int square)
{
	foreach(ChessPiece* piece, m_pieces[side])
	{
		Q_ASSERT(piece->enabled());
		if (piece->canAttack(square))
			return true;
	}
	
	return false;
}

bool Chessboard::isSideInCheck(Chessboard::ChessSide side)
{
	int square = m_king[side]->square();
	return canSideAttackSquare((ChessSide)!side, square);
}

CompleteChessMove Chessboard::completeMove(const ChessMove& move)
{
	int source = squareToIndex(move.sourceSquare());
	int target = squareToIndex(move.targetSquare());
	Q_ASSERT(isValidSquare(source));
	Q_ASSERT(isValidSquare(target));
	ChessPiece::PieceType promotion = move.promotion();
	ChessPiece* piece = pieceAt(source);

	if (piece == 0)
		return CompleteChessMove();

	// Fischer Random Castling
	const CastlingRights *cr = castlingRights();
 	bool allowCastling = false;
 	if (abs(source - target) != 1)
 		allowCastling = true;
	for (int cside = 0; cside < 2; cside++)
	{
		ChessPiece* rook = cr->rook(cside);
		if (rook != 0 && rook->square() == target)
		{
			target = cr->kingTarget(cside);
			allowCastling = true;
			break;
		}
	}

	QList<CompleteChessMove> moves;
	piece->generateMoves(&moves);

	foreach (CompleteChessMove tmpMove, moves)
	{
		if (tmpMove.sourceSquare() != source)
			continue;
		if (tmpMove.promotion() != promotion)
			continue;
		if (tmpMove.rookSquare() != -1 && !allowCastling)
			continue;
		if (tmpMove.targetSquare() != target)
			continue;
		
		return tmpMove;
	}
	
	return CompleteChessMove();
}

void Chessboard::generateMoves(QList<CompleteChessMove>* moves) const
{
	foreach(ChessPiece* piece, m_pieces[m_side])
	{
		Q_ASSERT(piece->enabled());
		piece->generateMoves(moves);
	}
}

void Chessboard::updateZobristKey(quint64 component)
{
	m_history.last().m_zobristKey ^= component;
}

int Chessboard::reversibleMoveCount() const
{
	return m_history.last().m_reversibleMoveCount;
}

void Chessboard::setReversibleMoveCount(int count)
{
	Q_ASSERT(count >= 0);
	m_history.last().m_reversibleMoveCount = count;
}

quint64 Chessboard::zobristKey() const
{
	return m_history.last().m_zobristKey;
}

void Chessboard::setZobristKey(quint64 key)
{
	m_history.last().m_zobristKey = key;
}

void Chessboard::makeMove(const CompleteChessMove& move)
{
	int source = move.sourceSquare();
	Q_ASSERT(isValidSquare(source));
	
	ChessPiece* piece = pieceAt(source);
	Q_ASSERT(piece != 0);
	Q_ASSERT(piece->enabled());
	Q_ASSERT(piece->side() == m_side);
	
	m_history.append(m_history.last());
	m_history.last().m_move = move;
	piece->makeMove(move);
	
	updateZobristKey(Zobrist::side());
	
	m_moveCount++;
	if (piece->type() == ChessPiece::PT_Pawn || move.capture() != 0 || move.rookSquare() != -1)
		setReversibleMoveCount(0);
	else
		setReversibleMoveCount(reversibleMoveCount() + 1);
	
	m_side = (m_side == White) ? Black : White;
}

void Chessboard::makeMove(const ChessMove& move)
{
	makeMove(completeMove(move));
}

void Chessboard::undoMove()
{
	Q_ASSERT(!m_history.isEmpty());
	CompleteChessMove move = m_history.takeLast().m_move;
	
	ChessPiece* piece;
	if (move.promotedPawn() != 0)
		piece = (ChessPiece*)move.promotedPawn();
	else
		piece = pieceAt(move.targetSquare());
	Q_ASSERT(piece != 0);
	
	piece->undoMove(move);

	m_side = (m_side == White) ? Black : White;
	m_moveCount--;

	ChessPiece* rook = move.castlingRook();
	if (rook != 0)
		rook->setSquare(move.rookSquare());
}

const CastlingRights* Chessboard::castlingRights(Chessboard::ChessSide side) const
{
	if (side == NoSide)
		side = m_side;
	return &m_history.last().m_castlingRights[side];
}

void Chessboard::setCastlingRights(Chessboard::ChessSide side, const CastlingRights& castlingRights)
{
	m_history.last().m_castlingRights[side] = castlingRights;
}

void Chessboard::disableCastlingRights(Chessboard::ChessSide side, int castlingSide)
{
	m_history.last().m_castlingRights[side].disable(castlingSide);
}

int Chessboard::enpassantSquare() const
{
	return m_history.last().m_enpassantSquare;
}

void Chessboard::setEnpassantSquare(int square)
{
	int& epSq = m_history.last().m_enpassantSquare;
	if (epSq != -1)
		updateZobristKey(Zobrist::enpassant(epSq));
	if (square != -1)
		updateZobristKey(Zobrist::enpassant(square));
	epSq = square;
}

bool Chessboard::setFenString(const QString& fenString)
{
	Fen fen(this, fenString);
	if (fen.isValid())
	{
		fen.apply();
		return true;
	}
	
	return false;
}

Chessboard::Result Chessboard::result()
{
	bool hasLegalMoves = false;
	QList<CompleteChessMove> moves;
	generateMoves(&moves);
	foreach (CompleteChessMove move, moves)
	{
		if (isMoveLegal(move))
		{
			hasLegalMoves = true;
			break;
		}
	}
	
	if (!hasLegalMoves)
	{
		if (isSideInCheck(m_side))
		{
			if (m_side == Black)
				return WhiteMates;
			else
				return BlackMates;
		}
		else
			return Stalemate;
	}
	
	bool enoughMaterial = false;
	for (int side = 0; side < 2; side++)
	{
		foreach (ChessPiece* piece, m_pieces[side])
		{
			ChessPiece::PieceType type = piece->type();
			if (type != ChessPiece::PT_King && type != ChessPiece::PT_Knight && type != ChessPiece::PT_Bishop)
			{
				enoughMaterial = true;
				break;
			}
		}
	}
	if (!enoughMaterial)
		return DrawByMaterial;
	
	if (reversibleMoveCount() >= 100)
		return DrawByFiftyMoves;
	
	if (repeatCount() >= 2)
		return DrawByRepetition;
	
	return NoResult;
}

/* Returns the number of times the current position has been reached
   in the game.  */
int Chessboard::repeatCount(int maxRepeats) const
{
	int repeatCount = 0;

	Q_ASSERT(m_history.count() >= reversibleMoveCount());

	/* If the num. of reversible moves in a row is less than 4, then
	   there's no way we could already have a repetition.  */
	if (reversibleMoveCount() < 4)
		return 0;

	int firstIndex = m_history.count() - 1;
	int lastIndex = firstIndex - reversibleMoveCount();
	for (int i = firstIndex; i > lastIndex; i--)
	{
		if (m_history[i].m_zobristKey == zobristKey())
		{
			repeatCount++;
			if (repeatCount >= maxRepeats)
				return repeatCount;
		}
	}

	return repeatCount;
}

void Chessboard::print() const
{
	QString str;
	for (int i = 0; i < m_boardSize; i++) {
		int square = squareToIndex(i);
		Q_ASSERT(isValidSquare(square));

		ChessPiece* piece = pieceAt(square);
		if (piece != 0)
			str += piece->toString();
		else
			str += ".";
		if ((i % m_width) == m_width - 1)
		{
			qDebug("%s", qPrintable(str));
			str.clear();
		}
	}
	qDebug("FEN: %s", qPrintable(fenString()));
	qDebug("Key: %llu", zobristKey());
}

bool Chessboard::isMoveLegal(const CompleteChessMove& move)
{
	bool isLegal = true;
	makeMove(move);
	if (isSideInCheck((ChessSide)!m_side))
		isLegal = false;
	
	undoMove();
	return isLegal;
}

bool Chessboard::isMoveLegal(const ChessMove& move)
{
	CompleteChessMove tmpMove = completeMove(move);
	if (tmpMove.isEmpty())
		return false;
	return isMoveLegal(tmpMove);
}

bool Chessboard::isMoveCheck(const CompleteChessMove& move)
{
	bool isCheck = false;
	makeMove(move);
	if (isSideInCheck(m_side))
		isCheck = true;

	undoMove();
	return isCheck;
}

bool Chessboard::isMoveMate(const CompleteChessMove& move)
{
	bool isMate = true;

	makeMove(move);
	if (isSideInCheck(m_side))
	{
		QList<CompleteChessMove> moves;
		generateMoves(&moves);
		foreach (CompleteChessMove tmpMove, moves)
		{
			if (isMoveLegal(tmpMove))
			{
				isMate = false;
				break;
			}
		}
	}
	else
		isMate = false;

	undoMove();
	return isMate;
}

qint64 Chessboard::perft(int depth, bool divide)
{
	Q_ASSERT(depth >= 0);

	if (depth == 0)
		return 1;

	qint64 nodes = 0;
	qint64 newNodes;
	QList<CompleteChessMove> moves;
	generateMoves(&moves);

	foreach (CompleteChessMove move, moves)
	{
		QString moveString = coordStringFromMove(move);
		makeMove(move);
		if (isSideInCheck((ChessSide)!m_side))
		{
			undoMove();
			continue;
		}
		
		newNodes = perft(depth - 1, false);
		if (divide)
			qDebug("%s %lld", qPrintable(moveString), newNodes);
		nodes += newNodes;
		undoMove();
	}

	return nodes;
}
