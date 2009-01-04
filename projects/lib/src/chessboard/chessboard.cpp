/*
    This file is part of Cute Chess.

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

#include "chessboard.h"
#include "notation.h"
#include "zobrist.h"
#include "bookmove.h"

using namespace Chess;


Board::Board(Variant variant, QObject* parent)
	: QObject(parent),
	  m_variant(NoVariant)
{
	setVariant(variant);
}

void Board::setVariant(Variant variant)
{
	if (variant == m_variant)
		return;
	
	m_variant = variant;
	m_isRandom = false;
	
	switch (m_variant) {
	case StandardChess:
		m_width = 8;
		m_height = 8;
		break;
	case CapablancaChess:
		m_width = 10;
		m_height = 8;
		break;
	default:
		Q_ASSERT(m_variant != NoVariant);
		break;
	}
	
	// Allocate the squares, with one 'wall' file on both sides,
	// and two 'wall' ranks at the top and bottom.
	m_arwidth = m_width + 2;
	m_squares.fill(InvalidPiece, m_arwidth * (m_height + 4));
	
	
	// Initialize the move offsets
	
	m_castleTarget[White][QueenSide] = (m_height + 1) * m_arwidth + 3;
	m_castleTarget[White][KingSide] = (m_height + 1) * m_arwidth + m_width - 1;
	m_castleTarget[Black][QueenSide] = 2 * m_arwidth + 3;
	m_castleTarget[Black][KingSide] = 2 * m_arwidth + m_width - 1;
	
	m_knightOffsets.resize(8);
	m_knightOffsets[0] = -2 * m_arwidth - 1;
	m_knightOffsets[1] = -2 * m_arwidth + 1;
	m_knightOffsets[2] = -m_arwidth - 2;
	m_knightOffsets[3] = -m_arwidth + 2;
	m_knightOffsets[4] = m_arwidth - 2;
	m_knightOffsets[5] = m_arwidth + 2;
	m_knightOffsets[6] = 2 * m_arwidth - 1;
	m_knightOffsets[7] = 2 * m_arwidth + 1;
	
	m_bishopOffsets.resize(4);
	m_bishopOffsets[0] = -m_arwidth - 1;
	m_bishopOffsets[1] = -m_arwidth + 1;
	m_bishopOffsets[2] = m_arwidth - 1;
	m_bishopOffsets[3] = m_arwidth + 1;
	
	m_rookOffsets.resize(4);
	m_rookOffsets[0] = -m_arwidth;
	m_rookOffsets[1] = -1;
	m_rookOffsets[2] = 1;
	m_rookOffsets[3] = m_arwidth;
}

void Board::initZobristKey()
{
	m_key = 0;
	
	for (int side = White; side <= Black; side++) {
		const int* rookSq = m_castlingRights.rookSquare[side];
		if (rookSq[QueenSide] != 0)
			m_key ^= Zobrist::castling(side, rookSq[QueenSide]);
		if (rookSq[KingSide] != 0)
			m_key ^= Zobrist::castling(side, rookSq[KingSide]);
		
		int sign = (side == White) ? 1 : -1;
		for (int sq = 0; sq < m_squares.size(); sq++) {
			int piece = m_squares[sq];
			if (piece == InvalidPiece || piece * sign <= 0)
				continue;
			
			m_key ^= Zobrist::piece(side, piece * sign, sq);
		}
	}
	
	if (m_enpassantSquare != 0)
		m_key ^= Zobrist::enpassant(m_enpassantSquare);
	if (m_side == White)
		m_key ^= Zobrist::side();
}

Side Board::sideToMove() const
{
	return (Side)m_side;
}

Variant Board::variant() const
{
	return m_variant;
}

quint64 Board::key() const
{
	return m_key;
}

QString Board::startingFen() const
{
	return m_startFen;
}

bool Board::isRandomVariant() const
{
	return m_isRandom;
}

int Board::width() const
{
	return m_width;
}

int Board::height() const
{
	return m_height;
}

int Board::pieceAt(const Square& square) const
{
	if (!isValidSquare(square))
		return InvalidPiece;
	
	return m_squares.at(squareIndex(square));
}

QVector<Move> Board::moveHistory() const
{
	QVector<Move> moves;
	
	foreach (const MoveData& md, m_history)
		moves.append(md.move);
	
	return moves;
}

void Board::print() const
{
	int i = m_arwidth * 2;
	for (int y = 0; y < m_height; y++) {
		i++;
		QString file;
		for (int x = 0; x < m_width; x++) {
			int pc = m_squares[i];
			QChar c = '.';
			
			if (pc != NoPiece)
				c = Notation::pieceChar(pc);
			file += c;
			file += ' ';
			
			i++;
		}
		i++;
		qDebug("%s", qPrintable(file));
	}
	qDebug("FEN: %s", qPrintable(fenString()));
}

Square Board::chessSquare(int index) const
{
	int file = (index % m_arwidth) - 1;
	int rank = (m_height - 1) - ((index / m_arwidth) - 2);
	Square square = { file, rank };
	
	return square;
}

int Board::squareIndex(const Square& square) const
{
	if (square.file < 0 || square.file >= m_width
	||  square.rank < 0 || square.rank >= m_height)
		return 0;
	
	int rank = (m_height - 1) - square.rank;
	return (rank + 2) * m_arwidth + 1 + square.file;
}

bool Board::isValidSquare(const Chess::Square& square) const
{
	if (square.file < 0 || square.file >= m_width
	||  square.rank < 0 || square.rank >= m_height)
		return false;
	return true;
}

Move Board::moveFromBook(const BookMove& bookMove) const
{
	int source = squareIndex(bookMove.sourceSquare());
	int target = squareIndex(bookMove.targetSquare());

	int castlingSide = -1;
	if ((m_squares[source] * m_sign) == King)
	{
		int diff = target - source;
		if (diff == -2 || diff == -3)
			castlingSide = QueenSide;
		else if (diff == 2 || diff == 3)
			castlingSide = KingSide;
	}
	
	return Move(source, target, bookMove.promotion(), castlingSide);
}

bool Board::inCheck(int side, int square) const
{
	if (square == 0)
		square = m_kingSquare[side];
	
	int sign = (side == White) ? 1 : -1;
	int attacker;
	
	// Pawn attacks
	int step = -sign * m_arwidth;
	// Left side
	attacker  = m_squares[square + step - 1];
	if ((attacker * sign) == -Pawn)
		return true;
	// Right side
	attacker = m_squares[square + step + 1];
	if ((attacker * sign) == -Pawn)
		return true;
	
	QVector<int>::const_iterator it;
	
	// Knight, archbishop, chancellor attacks
	for (it = m_knightOffsets.begin(); it != m_knightOffsets.end(); ++it) {
		attacker = m_squares[square + *it];
		switch (attacker * sign) {
		case -Knight: case -Archbishop: case -Chancellor:
			return true;
		}
	}
	
	// Bishop, queen, archbishop, king attacks
	for (it = m_bishopOffsets.begin(); it != m_bishopOffsets.end(); ++it) {
		int targetSquare = square + *it;
		if (targetSquare == m_kingSquare[!side])
			return true;
		while ((attacker = m_squares[targetSquare]) != InvalidPiece
		&&      attacker * sign <= 0) {
			switch (attacker * sign) {
			case -Bishop: case -Queen: case -Archbishop:
				return true;
			}
			if (attacker != NoPiece)
				break;
			targetSquare += *it;
		}
	}
	
	// Rook, queen, chancellor, king attacks
	for (it = m_rookOffsets.begin(); it != m_rookOffsets.end(); ++it) {
		int targetSquare = square + *it;
		if (targetSquare == m_kingSquare[!side])
			return true;
		while ((attacker = m_squares[targetSquare]) != InvalidPiece
		&&      attacker * sign <= 0) {
			switch (attacker * sign) {
			case -Rook: case -Queen: case -Chancellor:
				return true;
			}
			if (attacker != NoPiece)
				break;
			targetSquare += *it;
		}
	}
	
	return false;
}

void Board::makeMove(const Move& move, bool sendSignal)
{
	int source = move.sourceSquare();
	int target = move.targetSquare();
	int promotion = move.promotion();
	int piece = m_squares[source] * m_sign;
	int capture = m_squares[target];
	int epSq = m_enpassantSquare;
	int* rookSq = m_castlingRights.rookSquare[m_side];
	int rookSource = 0;
	int rookTarget = 0;
	int epTarget = 0;
	
	Q_ASSERT(source != 0);
	Q_ASSERT(target != 0);
	
	MoveData md = { move, capture, epSq, m_castlingRights,
	                m_key, m_reversibleMoveCount };
	
	m_key ^= Zobrist::piece(m_side, piece, source);
	
	if (epSq != 0) {
		m_key ^= Zobrist::enpassant(epSq);
		m_enpassantSquare = 0;
	}
	
	bool isReversible = true;
	if (piece == King) {
		m_kingSquare[m_side] = target;
		
		// In case of a castling move, make the rook's move
		if (move.castlingSide() != -1) {
			int cside = move.castlingSide();
			rookSource = rookSq[cside];
			rookTarget = (cside == QueenSide) ? target + 1 : target -1;
			
			m_squares[rookSource] = NoPiece;
			m_squares[rookTarget] = Rook * m_sign;
			m_key ^= Zobrist::piece(m_side, Rook, rookSource);
			m_key ^= Zobrist::piece(m_side, Rook, rookTarget);
			isReversible = false;
		}
		// Any king move removes all castling rights
		for (int i = QueenSide; i <= KingSide; i++) {
			int& rs = rookSq[i];
			if (rs != 0) {
				m_key ^= Zobrist::castling(m_side, rs);
				rs = 0;
			}
		}
	} else if (piece == Pawn) {
		isReversible = false;
		
		// Make an en-passant capture
		if (target == epSq) {
			epTarget = target + m_arwidth * m_sign;
			m_squares[epTarget] = NoPiece;
			m_key ^= Zobrist::piece(!m_side, Pawn, epTarget);
		// Push a pawn two squares ahead, creating an en-passant
		// opportunity for the opponent.
		} else if ((source - target) * m_sign == m_arwidth * 2) {
			if ((m_squares[target - 1] * m_sign) == -Pawn
			||  (m_squares[target + 1] * m_sign) == -Pawn) {
				m_enpassantSquare = source - m_arwidth * m_sign;
				m_key ^= Zobrist::enpassant(m_enpassantSquare);
			}
		} else if (promotion != NoPiece)
			piece = promotion;
	} else if (piece == Rook) {
		// Remove castling rights from the rook's square
		for (int i = QueenSide; i <= KingSide; i++) {
			if (source == rookSq[i]) {
				m_key ^= Zobrist::castling(m_side, source);
				rookSq[i] = 0;
				isReversible = false;
				break;
			}
		}
	}
	
	capture *= m_sign;
	// If the move captures opponent's castling rook, remove
	// his castling rights from that side.
	if (capture == -Rook) {
		int* opCr = m_castlingRights.rookSquare[!m_side];
		if (target == opCr[QueenSide]) {
			m_key ^= Zobrist::castling(!m_side, target);
			opCr[QueenSide] = 0;
		} else if (target == opCr[KingSide]) {
			m_key ^= Zobrist::castling(!m_side, target);
			opCr[KingSide] = 0;
		}
	}
	
	if (capture < 0) {
		isReversible = false;
		m_key ^= Zobrist::piece(!m_side, -capture, target);
	}
	m_key ^= Zobrist::side();
	m_key ^= Zobrist::piece(m_side, piece, target);
	m_squares[target] = piece * m_sign;
	m_squares[source] = NoPiece;
	
	if (isReversible)
		m_reversibleMoveCount++;
	else
		m_reversibleMoveCount = 0;
	
	m_history.push_back(md);
	m_sign *= -1;
	m_side = !m_side;
	
	if (sendSignal)
	{
		emit squareChanged(chessSquare(source));
		emit squareChanged(chessSquare(target));
		if (epTarget != 0)
			emit squareChanged(chessSquare(epTarget));
		if (rookSource != 0)
			emit squareChanged(chessSquare(rookSource));
		if (rookTarget != 0)
			emit squareChanged(chessSquare(rookTarget));
	}
}

void Board::undoMove()
{
	if (m_history.empty())
		return;
	
	const MoveData& md = m_history.back();
	const Move& move = md.move;
	int target = move.targetSquare();
	int source = move.sourceSquare();
	
	m_history.pop_back();
	m_sign *= -1;
	m_side = !m_side;
	
	m_enpassantSquare = md.enpassantSquare;
	m_castlingRights = md.castlingRights;
	m_key = md.key;
	m_reversibleMoveCount = md.reversibleMoveCount;
	
	if (target == m_kingSquare[m_side]) {
		m_kingSquare[m_side] = source;
		
		int cside = move.castlingSide();
		if (cside != -1) {
			// Move the rook back after castling
			if (cside == QueenSide)
				m_squares[target + 1] = NoPiece;
			else
				m_squares[target - 1] = NoPiece;
			const int* cr = m_castlingRights.rookSquare[m_side];
			m_squares[cr[cside]] = Rook * m_sign;
		}
	} else if (target == m_enpassantSquare) {
		// Restore the pawn captured by the en-passant move
		int epTarget = target + m_arwidth * m_sign;
		m_squares[epTarget] = -Pawn * m_sign;
	}
	
	if (move.promotion() != NoPiece)
		m_squares[source] = Pawn * m_sign;
	else
		m_squares[source] = m_squares[target];
	
	m_squares[target] = md.capture;
}

bool Board::isLegalPosition() const
{
	if (inCheck(!m_side))
		return false;
	
	if (m_history.empty())
		return true;
	
	const MoveData& md = m_history.back();
	const Move& move = md.move;
	
	// Make sure that no square between the king's initial and final
	// squares (including the initial and final squares) are under
	// attack (in check) by the opponent.
	if (move.castlingSide() != -1) {
		int source = move.sourceSquare();
		int target = move.targetSquare();
		int offset = (target >= source) ? 1 : -1;
		for (int i = source; i != target; i += offset) {
			if (inCheck(!m_side, i))
				return false;
		}
	}
	
	return true;
}

bool Board::isLegalMove(const Chess::Move& move)
{
	QVector<Move> moves = legalMoves();
	QVector<Move>::const_iterator it;
	for (it = moves.begin(); it != moves.end(); it++) {
		if (it->sourceSquare() == move.sourceSquare()
		&&  it->targetSquare() == move.targetSquare()
		&&  it->promotion() == move.promotion()
		&&  it->castlingSide() == move.castlingSide())
			return true;
	}
	
	return false;
}

Result Board::result()
{
	QVector<Move> moves(legalMoves());
	
	if (moves.size() == 0)
	{
		if (inCheck(m_side))
		{
			if (m_side == Black)
				return WhiteMates;
			else
				return BlackMates;
		}
		else
			return Stalemate;
	}
	
	int material[2] = { 0, 0 };
	QVector<int>::iterator it;
	for (it = m_squares.begin(); it != m_squares.end(); ++it)
	{
		int piece = *it;
		if (piece == NoPiece || piece == InvalidPiece)
			continue;
		int side;
		if (piece > 0)
			side = White;
		else
		{
			side = Black;
			piece *= -1;
		}
		
		if (piece == Knight || piece == Bishop)
			material[side] += 1;
		else
			material[side] += 2;
	}
	if (material[White] <= 3 && material[Black] <= 3)
		return DrawByMaterial;
	
	if (m_reversibleMoveCount >= 100)
		return DrawByFiftyMoves;
	
	if (repeatCount() >= 2)
		return DrawByRepetition;
	
	return NoResult;
}

int Board::repeatCount() const
{
	int repeatCount = 0;

	Q_ASSERT(m_history.size() >= m_reversibleMoveCount);

	// If the num. of reversible moves in a row is less than 4, then
	// there's no way we could already have a repetition.
	if (m_reversibleMoveCount < 4)
		return 0;

	int firstIndex = m_history.size() - 1;
	int lastIndex = firstIndex - m_reversibleMoveCount;
	for (int i = firstIndex; i > lastIndex; i--)
	{
		if (m_history[i].key == m_key)
			repeatCount++;
	}

	return repeatCount;
}

bool Board::isRepeatMove(const Chess::Move& move)
{
	bool isRepeat = false;
	
	makeMove(move);
	if (repeatCount() > 0)
		isRepeat = true;
	undoMove();
	
	return isRepeat;
}

