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


#ifndef ROUNDROBINTOURNAMENT_H
#define ROUNDROBINTOURNAMENT_H

#include "tournament.h"

/*!
 * \brief Round-robin type chess tournament.
 *
 * In a Round-robin tournament each player meets all
 * other contestants in turn.
 */
class LIB_EXPORT RoundRobinTournament : public Tournament
{
	Q_OBJECT

	public:
		/*! Creates a new Round-robin tournament. */
		explicit RoundRobinTournament(GameManager* gameManager,
					      QObject *parent = nullptr);
		// Inherited from Tournament
		virtual QString type() const;

	protected:
		// Inherited from Tournament
		virtual void initializePairing();
		virtual int gamesPerCycle() const;
		virtual TournamentPair* nextPair(int gameNumber);

	private:
		int m_pairNumber;
		QList<int> m_topHalf;
		QList<int> m_bottomHalf;
};

#endif // ROUNDROBINTOURNAMENT_H
