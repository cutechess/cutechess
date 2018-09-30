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

#ifndef ELO_H
#define ELO_H

#include <QtGlobal>

/*!
 * \brief Utility class for calculating Elo statistics for a single player
 *
 * The Elo class can be used to calculate Elo difference, Elo error margin,
 * etc. based on player's W/D/L statistics. At its present state it's very
 * suitable for matches between two players but not that accurate when
 * there are more than 2 players in a tournament. This is because the ratings
 * are calculated as if each player's results were against a single opponent.
 */
class LIB_EXPORT Elo
{
	public:
		/*! Creates a new Elo object. */
		Elo(int wins, int losses, int draws);

		/*! Returns the Elo difference. */
		qreal diff() const;
		/*! Returns the error margin in Elo points. */
		qreal errorMargin() const;
		/*! Returns the ratio of points won. */
		qreal pointRatio() const;
		/*! Returns the ratio of drawn games. */
		qreal drawRatio() const;
		/*! Returns the likelihood of superiority. */
		qreal LOS() const;

	private:
		int m_wins;
		int m_losses;
		int m_draws;
		qreal m_mu;
		qreal m_stdev;

		// Elo difference
		static qreal diff(qreal p);
		// Inverted error function
		static qreal erfInv(qreal x);
		// Quantile function for the standard Gaussian law:
		// probability -> quantile
		static qreal phiInv(qreal p);
};

#endif // ELO_H
