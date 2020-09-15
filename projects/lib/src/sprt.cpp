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

#include "sprt.h"
#include <cmath>
#include <QtGlobal>

class BayesElo;
class SprtProbability;

class BayesElo
{
	public:
		BayesElo(double bayesElo, double drawElo);
		BayesElo(const SprtProbability& p);

		double bayesElo() const;
		double drawElo() const;
		double scale() const;

	private:
		double m_bayesElo;
		double m_drawElo;
};

class SprtProbability
{
	public:
		SprtProbability(double wins, double losses, double draws);
		SprtProbability(const BayesElo& b);

		bool isValid() const;
		double pWin() const;
		double pLoss() const;
		double pDraw() const;

	private:
		double m_pWin;
		double m_pLoss;
		double m_pDraw;
};


BayesElo::BayesElo(double bayesElo, double drawElo)
	: m_bayesElo(bayesElo),
	  m_drawElo(drawElo)
{
}

BayesElo::BayesElo(const SprtProbability& p)
{
	Q_ASSERT(p.isValid());

	m_bayesElo = 200.0 * std::log10(p.pWin() / p.pLoss() *
					(1.0 - p.pLoss()) / (1.0 - p.pWin()));
	m_drawElo  = 200.0 * std::log10((1.0 - p.pLoss()) / p.pLoss() *
					(1.0 - p.pWin()) / p.pWin());
}

double BayesElo::bayesElo() const
{
	return m_bayesElo;
}

double BayesElo::drawElo() const
{
	return m_drawElo;
}

double BayesElo::scale() const
{
	const double x = std::pow(10.0, -m_drawElo / 400.0);
	return 4.0 * x / ((1.0 + x) * (1.0 + x));
}


SprtProbability::SprtProbability(double wins, double losses, double draws)
{
	Q_ASSERT(wins > 0 && losses > 0 && draws > 0);

	const double count = wins + losses + draws;

	m_pWin = wins / count;
	m_pLoss = losses / count;
	m_pDraw = 1.0 - m_pWin - m_pLoss;
}

SprtProbability::SprtProbability(const BayesElo& b)
{
	m_pWin  = 1.0 / (1.0 + std::pow(10.0, (b.drawElo() - b.bayesElo()) / 400.0));
	m_pLoss = 1.0 / (1.0 + std::pow(10.0, (b.drawElo() + b.bayesElo()) / 400.0));
	m_pDraw = 1.0 - m_pWin - m_pLoss;
}

bool SprtProbability::isValid() const
{
	return 0.0 < m_pWin && m_pWin < 1.0 &&
	       0.0 < m_pLoss && m_pLoss < 1.0 &&
	       0.0 < m_pDraw && m_pDraw < 1.0;
}

double SprtProbability::pWin() const
{
	return m_pWin;
}

double SprtProbability::pLoss() const
{
	return m_pLoss;
}

double SprtProbability::pDraw() const
{
	return m_pDraw;
}


Sprt::Sprt()
	: m_elo0(0),
	  m_elo1(0),
	  m_alpha(0),
	  m_beta(0),
	  m_wins(0),
	  m_losses(0),
	  m_draws(0)
{
}

bool Sprt::isNull() const
{
	return m_elo0 == 0 && m_elo1 == 0 && m_alpha == 0 && m_beta == 0;
}

void Sprt::initialize(double elo0, double elo1,
		      double alpha, double beta)
{
	m_elo0 = elo0;
	m_elo1 = elo1;
	m_alpha = alpha;
	m_beta = beta;
}

Sprt::Status Sprt::status() const
{
	Status status = {
		Continue,
		0.0,
		0.0,
		0.0
	};

	if (m_wins < 0 || m_losses < 0 || m_draws < 0)
		return status;

	// We add half a point to each class to regularize the model parameters.
	// This corresponds to a Dirichlet(0.5, 0.5, 0.5) prior distribution,
	// which is a Jeffreys' prior for the trinomial distribution:
	const double wins = m_wins + 0.5;
	const double losses = m_losses + 0.5;
	const double draws = m_draws + 0.5;

	// Estimate draw_elo out of sample
	const SprtProbability p(wins, losses, draws);
	const BayesElo b(p);

	// Probability laws under H0 and H1
	const double s = b.scale();
	const BayesElo b0(m_elo0 / s, b.drawElo());
	const BayesElo b1(m_elo1 / s, b.drawElo());
	const SprtProbability p0(b0), p1(b1);

	// Log-Likelyhood Ratio
	status.llr = wins * std::log(p1.pWin() / p0.pWin()) +
		     losses * std::log(p1.pLoss() / p0.pLoss()) +
		     draws * std::log(p1.pDraw() / p0.pDraw());

	// Bounds based on error levels of the test
	status.lBound = std::log(m_beta / (1.0 - m_alpha));
	status.uBound = std::log((1.0 - m_beta) / m_alpha);

	if (status.llr > status.uBound)
		status.result = AcceptH1;
	else if (status.llr < status.lBound)
		status.result = AcceptH0;

	return status;
}

void Sprt::addGameResult(GameResult result)
{
	if (result == Win)
		m_wins++;
	else if (result == Draw)
		m_draws++;
	else if (result == Loss)
		m_losses++;
}
