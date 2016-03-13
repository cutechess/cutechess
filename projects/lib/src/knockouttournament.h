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

#ifndef KNOCKOUTTOURNAMENT_H
#define KNOCKOUTTOURNAMENT_H

#include "tournament.h"


/*!
 * \brief Knockout type chess tournament.
 *
 * A single-elimination tournament where the number of rounds is
 * determined by the number of players.
 *
 * TODO:
 * - In a drawn encounter the first player advances, which is not
 * fair. Setting a high enough gamesPerEncounter() value mitigates
 * this problem, but not completely.
 * - Code cleanup.
 * - Even though scores are now always up-to-date, they shouldn't show a
 * winner for a match when it's still in progress.
 */
class LIB_EXPORT KnockoutTournament : public Tournament
{
	Q_OBJECT

	public:
		/*! Creates a new Knockout tournament. */
		explicit KnockoutTournament(GameManager* gameManager,
					    QObject *parent = 0);
		// Inherited from Tournament
		virtual QString type() const;
		virtual bool canSetRoundMultiplier() const;
		virtual QString results() const;

	protected:
		// Inherited from Tournament
		virtual void initializePairing();
		virtual int gamesPerCycle() const;
		virtual QPair<int, int> nextPair();
		virtual void onFinished();
		virtual void addScore(int player, int score);

	private:
		class KnockoutPlayer
		{
			public:
				KnockoutPlayer(int index = -1, int score = 0);
				int index;
				int score;
		};
		typedef QPair<KnockoutPlayer, KnockoutPlayer> Pair;

		static int playerSeed(int rank, int bracketSize);
		QList<KnockoutPlayer> firstRoundPlayers() const;
		QList<KnockoutPlayer> lastRoundWinners() const;

		QList< QList<Pair> > m_rounds;
		QMap<int, int> m_playerScore;
		int m_currentPair;
};

#endif // KNOCKOUTTOURNAMENT_H
