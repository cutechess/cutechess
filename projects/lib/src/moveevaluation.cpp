#include "moveevaluation.h"

MoveEvaluation::MoveEvaluation()
	: m_isBookEval(false),
	  m_depth(0),
	  m_score(0),
	  m_time(0),
	  m_nodeCount(0)
{
}

bool MoveEvaluation::isEmpty() const
{
	if (m_depth == 0
	&&  m_score == 0
	&&  m_time < 500
	&&  m_nodeCount == 0)
		return true;
	return false;
}

bool MoveEvaluation::isBookEval() const
{
	return m_isBookEval;
}

int MoveEvaluation::depth() const
{
	return m_depth;
}

int MoveEvaluation::score() const
{
	return m_score;
}

int MoveEvaluation::time() const
{
	return m_time;
}

int MoveEvaluation::nodeCount() const
{
	return m_nodeCount;
}

QString MoveEvaluation::pv() const
{
	return m_pv;
}

void MoveEvaluation::clear()
{
	m_isBookEval = false;
	m_depth = 0;
	m_score = 0;
	m_time = 0;
	m_nodeCount = 0;
	m_pv.clear();
}

void MoveEvaluation::setBookEval(bool isBookEval)
{
	m_isBookEval = isBookEval;
}

void MoveEvaluation::setDepth(int depth)
{
	m_depth = depth;
}

void MoveEvaluation::setScore(int score)
{
	m_score = score;
}

void MoveEvaluation::setTime(int time)
{
	m_time = time;
}

void MoveEvaluation::setNodeCount(int nodeCount)
{
	m_nodeCount = nodeCount;
}

void MoveEvaluation::setPv(const QString& pv)
{
	m_pv = pv;
}
