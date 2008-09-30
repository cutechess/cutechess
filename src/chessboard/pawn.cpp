#include "pawn.h"
#include "completechessmove.h"


Pawn::Pawn(Chessboard::ChessSide side, int square, Chessboard* board)
: ChessPiece(side, square, board)
{
	m_sign = (m_side == Chessboard::White) ? 1 : -1;
	int vStep = -m_sign * board->arrayWidth();
	
	m_offsets[0] = vStep;
	m_offsets[1] = vStep * 2;
	m_captureOffsets[0] = vStep - 1;
	m_captureOffsets[1] = vStep + 1;
	m_epOffsets[0] = -1;
	m_epOffsets[1] = 1;
}

ChessPiece::PieceType Pawn::type() const
{
	return PT_Pawn;
}

bool Pawn::canAttack(int square) const
{
	if (square == (m_square + m_captureOffsets[0])
	||  square == (m_square + m_captureOffsets[1]))
		return true;
	return false;
}

void Pawn::addMoves(int target, ChessPiece* capture, bool isPromotion, QList<CompleteChessMove>* moves) const
{
	if (isPromotion)
	{
		moves->append(CompleteChessMove(m_square, target, capture, PT_Knight, this));
		moves->append(CompleteChessMove(m_square, target, capture, PT_Bishop, this));
		moves->append(CompleteChessMove(m_square, target, capture, PT_Rook, this));
		moves->append(CompleteChessMove(m_square, target, capture, PT_Queen, this));
		if (m_board->variant() == Chessboard::CapablancaChess)
		{
			moves->append(CompleteChessMove(m_square, target, capture, PT_Archbishop, this));
			moves->append(CompleteChessMove(m_square, target, capture, PT_Chancellor, this));
		}
	}
	else
		moves->append(CompleteChessMove(m_square, target, capture, PT_None));
}

void Pawn::generateMoves(QList<CompleteChessMove>* moves) const
{
	int rank;
	int target;
	bool isPromotion = false;

	if (m_side == Chessboard::White)
		rank = 7 - (m_square / m_board->arrayWidth());
	else
		rank = m_square / m_board->arrayWidth();

	if (rank == 6)
		isPromotion = true;

	// one forward
	target = m_square + m_offsets[0];
	if (m_board->isValidSquare(target) && m_board->pieceAt(target) == 0)
	{
		addMoves(target, 0, isPromotion, moves);

		// two forward
		target = m_square + m_offsets[1];
		if (rank == 1 && m_board->pieceAt(target) == 0)
			moves->append(CompleteChessMove(m_square, target));
	}
	
	// captures
	for (int i = 0; i < 2; i++)
	{
		target = m_square + m_captureOffsets[i];
		if (m_board->isValidSquare(target))
		{
			ChessPiece* capture;
			if (target == m_board->enpassantSquare())
				capture = m_board->pieceAt(m_square + m_epOffsets[i]);
			else
				capture = m_board->pieceAt(target);
			
			if (capture != 0 && capture->side() != m_side)
				addMoves(target, capture, isPromotion, moves);
		}
	}
}

// make promotion moves and enpassant captures
void Pawn::makeMove(const CompleteChessMove& move)
{
	ChessPiece::makeMove(move);
	
	int source = move.sourceSquare();
	int target = move.targetSquare();
	
	if (target == source + m_offsets[1])
		m_board->setEnpassantSquare(source + m_offsets[0]);
	else if (move.promotion() != PT_None)
	{
		disable();
		
		ChessPiece* newPiece = ChessPiece::fromType(m_side, move.promotion(), target, m_board);
		Q_CHECK_PTR(newPiece);
		m_board->addPiece(newPiece);
	}
}

void Pawn::undoMove(const CompleteChessMove& move)
{
	if (move.promotedPawn() != 0)
	{
		Q_ASSERT(move.promotedPawn() == this);
		ChessPiece* promotion = m_board->pieceAt(move.targetSquare());
		Q_ASSERT(promotion != 0);
		promotion->disable();
		delete promotion;
		enable();
	}
	ChessPiece::undoMove(move);
}
