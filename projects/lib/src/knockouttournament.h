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

#ifndef KNOCKOUTTOURNAMENT_H
#define KNOCKOUTTOURNAMENT_H

#include "tournament.h"


/*!
 * \brief Knockout type chess tournament.
 *
 * A single-elimination tournament where the number of rounds is
 * determined by the number of players.
 */
class LIB_EXPORT KnockoutTournament : public Tournament
{
	Q_OBJECT

	public:
		/*! Creates a new Knockout tournament. */
		explicit KnockoutTournament(GameManager* gameManager,
					    QObject *parent = nullptr);

		// Inherited from Tournament
		virtual QString type() const;
		virtual bool canSetRoundMultiplier() const;
		virtual QString results() const;

	protected:
		// Inherited from Tournament
		virtual void initializePairing();
		virtual int gamesPerCycle() const;
		virtual TournamentPair* nextPair(int gameNumber);
		virtual void addScore(int player, int score);
		virtual bool areAllGamesFinished() const;

	private:
		static int playerSeed(int rank, int bracketSize);

		QList<int> firstRoundPlayers() const;
		QList<int> lastRoundWinners() const;
		bool needMoreGames(const TournamentPair* pair) const;

		QList< QList<TournamentPair*> > m_rounds;
};

#endif // KNOCKOUTTOURNAMENT_H
