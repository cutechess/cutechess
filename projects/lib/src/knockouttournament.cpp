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
#include "playerbuilder.h"
#include "mersenne.h"


KnockoutTournament::KnockoutPlayer::KnockoutPlayer(int index, int score)
	: index(index),
	  score(score)
{
}

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

int KnockoutTournament::playerSeed(int rank, int bracketSize)
{
	if (rank <= 1)
		return 0;

	// If our rank is even we need to put the player into the right bracket
	// so we add half the bracket size to his position and make a recursive
	// call with half the rank and half the bracket size.
	if (rank % 2 == 0)
		return bracketSize / 2 + playerSeed(rank / 2, bracketSize / 2);

	// If the rank is uneven, we put the player in the left bracket.
	// Since rank is uneven we need to add + 1 so that it stays uneven.
	return playerSeed(rank / 2 + 1, bracketSize / 2);
}

QList<KnockoutTournament::KnockoutPlayer> KnockoutTournament::firstRoundPlayers() const
{
	QList<KnockoutPlayer> players;
	int n(playerCount());
	int seedCount = qMin(this->seedCount(), n);

	for (int i = 0; i < seedCount; i++)
		players << KnockoutPlayer(i);

	QList<KnockoutPlayer> unseeded;
	for (int i = seedCount; i < n; i++)
	{
		unseeded << KnockoutPlayer(i);
	}

	while (!unseeded.isEmpty())
	{
		int i = Mersenne::random() % unseeded.size();
		players << unseeded.takeAt(i);
	}

	return players;
}

void KnockoutTournament::initializePairing()
{
	m_playerScore.clear();
	m_currentPair = 0;
	int x = 1;
	while (x < playerCount())
		x *= 2;
	QVector<KnockoutPlayer> all = QVector<KnockoutPlayer>(x);

	QList<KnockoutPlayer> players(firstRoundPlayers());
	int byeCount = x - players.size();
	for (int i = 0; i < byeCount; i++)
		players << KnockoutPlayer();

	int byes = 0;
	for (int i = 0; i < players.size(); i++)
	{
		int index = playerSeed(i + 1, x);
		KnockoutPlayer player(players.at(i));

		// Pair BYEs with top-seeded players
		if (player.index == -1)
		{
			int byeIndex = playerSeed(++byes, x) + 1;
			all[index] = all.at(byeIndex);
			all[byeIndex] = player;
		}
		else
			all[index] = player;
	}

	QList<Pair> pairs;
	for (int i = 0; i < x; i += 2)
	{
		KnockoutPlayer first(all.at(i));
		KnockoutPlayer second(all.at(i + 1));
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

void KnockoutTournament::addScore(int player, int score)
{
	QList<Pair>& lastRound(m_rounds.last());
	QList<Pair>::iterator it;
	for (it = lastRound.begin(); it != lastRound.end(); ++it)
	{
		if (it->first.index == player)
		{
			it->first.score += score;
			break;
		}
		if (it->second.index == player)
		{
			it->second.score += score;
			break;
		}
	}

	Tournament::addScore(player, score);
}

QList<KnockoutTournament::KnockoutPlayer> KnockoutTournament::lastRoundWinners() const
{
	QList<KnockoutPlayer> winners;
	const QList<Pair>& lastRound(m_rounds.last());

	QList<Pair>::const_iterator it;
	for (it = lastRound.constBegin(); it != lastRound.constEnd(); ++it)
	{
		KnockoutPlayer winner;

		if (it->second.index == -1)
			winner.index = it->first.index;
		else
		{
			if (it->first.score >= it->second.score)
				winner.index = it->first.index;
			else
				winner.index = it->second.index;
		}
		winners << winner;
	}

	return winners;
}

QPair<int, int> KnockoutTournament::nextPair()
{
	QList<Pair>& lastRound(m_rounds.last());

	// Start new round
	if (m_currentPair >= lastRound.size())
	{
		// All of the previous round's games must finish before a new
		// round can begin.
		if (gamesInProgress() > 0)
			return qMakePair(-1, -1);

		QList<KnockoutPlayer> winners(lastRoundWinners());
		Q_ASSERT(winners.count() > 1);

		QList<Pair> nextRound;
		for (int i = 0; i < winners.count(); i += 2)
		{
			nextRound << qMakePair(winners.at(i), winners.at(i + 1));
		}
		m_rounds << nextRound;
		m_currentPair = 0;
		setCurrentRound(currentRound() + 1);
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
			int lineNum = ((2 << (r - 1)) - 1) +
				      (x * (2 << r));
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
