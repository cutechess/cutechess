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
#include <QStringList>
#include "playerbuilder.h"
#include "mersenne.h"


KnockoutTournament::KnockoutTournament(GameManager* gameManager,
				       QObject *parent)
	: Tournament(gameManager, parent)
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

QList<int> KnockoutTournament::firstRoundPlayers() const
{
	QList<int> players;
	int n(playerCount());
	int seedCount = qMin(this->seedCount(), n);

	for (int i = 0; i < seedCount; i++)
		players << i;

	QList<int> unseeded;
	for (int i = seedCount; i < n; i++)
	{
		unseeded << i;
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
	int x = 1;
	while (x < playerCount())
		x *= 2;
	QVector<int> all = QVector<int>(x, -1);

	QList<int> players(firstRoundPlayers());
	int byeCount = x - players.size();
	for (int i = 0; i < byeCount; i++)
		players << -1;

	int byes = 0;
	for (int i = 0; i < players.size(); i++)
	{
		int index = playerSeed(i + 1, x);
		int player(players.at(i));

		// Pair BYEs with top-seeded players
		if (player == -1)
		{
			int byeIndex = playerSeed(++byes, x) + 1;
			all[index] = all.at(byeIndex);
			all[byeIndex] = player;
		}
		else
			all[index] = player;
	}

	QList<TournamentPair> pairs;
	for (int i = 0; i < x; i += 2)
		pairs.append(TournamentPair(all.at(i), all.at(i + 1)));

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
	QList<TournamentPair>& lastRound(m_rounds.last());
	QList<TournamentPair>::iterator it;
	for (it = lastRound.begin(); it != lastRound.end(); ++it)
	{
		if (it->firstPlayer() == player)
		{
			it->addFirstScore(score);
			break;
		}
		if (it->secondPlayer() == player)
		{
			it->addSecondScore(score);
			break;
		}
	}

	Tournament::addScore(player, score);
}

QList<int> KnockoutTournament::lastRoundWinners() const
{
	QList<int> winners;
	const QList<TournamentPair>& lastRound(m_rounds.last());

	QList<TournamentPair>::const_iterator it;
	for (it = lastRound.constBegin(); it != lastRound.constEnd(); ++it)
		winners << it->leader();

	return winners;
}

bool KnockoutTournament::areAllGamesFinished() const
{
	const QList<TournamentPair>& lastRound(m_rounds.last());
	if (lastRound.size() > 1)
		return false;

	const TournamentPair& pair(lastRound.first());
	return !pair.gamesInProgress() && !needMoreGames(pair);
}

bool KnockoutTournament::needMoreGames(const TournamentPair& pair) const
{
	// Second player is a BYE
	if (!pair.isValid())
		return false;

	// Assign the leading player all points from ongoing games.
	// If the leader still doesn't have enough points to win the
	// encounter, return true
	int leadScore = qMax(pair.firstScore(), pair.secondScore());
	int pointsInProgress = pair.gamesInProgress() * 2;
	leadScore += pointsInProgress;
	if (leadScore <= gamesPerEncounter())
		return true;

	// Make sure the score diff is big enough
	int minDiff = 1;
	if (gamesPerEncounter() % 2 == 0)
		minDiff = 3;

	int maxDiff = qAbs(pair.scoreDiff()) + pointsInProgress;
	if (maxDiff >= minDiff)
		return false;

	if (minDiff == 1 || maxDiff == 0)
		return true;

	// If the encounter was extended, make sure there's an even
	// number of games if gamesPerEncounter() is even
	return pair.gamesStarted() % 2 != 0;
}

void KnockoutTournament::startGame(const TournamentPair& pair)
{
	// TODO: This could be a lot easier if we used pointers
	// and had only one instance of each pair.
	QList<TournamentPair>& lastRound(m_rounds.last());
	QList<TournamentPair>::iterator it;
	for (it = lastRound.begin(); it != lastRound.end(); ++it)
	{
		if (it->hasSamePlayers(pair))
		{
			it->addStartedGame();
			break;
		}
	}

	Tournament::startGame(pair);
}

TournamentPair KnockoutTournament::nextPair(int gameNumber)
{
	Q_UNUSED(gameNumber);

	QList<TournamentPair>& lastRound(m_rounds.last());
	foreach (const TournamentPair& pair, lastRound)
	{
		if (needMoreGames(pair))
			return pair;
	}

	QList<int> winners(lastRoundWinners());
	if (winners.size() <= 1 || gamesInProgress())
		return TournamentPair();

	QList<TournamentPair> nextRound;
	for (int i = 0; i < winners.size(); i += 2)
	{
		nextRound << TournamentPair(winners.at(i),
					    winners.at(i + 1));
	}
	m_rounds << nextRound;
	setCurrentRound(currentRound() + 1);

	foreach (const TournamentPair& pair, nextRound)
	{
		if (pair.isValid())
			return pair;
	}

	Q_UNREACHABLE();
	return TournamentPair();
}

QString KnockoutTournament::results() const
{
	QStringList lines;

	foreach (const TournamentPair& pair, m_rounds.first())
	{
		lines << playerAt(pair.firstPlayer()).name() << "";
		if (!pair.isValid())
			lines << "bye";
		else
			lines << playerAt(pair.secondPlayer()).name();
		lines << "";
	}
	lines.removeLast();

	for (int round = 0; round < currentRound(); round++)
	{
		int x = 0;
		foreach (const TournamentPair& pair, m_rounds.at(round))
		{
			int winner = pair.leader();
			QString score;
			if (winner == pair.firstPlayer())
			{
				score = QString("%1-%2")
					.arg(pair.firstScore())
					.arg(pair.secondScore());
			}
			else
			{
				score = QString("%1-%2")
					.arg(pair.secondScore())
					.arg(pair.firstScore());
			}
			int r = round + 1;
			int lineNum = ((2 << (r - 1)) - 1) +
				      (x * (2 << r));
			QString text = QString(r * 2, '\t');
			text += playerAt(winner).name();
			if (score != "0-0")
				text += QString(" (%1)").arg(score);
			lines[lineNum] += text;
			x++;
		}
	}

	return lines.join('\n');
}
