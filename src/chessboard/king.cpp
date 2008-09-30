#include "king.h"
#include "completechessmove.h"
#include "zobrist.h"


King::King(Chessboard::ChessSide side, int square, Chessboard* board)
: ChessPiece(side, square, board)
{
	int vStep = board->arrayWidth();
	m_hopOffsetCount = 8;
	m_slideOffsetCount = 0;
	
	m_hopOffsets[0] = -vStep + 1;
	m_hopOffsets[1] = -vStep - 1;
	m_hopOffsets[2] = vStep - 1;
	m_hopOffsets[3] = vStep + 1;
	m_hopOffsets[4] = -vStep;
	m_hopOffsets[5] = -1;
	m_hopOffsets[6] = 1;
	m_hopOffsets[7] = vStep;
}

ChessPiece::PieceType King::type() const
{
	return PT_King;
}

bool King::canCastle(int castleSide) const
{
	const CastlingRights* cr = m_board->castlingRights();
	if (!cr->canCastle(castleSide))
		return false;
	
	int target = cr->kingTarget(castleSide);
	ChessPiece* rook = cr->rook(castleSide);
	Q_ASSERT(rook != 0);
	int leftMost = -1;
	int rightMost = -1;

	if (castleSide == CastlingRights::LeftSide)
	{
		int rookTarget = target + 1;
		
		leftMost = rook->square();
		if (target < leftMost)
			leftMost = target;
		
		rightMost = m_square;
		if (rookTarget > rightMost)
			rightMost = rookTarget;
	}
	else
	{
		int rookTarget = target - 1;
		
		leftMost = m_square;
		if (rookTarget < leftMost)
			leftMost = rookTarget;
		
		rightMost = rook->square();
		if (target > rightMost)
			rightMost = target;
	}

	for (int i = leftMost; i <= rightMost; i++)
	{
		ChessPiece* piece = m_board->pieceAt(i);
		if (piece != 0 && piece != rook && piece != this)
			return false;
	}

	int offset = (target >= m_square) ? 1 : -1;
	for (int i = m_square; i != target + offset; i += offset)
	{
		if (m_board->canSideAttackSquare((Chessboard::ChessSide)!m_side, i))
			return false;
	}

	return true;
}

void King::generateMoves(QList<CompleteChessMove>* moves) const
{
	// normal king moves and captures
	ChessPiece::generateMoves(moves);

	// castling moves
	const CastlingRights* cr = m_board->castlingRights();
	for (int i = CastlingRights::LeftSide; i <= CastlingRights::RightSide; i++)
	{
		if (canCastle(i))
		{
			int target = cr->kingTarget(i);
			ChessPiece* rook = cr->rook(i);
			moves->append(CompleteChessMove(m_square, target, 0, PT_None, 0, rook));
		}
	}
}

void King::makeMove(const CompleteChessMove& move)
{
	int target = move.targetSquare();
	ChessPiece* rook = move.castlingRook();
	
	for (int cside = 0; cside < 2; cside++)
	{
		if (m_board->castlingRights()->canCastle(cside))
		{
			m_board->disableCastlingRights(m_side, cside);
			m_board->updateZobristKey(Zobrist::castling(m_side, cside));
		}
	}
	
	ChessPiece::makeMove(move);
	
	if (rook != 0)
	{
		int rookSquare = rook->square();
		if (rookSquare < m_square)
			rook->setSquare(target + 1); // leftside castle
		else
			rook->setSquare(target - 1); // rightside castle
	}
}
