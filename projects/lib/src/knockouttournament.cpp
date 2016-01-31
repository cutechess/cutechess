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

#include "knockouttournament.h"

KnockoutTournament::KnockoutTournament(GameManager* gameManager,
				       QObject *parent)
	: Tournament(gameManager, parent),
	  m_currentPair(-1)
{
}

QString KnockoutTournament::type() const
{
	return "knockout";
}

void KnockoutTournament::initializePairing()
{
	m_pairs.clear();
	m_currentPair = 0;
	int x = 1;
	while (x < playerCount())
		x *= 2;
	int player = 0;
	for (int i = 0; i < (x - playerCount()); i++)
		m_pairs.append(qMakePair(player++, -1));
	for (int i = player; i < playerCount(); i += 2)
	{
		m_pairs.append(qMakePair(i, i + 1));
	}
}

int KnockoutTournament::gamesPerCycle() const
{
	int x = 1;
	while (x < playerCount())
		x *= 2;
	int round = x / 2;
	int total = round - (x - playerCount());
	while (round >= 2)
	{
		round /= 2;
		total += round;
	}

	return total;
}

QPair<int, int> KnockoutTournament::nextPair()
{
	if (m_currentPair >= m_pairs.count())
	{
		QList<int> winners;
		for (int i = 0; i < m_pairs.size(); i++)
		{
			QPair<int, int> p(m_pairs.at(i));
			if (p.second == -1)
				winners.append(p.first);
			else
			{
				PlayerData data(playerAt(p.first));
				if (data.wins >= data.losses)
					winners.append(p.first);
				else
					winners.append(p.second);
			}
		}
		m_pairs.clear();
		if (winners.count() <= 1)
		{
			initializePairing();
			setCurrentRound(1);
		}
		else
		{
			for (int i = 0; i < winners.count(); i += 2)
			{
				m_pairs.append(qMakePair(winners.at(i), winners.at(i + 1)));
			}
			m_currentPair = 0;
			setCurrentRound(currentRound() + 1);
		}
	}

	QPair<int, int> pair(m_pairs.at(m_currentPair++));
	if (pair.second == -1)
		return nextPair();
	return pair;
}
