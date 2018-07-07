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

#include "tournamentpair.h"
#include <algorithm>


TournamentPair::TournamentPair(int firstPlayer,
			       int secondPlayer)
	: m_gamesStarted(0),
	  m_hasOriginalOrder(true)
{
	m_first.index = firstPlayer;
	m_first.score = 0;
	m_second.index = secondPlayer;
	m_second.score = 0;
}

bool TournamentPair::hasSamePlayers(const TournamentPair* other) const
{
	if (!other)
		return false;

	if (other->firstPlayer() == firstPlayer()
	&&  other->secondPlayer() == secondPlayer())
		return true;
	if (other->secondPlayer() == firstPlayer()
	&&  other->firstPlayer() == secondPlayer())
		return true;
	return false;
}

bool TournamentPair::isValid() const
{
	return m_first.index != -1 && m_second.index != -1;
}

bool TournamentPair::hasOriginalOrder() const
{
	return m_hasOriginalOrder;
}

int TournamentPair::firstPlayer() const
{
	return m_first.index;
}

int TournamentPair::secondPlayer() const
{
	return m_second.index;
}

int TournamentPair::leader() const
{
	if (m_first.index == -1)
		return secondPlayer();
	if (m_second.index == -1)
		return firstPlayer();

	if (m_first.score > m_second.score)
		return firstPlayer();
	else if (m_first.score < m_second.score)
		return secondPlayer();
	return -1;
}

int TournamentPair::gamesStarted() const
{
	return m_gamesStarted;
}

int TournamentPair::gamesFinished() const
{
	return scoreSum() / 2;
}

int TournamentPair::gamesInProgress() const
{
	return m_gamesStarted - gamesFinished();
}

int TournamentPair::firstScore() const
{
	return m_first.score;
}

int TournamentPair::secondScore() const
{
	return m_second.score;
}

int TournamentPair::scoreSum() const
{
	return m_first.score + m_second.score;
}

int TournamentPair::scoreDiff() const
{
	return m_first.score - m_second.score;
}

void TournamentPair::addFirstScore(int score)
{
	m_first.score += score;
}

void TournamentPair::addSecondScore(int score)
{
	m_second.score += score;
}

void TournamentPair::addStartedGame()
{
	m_gamesStarted++;
}

void TournamentPair::swapPlayers()
{
	std::swap(m_first, m_second);
	m_hasOriginalOrder = !m_hasOriginalOrder;
}
