/*
    This file is part of SloppyGUI.

    SloppyGUI is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SloppyGUI is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SloppyGUI.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "chessboard.h"

using namespace Chess;


void Board::generateMoves(QVector<Chess::Move>& moves) const
{
	// Cut the wall squares (the ones with a value of InvalidPiece) off
	// from the squares to iterate over. It bumps the speed up a bit.
	unsigned begin = m_arwidth * 2;
	unsigned end = m_squares.size() - begin;
	
	moves.clear();
	
	for (unsigned sq = begin; sq < end; sq++) {
		int piece = m_squares[sq];
		if ((piece * m_sign) <= 0 || piece == InvalidPiece)
			continue;
		
		switch (piece * m_sign) {
		case Pawn:
			generatePawnMoves(sq, moves);
			break;
		case Knight:
			generateHoppingMoves(sq, m_knightOffsets, moves);
			break;
		case Bishop:
			generateSlidingMoves(sq, m_bishopOffsets, moves);
			break;
		case Rook:
			generateSlidingMoves(sq, m_rookOffsets, moves);
			break;
		case Queen:
			generateSlidingMoves(sq, m_bishopOffsets, moves);
			generateSlidingMoves(sq, m_rookOffsets, moves);
			break;
		case Archbishop:
			generateSlidingMoves(sq, m_bishopOffsets, moves);
			generateHoppingMoves(sq, m_knightOffsets, moves);
			break;
		case Chancellor:
			generateSlidingMoves(sq, m_rookOffsets, moves);
			generateHoppingMoves(sq, m_knightOffsets, moves);
			break;
		case King:
			generateHoppingMoves(sq, m_bishopOffsets, moves);
			generateHoppingMoves(sq, m_rookOffsets, moves);
			generateCastlingMoves(moves);
			break;
		}
	}
}

QVector<Move> Board::legalMoves()
{
	QVector<Move> moves;
	
	// Generate pseudo-legal moves
	generateMoves(moves);
	// Erase all illegal moves from the vector
	for (int i = moves.size() - 1; i >= 0; i--) {
		makeMove(moves[i]);
		if (!isLegalPosition())
			moves.erase(moves.begin() + i);
		undoMove();
	}
	
	return moves;
}

void Board::addPromotions(int sourceSquare,
                          int targetSquare,
                          QVector<Chess::Move>& moves) const
{
	moves.push_back(Move(sourceSquare, targetSquare, Knight));
	moves.push_back(Move(sourceSquare, targetSquare, Bishop));
	moves.push_back(Move(sourceSquare, targetSquare, Rook));
	moves.push_back(Move(sourceSquare, targetSquare, Queen));
	
	if (m_variant == CapablancaChess) {
		moves.push_back(Move(sourceSquare, targetSquare, Archbishop));
		moves.push_back(Move(sourceSquare, targetSquare, Chancellor));
	}
}

void Board::generatePawnMoves(int sourceSquare,
                              QVector<Chess::Move>& moves) const
{
	int targetSquare;
	int capture;
	int step = m_sign * m_arwidth;
	bool isPromotion = (m_squares[sourceSquare - step * 2] == InvalidPiece);
	
	// One square ahead
	targetSquare = sourceSquare - step;
	capture = m_squares[targetSquare];
	if (capture == NoPiece) {
		if (isPromotion)
			addPromotions(sourceSquare, targetSquare, moves);
		else {
			moves.push_back(Move(sourceSquare, targetSquare));
			
			// Two squares ahead
			int wall = m_squares[sourceSquare + step * 2];
			if (wall == InvalidPiece) {
				targetSquare -= step;
				capture = m_squares[targetSquare];
				if (capture == NoPiece)
					moves.push_back(Move(sourceSquare, targetSquare));
			}
		}
	}
	
	// Captures, including en-passant moves
	for (int i = -1; i <= 1; i += 2) {
		targetSquare = sourceSquare - step + i;
		capture = m_squares[targetSquare];
		if ((capture != InvalidPiece && capture * m_sign < 0)
		||  targetSquare == m_enpassantSquare) {
			if (isPromotion)
				addPromotions(sourceSquare, targetSquare, moves);
			else
				moves.push_back(Move(sourceSquare, targetSquare));
		}
	}
}

bool Board::canCastle(int castlingSide) const
{
	int rookSq = m_castlingRights.rookSquare[m_side][castlingSide];
	if (rookSq == 0)
		return false;
	
	int kingSq = m_kingSquare[m_side];
	int target = m_castleTarget[m_side][castlingSide];
	int left;
	int right;
	int rtarget;
	
	// Find all the squares involved in the castling
	if (castlingSide == QueenSide) {
		rtarget = target + 1;
		
		if (target < rookSq)
			left = target;
		else
			left = rookSq;
		
		if (rtarget > kingSq)
			right = rtarget;
		else
			right = kingSq;
	} else {
		rtarget = target - 1;
		
		if (rtarget < kingSq)
			left = rtarget;
		else
			left = kingSq;
		
		if (target > rookSq)
			right = target;
		else
			right = rookSq;
	}
	
	// Make sure that the smallest back rank interval containing the king,
	// the castling rook, and their destination squares contains no pieces
	// other than the king and the castling rook.
	for (int i = left; i <= right; i++) {
		if (i != kingSq && i != rookSq && m_squares[i] != NoPiece)
			return false;
	}

	return true;
}

void Board::generateCastlingMoves(QVector<Chess::Move>& moves) const
{
	int source = m_kingSquare[m_side];
	for (int i = QueenSide; i <= KingSide; i++) {
		if (canCastle(i)) {
			int target = m_castleTarget[m_side][i];
			moves.push_back(Move(source, target, NoPiece, i));
		}
	}
}

void Board::generateHoppingMoves(int sourceSquare,
                                 const QVector<int>& offsets,
                                 QVector<Chess::Move>& moves) const
{
	QVector<int>::const_iterator it;
	for (it = offsets.begin(); it != offsets.end(); ++it) {
		int targetSquare = sourceSquare + *it;
		int capture = m_squares[targetSquare];
		if (capture == InvalidPiece || capture * m_sign > 0)
			continue;
		moves.push_back(Move(sourceSquare, targetSquare));
	}
}

void Board::generateSlidingMoves(int sourceSquare,
                                 const QVector<int>& offsets,
                                 QVector<Chess::Move>& moves) const
{
	QVector<int>::const_iterator it;
	for (it = offsets.begin(); it != offsets.end(); ++it) {
		int targetSquare = sourceSquare + *it;
		int capture;
		while ((capture = m_squares[targetSquare]) != InvalidPiece
		&&      capture * m_sign <= 0) {
			moves.push_back(Move(sourceSquare, targetSquare));
			if (capture != NoPiece)
				break;
			targetSquare += *it;
		}
	}
}
