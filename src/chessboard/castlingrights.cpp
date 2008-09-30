#include "castlingrights.h"

CastlingRights::CastlingRights()
{
	m_canCastle[LeftSide] = false;
	m_canCastle[RightSide] = false;
	m_rook[LeftSide] = 0;
	m_rook[RightSide] = 0;
	m_kingTarget[LeftSide] = -1;
	m_kingTarget[RightSide] = -1;
}

bool CastlingRights::canCastle(int side) const
{
	return m_canCastle[side];
}

void CastlingRights::enable(int side)
{
	m_canCastle[side] = true;
}

void CastlingRights::disable(int side)
{
	m_canCastle[side] = false;
}

ChessPiece* CastlingRights::rook(int side) const
{
	return m_rook[side];
}

void CastlingRights::setRook(ChessPiece* rook, int side)
{
	m_rook[side] = rook;
}

int CastlingRights::kingTarget(int side) const
{
	return m_kingTarget[side];
}

void CastlingRights::setKingTarget(int square, int side)
{
	m_kingTarget[side] = square;
}
