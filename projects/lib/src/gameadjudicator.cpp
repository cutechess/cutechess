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

#include "gameadjudicator.h"
#include "board/board.h"
#include "moveevaluation.h"

GameAdjudicator::GameAdjudicator()
	: m_drawMoveNum(0),
	  m_drawMoveCount(0),
	  m_drawScore(0),
	  m_drawScoreCount(0),
	  m_resignMoveCount(0),
	  m_resignScore(0),
	  m_maxGameLength(0),
	  m_tbEnabled(false)
{
	m_resignScoreCount[0] = 0;
	m_resignScoreCount[1] = 0;
}

void GameAdjudicator::setDrawThreshold(int moveNumber, int moveCount, int score)
{
	Q_ASSERT(moveNumber >= 0);
	Q_ASSERT(moveCount >= 0);

	m_drawMoveNum = moveNumber;
	m_drawMoveCount = moveCount;
	m_drawScore = score;
	m_drawScoreCount = 0;
}

void GameAdjudicator::setResignThreshold(int moveCount, int score)
{
	Q_ASSERT(moveCount >= 0);

	m_resignMoveCount = moveCount;
	m_resignScore = score;
	m_resignScoreCount[0] = 0;
	m_resignScoreCount[1] = 0;
}

void GameAdjudicator::setMaximumGameLength(int moveCount)
{
	Q_ASSERT(moveCount >= 0);
	m_maxGameLength = moveCount;
}

void GameAdjudicator::setTablebaseAdjudication(bool enable)
{
	m_tbEnabled = enable;
}

void GameAdjudicator::addEval(const Chess::Board* board, const MoveEvaluation& eval)
{
	Chess::Side side = board->sideToMove().opposite();

	// Tablebase adjudication
	if (m_tbEnabled)
	{
		m_result = board->tablebaseResult();
		if (!m_result.isNone())
			return;
	}

	// Moves forced by the user (eg. from opening book or played by user)
	if (eval.depth() <= 0)
	{
		m_drawScoreCount = 0;
		m_resignScoreCount[side] = 0;
		return;
	}

	// Draw adjudication
	if (m_drawMoveNum > 0)
	{
		if (qAbs(eval.score()) <= m_drawScore)
			m_drawScoreCount++;
		else
			m_drawScoreCount = 0;

		if (board->plyCount() / 2 >= m_drawMoveNum
		&&  m_drawScoreCount >= m_drawMoveCount * 2)
		{
			m_result = Chess::Result(Chess::Result::Adjudication, Chess::Side::NoSide);
			return;
		}
	}

	// Resign adjudication
	if (m_resignMoveCount > 0)
	{
		int& count = m_resignScoreCount[side];
		if (qAbs(eval.score()) >= m_resignScore)
			count++;
		else
			count = 0;

		if (count >= m_resignMoveCount)
			m_result = Chess::Result(Chess::Result::Adjudication,
						 whiteEvalPov() ?
						 	(eval.score() > 0 ? Chess::Side::White : Chess::Side::Black) :
						 	side.opposite());
	}

	// Limit game length
	if (m_maxGameLength > 0
	&&  board->plyCount() >= 2 * m_maxGameLength)
	{
		m_result = Chess::Result(Chess::Result::Adjudication, Chess::Side::NoSide);
		return;
	}
}

void GameAdjudicator::resetDrawMoveCount()
{
	m_drawScoreCount = 0;
}

Chess::Result GameAdjudicator::result() const
{
	return m_result;
}
