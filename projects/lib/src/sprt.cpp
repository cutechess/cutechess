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

#include "sprt.h"
#include <cmath>

Sprt::Sprt()
	: m_llrSum(0.0),
	  m_llrWin(0.0),
	  m_llrDraw(0.0),
	  m_llrLoss(0.0),
	  m_lowerBound(0.0),
	  m_upperBound(0.0)
{
}

void Sprt::initialize(double elo0, double elo1,
		      double alpha, double beta)
{
	m_llrSum = 0.0;

	double pWin0, pDraw0, pLoss0;
	double pWin1, pDraw1, pLoss1;

	setResultProbabilities(elo0, pWin0, pDraw0, pLoss0);
	setResultProbabilities(elo1, pWin1, pDraw1, pLoss1);

	m_llrWin = std::log(pWin1 / pWin0);
	m_llrDraw = std::log(pDraw1 / pDraw0);
	m_llrLoss = std::log(pLoss1 / pLoss0);

	m_lowerBound = std::log(beta / (1.0 - alpha));
	m_upperBound = std::log((1.0 - beta) / alpha);
}

void Sprt::setResultProbabilities(double eloDiff,
				  double& pWin,
				  double& pDraw,
				  double& pLoss)
{
	const double drawElo = 97.3; // From BayesElo

	pWin  = 1.0 / (1.0 + std::pow(10.0, (-eloDiff + drawElo) / 400.0));
	pLoss = 1.0 / (1.0 + std::pow(10.0, (eloDiff + drawElo) / 400.0));
	pDraw = 1.0 - pWin - pLoss;
}

Sprt::Status Sprt::status() const
{
	if (m_llrSum > m_upperBound)
		return AcceptH1;
	else if (m_llrSum < m_lowerBound)
		return AcceptH0;
	return Continue;
}

void Sprt::addResult(GameResult result)
{
	if (result == Win)
		m_llrSum += m_llrWin;
	else if (result == Draw)
		m_llrSum += m_llrDraw;
	else if (result == Loss)
		m_llrSum += m_llrLoss;
}
