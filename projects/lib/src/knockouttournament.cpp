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
#include <cmath>
#include "playerbuilder.h"

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

bool KnockoutTournament::canSetRoundMultiplier() const
{
	return false;
}

void KnockoutTournament::initializePairing()
{
	QList<Pair> pairs;

	m_playerScore.clear();
	m_currentPair = 0;
	int x = 1;
	while (x < playerCount())
		x *= 2;
	int player = 0;
	for (int i = 0; i < (x - playerCount()); i++)
	{
		KnockoutPlayer first =
		{
			player++,
			0
		};
		KnockoutPlayer second =
		{
			-1,
			0
		};
		pairs.append(qMakePair(first, second));
	}
	for (int i = player; i < playerCount(); i += 2)
	{
		KnockoutPlayer first =
		{
			i,
			0
		};
		KnockoutPlayer second =
		{
			i + 1,
			0
		};
		pairs.append(qMakePair(first, second));
	}

	m_rounds.clear();
	m_rounds << pairs;
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

QList<KnockoutTournament::KnockoutPlayer> KnockoutTournament::lastRoundWinners()
{
	QList<KnockoutPlayer> winners;
	QList<Pair>& lastRound(m_rounds.last());

	QList<Pair>::iterator it;
	for (it = lastRound.begin(); it != lastRound.end(); ++it)
	{
		if (it->second.index == -1)
			winners.append(it->first);
		else
		{
			int i1 = it->first.index;
			int i2 = it->second.index;
			PlayerData p1(playerAt(i1));
			int s1 = p1.wins * 2 + p1.draws;
			PlayerData p2(playerAt(i2));
			int s2 = p2.wins * 2 + p2.draws;

			it->first.score = s1 - m_playerScore[i1];
			it->second.score = s2 - m_playerScore[i2];
			m_playerScore[i1] = s1;
			m_playerScore[i2] = s2;

			if (it->first.score >= it->second.score)
				winners.append(it->first);
			else
				winners.append(it->second);
		}
	}

	return winners;
}

QPair<int, int> KnockoutTournament::nextPair()
{
	QList<Pair>& lastRound(m_rounds.last());

	// Start new round
	if (m_currentPair >= lastRound.size())
	{
		QList<KnockoutPlayer> winners(lastRoundWinners());
		if (winners.count() <= 1)
		{
			initializePairing();
			setCurrentRound(1);
		}
		else
		{
			QList<Pair> nextRound;
			for (int i = 0; i < winners.count(); i += 2)
			{
				nextRound << qMakePair(winners.at(i), winners.at(i + 1));
			}
			m_rounds << nextRound;
			m_currentPair = 0;
			setCurrentRound(currentRound() + 1);
		}
	}

	const Pair& pair(m_rounds.last().at(m_currentPair++));
	if (pair.second.index == -1)
		return nextPair();
	return qMakePair(pair.first.index, pair.second.index);
}

void KnockoutTournament::onFinished()
{
	lastRoundWinners();
	Tournament::onFinished();
}

QString KnockoutTournament::results() const
{
	QStringList lines;

	foreach (const Pair& pair, m_rounds.first())
	{
		lines << playerAt(pair.first.index).builder->name() << "";
		if (pair.second.index == -1)
			lines << "bye";
		else
			lines << playerAt(pair.second.index).builder->name();
		lines << "";
	}
	lines.removeLast();

	for (int round = 0; round < currentRound(); round++)
	{
		int x = 0;
		foreach (const Pair& pair, m_rounds.at(round))
		{
			int winner;
			QString score;
			if (pair.first.score >= pair.second.score)
			{
				winner = pair.first.index;
				score = QString("%1-%2")
					.arg(pair.first.score)
					.arg(pair.second.score);
			}
			else
			{
				winner = pair.second.index;
				score = QString("%1-%2")
					.arg(pair.second.score)
					.arg(pair.first.score);
			}
			int r = round + 1;
			int lineNum = (std::pow(2, r) - 1) +
				      (x * std::pow(2, r + 1));
			QString text = QString(r * 2, '\t');
			text += playerAt(winner).builder->name();
			if (score != "0-0")
				text += QString(" (%1)").arg(score);
			lines[lineNum] += text;
			x++;
		}
	}

	return lines.join('\n');
}
