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


#include "roundrobintournament.h"
#include <algorithm>

RoundRobinTournament::RoundRobinTournament(GameManager* gameManager,
					   QObject *parent)
	: Tournament(gameManager, parent),
	  m_pairNumber(0)
{
}

QString RoundRobinTournament::type() const
{
	return "round-robin";
}

void RoundRobinTournament::initializePairing()
{
	m_pairNumber = 0;
	m_topHalf.clear();
	m_bottomHalf.clear();
	int count = playerCount() + (playerCount() % 2);

	for (int i = 0; i < count / 2; i++)
		m_topHalf.append(i);
	for (int i = count - 1; i >= count / 2; i--)
		m_bottomHalf.append(i);
}

int RoundRobinTournament::gamesPerCycle() const
{
	return (playerCount() * (playerCount() - 1)) / 2;
}

TournamentPair* RoundRobinTournament::nextPair(int gameNumber)
{
	if (gameNumber >= finalGameCount())
		return nullptr;
	if (gameNumber % gamesPerEncounter() != 0)
		return currentPair();

	if (m_pairNumber >= m_topHalf.size())
	{
		m_pairNumber = 0;
		setCurrentRound(currentRound() + 1);
		m_topHalf.insert(1, m_bottomHalf.takeFirst());
		m_bottomHalf.append(m_topHalf.takeLast());
	}

	int white = m_topHalf.at(m_pairNumber);
	int black = m_bottomHalf.at(m_pairNumber);

	m_pairNumber++;

	// If 'white' or 'black' equals 'playerCount()' it means
	// that it's a "bye" player, that is an empty player that
	// makes the pairings easier to organize. In that case
	// no game is played and we skip to the next pair.
	if (white < playerCount() && black < playerCount())
		return pair(white, black);
	else
		return nextPair(gameNumber);
}
