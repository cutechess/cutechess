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

#ifndef SPRT_H
#define SPRT_H

/*!
 * \brief A Sequential Probability Ratio Test
 *
 * The Sprt class implements a Sequential Probability Ratio Test (SPRT) that
 * can be used as a termination criterion for stopping a match between two
 * players when the ELO difference is known to be outside of the specified
 * interval.
 *
 * \sa http://en.wikipedia.org/wiki/Sequential_probability_ratio_test
 */
class LIB_EXPORT Sprt
{
	public:
		/*! The current status of the test. */
		enum Status
		{
			Continue,	//!< Continue monitoring
			AcceptH0,	//!< Accept null hypothesis H0
			AcceptH1	//!< Accept alternative hypothesis H1
		};

		/*! The result of a chess game. */
		enum GameResult
		{
			NoResult,	//!< Game ended with no result
			Win,		//!< First player won
			Loss,		//!< First player lost
			Draw		//!< Game was drawn
		};

		/*! Creates a new uninitialized Sprt object. */
		Sprt();

		/*!
		 * Returns true if the SPRT is uninitialized; otherwise
		 * returns false.
		 */
		bool isNull() const;

		/*!
		 * Initializes the SPRT.
		 *
		 * \a elo0 is the ELO difference between player A and
		 * player B for H0 and \a elo1 for H1.
		 *
		 * \a alpha is the maximum probability for a type I error and
		 * \a beta for a type II error outside interval [elo0, elo1].
		 */
		void initialize(double elo0, double elo1,
				double alpha, double beta);
		/*! Returns the current status of the test. */
		Status status() const;
		/*!
		 * Updates the test with \a result.
		 *
		 * After calling this function, status() should be called to
		 * check if H0 or H1 can be accepted.
		 */
		void addResult(GameResult result);

	private:
		/*!
		 * \internal From BayesElo: computes probabilities of win and
		 * draw, given the elo difference. So it basically makes
		 * a realistic assumption on (pWin, pDraw) for a given
		 * elo difference, that is for a given theoretical score
		 * pWin + pDraw/2.
		 */
		static void setResultProbabilities(double eloDiff,
						   double& pWin,
						   double& pDraw,
						   double& pLoss);

		// Cumulative sum of the log-likelihood ratio
		double m_llrSum;
		// Log-likelihood ratio for wins
		double m_llrWin;
		// Log-likelihood ratio for draws
		double m_llrDraw;
		// Log-likelihood ratio for losses
		double m_llrLoss;
		// Treshold for accepting H0
		double m_lowerBound;
		// Treshold for accepting H1
		double m_upperBound;
};

#endif // SPRT_H
