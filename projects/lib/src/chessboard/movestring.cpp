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

using namespace Chess;


QString Board::unicodeSanMove(Side side, const QString& sanMove)
{
	if (side == NoSide || sanMove.size() < 2)
		return sanMove;

	QChar c = sanMove.at(0);
	// Don't use piece symbols for pawn moves
	if (c.isLower())
		return sanMove;

	Piece piece(c);
	if (!piece.isValid())
		return sanMove;

	piece.setSide(side);
	QString str = piece.symbol() + sanMove.right(sanMove.size() - 1);

	// promotion
	c = str.at(str.size() - 1);
	if (c.isUpper())
	{
		Piece promotion(c);
		if (promotion.isValid())
		{
			promotion.setSide(side);
			str.replace(str.size() - 1, 1, promotion.symbol());
		}
	}

	return str;
}

QString Board::moveString(const Move& move, MoveNotation notation)
{
	switch (notation)
	{
	case StandardAlgebraic:
		return sanMoveString(move);
	case LongAlgebraic:
		// Long Algebraic notation doesn't support castling in random
		// variants like Fischer Random chess, so we'll use SAN for
		// castling.
		if (move.castlingSide() != -1 && m_variant.isRandom())
			return sanMoveString(move);
		return longAlgebraicMoveString(move);
	case UciLongAlgebraic:
		// UCI Long Algebraic needs the target square to be the
		// castling rook's square
		if (move.castlingSide() != -1 && m_variant.isRandom())
		{
			int cside = move.castlingSide();
			int src = move.sourceSquare();
			int trg = m_castlingRights.rookSquare[m_side][cside];
			Move tmp(src, trg, Piece::NoPiece, cside);
			return longAlgebraicMoveString(tmp);
		}
		return longAlgebraicMoveString(move);
	default:
		return QString();
	}
}

Move Board::moveFromString(const QString& str)
{
	Move move = moveFromSanString(str);
	if (move.isNull())
	{
		move = moveFromLongAlgebraicString(str);
		if (!isLegalMove(move))
			return Move();
	}
	return move;
}

QString Board::longAlgebraicMoveString(const Move& move) const
{
	Square source = chessSquare(move.sourceSquare());
	Square target = chessSquare(move.targetSquare());
	
	QString str = source.toString() + target.toString();
	
	if (move.promotion() != Piece::NoPiece)
		str += Piece(Black, move.promotion()).toChar();
	
	return str;
}

QString Board::sanMoveString(const Move& move)
{
	QString str;
	int source = move.sourceSquare();
	int target = move.targetSquare();
	Piece piece = m_squares[source];
	Piece capture = m_squares[target];
	Square square = chessSquare(source);
	
	char checkOrMate = 0;
	makeMove(move);
	if (inCheck(m_side))
	{
		if (canMove())
			checkOrMate = '+';
		else
			checkOrMate = '#';
	}
	undoMove();
	
	bool needRank = false;
	bool needFile = false;
	
	if (piece.type() == Piece::Pawn)
	{
		if (target == m_enpassantSquare)
			capture = Piece(Side(!m_side), Piece::Pawn);
		if (capture.isValid())
			needFile = true;
	}
	else if (piece.type() == Piece::King)
	{
		int cside = move.castlingSide();
		if (cside != -1)
		{
			if (cside == QueenSide)
				str = "O-O-O";
			else
				str = "O-O";
			if (checkOrMate != 0)
				str += checkOrMate;
			return str;
		}
		else
			str += piece.toChar().toUpper();
	}
	else	// not king or pawn
	{
		str += piece.toChar().toUpper();
		QVarLengthArray<Move> moves;
		generateMoves(moves, piece.type());
		
		for (int i = 0; i < moves.size(); i++)
		{
			const Move& move2 = moves[i];
			if (move2.sourceSquare() == source
			||  move2.targetSquare() != target)
				continue;
			
			makeMove(move2);
			bool isLegal = isLegalPosition();
			undoMove();
			if (!isLegal)
				continue;
			
			Square square2(chessSquare(move2.sourceSquare()));
			if (square2.file() != square.file())
				needFile = true;
			else if (square2.rank() != square.rank())
				needRank = true;
		}
	}
	if (needFile)
		str += 'a' + square.file();
	if (needRank)
		str += '1' + square.rank();

	if (capture.isValid())
		str += 'x';
	
	str += chessSquare(target).toString();

	if (move.promotion())
	{
		str += '=';
		str += Piece(White, move.promotion()).toChar();
	}

	if (checkOrMate != 0)
		str += checkOrMate;
	
	return str;
}

