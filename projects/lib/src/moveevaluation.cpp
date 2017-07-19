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

#include "moveevaluation.h"

MoveEvaluation::MoveEvaluation()
	: m_isBookEval(false),
	  m_depth(0),
	  m_selDepth(0),
	  m_score(NULL_SCORE),
	  m_time(0),
	  m_pvNumber(0),
	  m_nodeCount(0),
	  m_nps(0),
	  m_tbHits(0)
{
}

bool MoveEvaluation::operator==(const MoveEvaluation& other) const
{
	if (m_isBookEval == other.m_isBookEval
	&&  m_depth == other.m_depth
	&&  m_selDepth == other.m_selDepth
	&&  m_score == other.m_score
	&&  m_time == other.m_time
	&&  m_pvNumber == other.m_pvNumber
	&&  m_nodeCount == other.m_nodeCount
	&&  m_nps == other.m_nps
	&&  m_tbHits == other.m_tbHits)
		return true;
	return false;
}

bool MoveEvaluation::operator!=(const MoveEvaluation& other) const
{
	if (m_isBookEval != other.m_isBookEval
	||  m_depth != other.m_depth
	||  m_selDepth != other.m_selDepth
	||  m_score != other.m_score
	||  m_time != other.m_time
	||  m_pvNumber != other.m_pvNumber
	||  m_nodeCount != other.m_nodeCount
	||  m_nps != other.m_nps
	||  m_tbHits != other.m_tbHits)
		return true;
	return false;
}

bool MoveEvaluation::isEmpty() const
{
	if (m_depth == 0
	&&  m_selDepth == 0
	&&  m_score == NULL_SCORE
	&&  m_time < 500
	&&  m_pvNumber == 0
	&&  m_nodeCount == 0
	&&  m_nps == 0
	&&  m_tbHits == 0)
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

int MoveEvaluation::selectiveDepth() const
{
	return m_selDepth;
}

int MoveEvaluation::score() const
{
	return m_score;
}

int MoveEvaluation::time() const
{
	return m_time;
}

quint64 MoveEvaluation::nodeCount() const
{
	return m_nodeCount;
}

quint64 MoveEvaluation::nps() const
{
	if (m_nps || !m_time)
		return m_nps;
	return quint64(m_nodeCount / (double(m_time) / 1000.0));
}

quint64 MoveEvaluation::tbHits() const
{
	return m_tbHits;
}

QString MoveEvaluation::pv() const
{
	return m_pv;
}

int MoveEvaluation::pvNumber() const
{
	return m_pvNumber;
}

void MoveEvaluation::clear()
{
	m_isBookEval = false;
	m_depth = 0;
	m_selDepth = 0;
	m_score = NULL_SCORE;
	m_time = 0;
	m_pvNumber = 0;
	m_nodeCount = 0;
	m_nps = 0;
	m_tbHits = 0;
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

void MoveEvaluation::setSelectiveDepth(int depth)
{
	m_selDepth = depth;
}

void MoveEvaluation::setScore(int score)
{
	m_score = score;
}

void MoveEvaluation::setTime(int time)
{
	m_time = time;
}

void MoveEvaluation::setNodeCount(quint64 nodeCount)
{
	m_nodeCount = nodeCount;
}

void MoveEvaluation::setNps(quint64 nps)
{
	m_nps = nps;
}

void MoveEvaluation::setTbHits(quint64 tbHits)
{
	m_tbHits = tbHits;
}

void MoveEvaluation::setPv(const QString& pv)
{
	m_pv = pv;
}

void MoveEvaluation::setPvNumber(int number)
{
	m_pvNumber = number;
}

void MoveEvaluation::merge(const MoveEvaluation& other)
{
	if (other.m_depth)
		m_depth = other.m_depth;
	if (other.m_selDepth)
		m_selDepth = other.m_selDepth;
	m_isBookEval = other.m_isBookEval;
	if (other.m_nodeCount)
		m_nodeCount = other.m_nodeCount;
	if (other.m_nps)
		m_nps = other.m_nps;
	if (other.m_tbHits)
		m_tbHits = other.m_tbHits;
	if (!other.m_pv.isEmpty())
		m_pv = other.m_pv;
	if (other.m_pvNumber)
		m_pvNumber = other.m_pvNumber;
	if (other.m_score != NULL_SCORE)
		m_score = other.m_score;
	if (other.m_time)
		m_time = other.m_time;
}
