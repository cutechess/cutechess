/*
    This file is part of Cute Chess.
    Copyright (C) 2008-2018 Cute Chess authors

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
	  m_isTrusted(false),
	  m_depth(0),
	  m_selDepth(0),
	  m_score(NULL_SCORE),
	  m_time(0),
	  m_pvNumber(0),
	  m_hashUsage(0),
	  m_ponderhitRate(0),
	  m_nodeCount(0),
	  m_nps(0),
	  m_tbHits(0)
{
}

bool MoveEvaluation::operator==(const MoveEvaluation& other) const
{
	if (m_isBookEval == other.m_isBookEval
	&&  m_isTrusted == other.m_isTrusted
	&&  m_depth == other.m_depth
	&&  m_selDepth == other.m_selDepth
	&&  m_score == other.m_score
	&&  m_time == other.m_time
	&&  m_pvNumber == other.m_pvNumber
	&&  m_hashUsage == other.m_hashUsage
	&&  m_ponderhitRate == other.m_ponderhitRate
	&&  m_nodeCount == other.m_nodeCount
	&&  m_nps == other.m_nps
	&&  m_tbHits == other.m_tbHits
	&&  m_ponderMove == other.m_ponderMove)
		return true;
	return false;
}

bool MoveEvaluation::operator!=(const MoveEvaluation& other) const
{
	if (m_isBookEval != other.m_isBookEval
	||  m_isTrusted != other.m_isTrusted
	||  m_depth != other.m_depth
	||  m_selDepth != other.m_selDepth
	||  m_score != other.m_score
	||  m_time != other.m_time
	||  m_pvNumber != other.m_pvNumber
	||  m_hashUsage != other.m_hashUsage
	||  m_ponderhitRate != other.m_ponderhitRate
	||  m_nodeCount != other.m_nodeCount
	||  m_nps != other.m_nps
	||  m_tbHits != other.m_tbHits
	||  m_ponderMove != other.m_ponderMove)
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
	&&  m_hashUsage == 0
	&&  m_ponderhitRate == 0
	&&  m_nodeCount == 0
	&&  m_nps == 0
	&&  m_tbHits == 0
	&&  m_ponderMove.isEmpty())
		return true;
	return false;
}

bool MoveEvaluation::isBookEval() const
{
	return m_isBookEval;
}

bool MoveEvaluation::isTrusted() const
{
	return m_isTrusted;
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

QString MoveEvaluation::scoreText() const
{
	if (isBookEval())
		return "book";
	if (m_score == NULL_SCORE)
		return QString();

	QString str;
	if (depth() > 0)
	{
		int absScore = qAbs(m_score);
		if (m_score > 0)
			str += "+";

		// Detect mate-in-n scores
		if (absScore > MATE_SCORE - 200
		&&  (absScore = 1000 - (absScore % 1000)) < 200)
		{
			if (m_score < 0)
				str += "-";
			str += "M" + QString::number(absScore);
		}
		else
			str += QString::number(double(m_score) / 100.0, 'f', 2);
	}

	return str;
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

int MoveEvaluation::hashUsage() const
{
	return m_hashUsage;
}

int MoveEvaluation::ponderhitRate() const
{
	return m_ponderhitRate;
}

QString MoveEvaluation::ponderMove() const
{
	return m_ponderMove;
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
	m_hashUsage = 0;
	m_ponderhitRate = 0;
	m_pv.clear();
	m_ponderMove.clear();
}

void MoveEvaluation::setBookEval(bool isBookEval)
{
	m_isBookEval = isBookEval;
}

void MoveEvaluation::setIsTrusted(bool isTrusted)
{
	m_isTrusted = isTrusted;
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

void MoveEvaluation::setHashUsage(int hashUsage)
{
	m_hashUsage = hashUsage;
}

void MoveEvaluation::setPonderhitRate(int rate)
{
	m_ponderhitRate = rate;
}

void MoveEvaluation::setPonderMove(const QString& san)
{
	m_ponderMove = san;
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
	if (other.m_hashUsage)
		m_hashUsage = other.m_hashUsage;
	if (other.m_ponderhitRate)
		m_ponderhitRate = other.m_ponderhitRate;
	if (!other.m_ponderMove.isEmpty())
		m_ponderMove = other.m_ponderMove;
	if (!other.m_pv.isEmpty())
		m_pv = other.m_pv;
	if (other.m_pvNumber)
		m_pvNumber = other.m_pvNumber;
	if (other.m_score != NULL_SCORE)
		m_score = other.m_score;
	if (other.m_time)
		m_time = other.m_time;
}
