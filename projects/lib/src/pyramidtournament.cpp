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


#include "pyramidtournament.h"
#include <algorithm>

PyramidTournament::PyramidTournament(GameManager* gameManager,
					   QObject *parent)
	: Tournament(gameManager, parent),
	  m_pairNumber(0),
	  m_currentPlayer(1)
{
}

QString PyramidTournament::type() const
{
	return "pyramid";
}

void PyramidTournament::initializePairing()
{
	m_pairNumber = 0;
	m_currentPlayer = 1;
	setCurrentRound(1);
}

int PyramidTournament::gamesPerCycle() const
{
	return (playerCount() * (playerCount() - 1)) / 2;
}

TournamentPair* PyramidTournament::nextPair(int gameNumber)
{
	if (gameNumber >= finalGameCount())
		return nullptr;
	if (gameNumber % gamesPerEncounter() != 0)
		return currentPair();

	if (m_pairNumber >= m_currentPlayer)
	{
		m_pairNumber = 0;
		setCurrentRound(currentRound() + 1);
		if (++m_currentPlayer >= playerCount())
			m_currentPlayer = 1;
	}

	int white = m_currentPlayer;
	int black = m_pairNumber++;

	return pair(white, black);
}
