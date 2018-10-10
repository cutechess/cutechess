#include "elo.h"
#include <cmath>

Elo::Elo(int wins, int losses, int draws)
{
	m_wins = wins;
	m_losses = losses;
	m_draws = draws;

	qreal n = wins + losses + draws;
	qreal w = wins / n;
	qreal l = losses / n;
	qreal d = draws / n;
	m_mu = w + d / 2.0;

	qreal devW = w * std::pow(1.0 - m_mu, 2.0);
	qreal devL = l * std::pow(0.0 - m_mu, 2.0);
	qreal devD = d * std::pow(0.5 - m_mu, 2.0);
	m_stdev = std::sqrt(devW + devL + devD) / std::sqrt(n);
}

qreal Elo::pointRatio() const
{
	qreal total = (m_wins + m_losses + m_draws) * 2;
	return ((m_wins * 2) + m_draws) / total;
}

qreal Elo::drawRatio() const
{
	qreal n = m_wins + m_losses + m_draws;
	return m_draws / n;
}

qreal Elo::diff() const
{
	return diff(m_mu);
}

qreal Elo::diff(qreal p)
{
	return -400.0 * std::log10(1.0 / p - 1.0);
}

qreal Elo::errorMargin() const
{
	qreal muMin = m_mu + phiInv(0.025) * m_stdev;
	qreal muMax = m_mu + phiInv(0.975) * m_stdev;
	return (diff(muMax) - diff(muMin)) / 2.0;
}

qreal Elo::erfInv(qreal x)
{
	const qreal pi = 3.1415926535897;

	qreal a = 8.0 * (pi - 3.0) / (3.0 * pi * (4.0 - pi));
	qreal y = std::log(1.0 - x * x);
	qreal z = 2.0 / (pi * a) + y / 2.0;

	qreal ret = std::sqrt(std::sqrt(z * z - y / a) - z);

	if (x < 0.0)
		return -ret;
	return ret;
}

qreal Elo::phiInv(qreal p)
{
	return std::sqrt(2.0) * erfInv(2.0 * p - 1.0);
}

qreal Elo::LOS() const
{
	return 100 * (0.5 + 0.5 * std::erf((m_wins - m_losses) / std::sqrt(2.0 * (m_wins + m_losses))));
}