Move Board::moveFromLongAlgebraicString(const QString& str) const
{
	int len = str.length();
	if (len < 4)
		return Move();
	
	Square sourceSq(str.mid(0, 2));
	Square targetSq(str.mid(2, 2));
	if (!isValidSquare(sourceSq) || !isValidSquare(targetSq))
		return Move();
	
	Piece::Type promotion = Piece::NoPiece;
	if (len > 4)
	{
		promotion = Piece(str[len - 1]).type();
		if (promotion == Piece::NoPiece)
			return Move();
	}
	
	int source = squareIndex(sourceSq);
	int target = squareIndex(targetSq);
	
	int castlingSide = -1;
	if (m_squares[source] == Piece(m_side, Piece::King))
	{
		// If the king tries to capture its own rook, it's
		// a castling move in UciLongAlgebraic format.
		const int* rs = &m_castlingRights.rookSquare[m_side][0];
		for (int i = 0; i < 2; i++)
		{
			if (target == rs[i])
			{
				target = m_castleTarget[m_side][i];
				castlingSide = i;
				break;
			}
		}

		if (castlingSide == -1)
		{
			int diff = target - source;
			if (diff == -2 || diff == -3)
				castlingSide = QueenSide;
			else if (diff == 2 || diff == 3)
				castlingSide = KingSide;
		}
	}
	
	return Move(source, target, promotion, castlingSide);
}

Move Board::moveFromSanString(const QString& str)
{
	if (str.length() < 2)
		return Move();
	
	QString mstr = str;
	
	// Ignore check/mate/strong move/blunder notation
	while (mstr.endsWith('+') || mstr.endsWith('#')
	||     mstr.endsWith('!') || mstr.endsWith('?'))
	{
		mstr.chop(1);
	}
	
	if (mstr.length() < 2)
		return Move();

	// Castling
	if (mstr.startsWith("O-O"))
	{
		int cside;
		if (mstr == "O-O")
			cside = KingSide;
		else if (mstr == "O-O-O")
			cside = QueenSide;
		else
			return Move();
		
		int source = m_kingSquare[m_side];
		int target = m_castleTarget[m_side][cside];

		Move move(source, target, Piece::NoPiece, cside);
		if (isLegalMove(move))
			return move;
		else
			return Move();
	}
	
	Square sourceSq;
	Square targetSq;
	QString::const_iterator it = mstr.begin();
	
	// A SAN move can't start with the capture mark, and
	// a pawn move must not specify the piece type
	if (*it == 'x' || *it == 'P')
		return Move();
	
	// Piece type
	Piece piece = Piece(*it);
	if (piece.side() != White)
		piece = Piece::NoPiece;
	else
		piece.setSide(m_side);
	if (piece.isEmpty())
	{
		piece = Piece(m_side, Piece::Pawn);
		targetSq = mstr.mid(0, 2);
		if (isValidSquare(targetSq))
			it += 2;
	}
	else
		++it;
	
	bool stringIsCapture = false;
	
	if (!isValidSquare(targetSq))
	{
		// Source square's file
		sourceSq.setFile(it->toAscii() - 'a');
		if (sourceSq.file() < 0 || sourceSq.file() >= m_width)
			sourceSq.setFile(-1);
		else if (++it == mstr.end())
			return Move();

		// Source square's rank
		if (it->isDigit())
		{
			sourceSq.setRank(it->toAscii() - '1');
			if (sourceSq.rank() < 0 || sourceSq.rank() >= m_height)
				return Move();
			++it;
		}
		if (it == mstr.end())
		{
			// What we thought was the source square, was
			// actually the target square.
			if (isValidSquare(sourceSq))
			{
				targetSq = sourceSq;
				sourceSq.setRank(-1);
				sourceSq.setFile(-1);
			}
			else
				return Move();
		}
		// Capture
		else if (*it == 'x')
		{
			if(++it == mstr.end())
				return Move();
			stringIsCapture = true;
		}
		
		// Target square
		if (!isValidSquare(targetSq))
		{
			if (it + 1 == mstr.end())
				return Move();
			targetSq = mstr.mid(it - mstr.begin(), 2);
			it += 2;
		}
	}
	if (!isValidSquare(targetSq))
		return Move();
	int target = squareIndex(targetSq);
	
	// Make sure that the move string is right about whether
	// or not the move is a capture.
	bool isCapture = false;
	if (m_squares[target].side() == !m_side
	||  (target == m_enpassantSquare && piece.type() == Piece::Pawn))
		isCapture = true;
	if (isCapture != stringIsCapture)
		return Move();
	
	// Promotion
	int promotion = Piece::NoPiece;
	if (it != mstr.end())
	{
		if ((*it == '=' || *it == '(') && ++it == mstr.end())
			return Move();
		
		promotion = Piece(*it).type();
		if (promotion == Piece::NoPiece)
			return Move();
	}
	
	QVarLengthArray<Move> moves;
	generateMoves(moves, piece.type());
	const Move* match = 0;
	
	// Loop through all legal moves to find a move that matches
	// the data we got from the move string.
	for (int i = 0; i < moves.size(); i++)
	{
		const Move& move = moves[i];
		if (move.targetSquare() != target)
			continue;
		Square sourceSq2 = chessSquare(move.sourceSquare());
		if (sourceSq.rank() != -1 && sourceSq2.rank() != sourceSq.rank())
			continue;
		if (sourceSq.file() != -1 && sourceSq2.file() != sourceSq.file())
			continue;
		// Castling moves were handled earlier
		if (move.castlingSide() != -1)
			continue;
		if (move.promotion() != promotion)
			continue;

		makeMove(move);
		bool isLegal = isLegalPosition();
		undoMove();
		if (!isLegal)
			continue;
		
		// Return an empty move if there are multiple moves that
		// match the move string.
		if (match != 0)
			return Move();
		match = &move;
	}
	
	if (match != 0)
		return *match;
	
	return Move();
}
