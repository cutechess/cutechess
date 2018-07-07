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


#include "gauntlettournament.h"
#include <algorithm>
#include "chessgame.h"

GauntletTournament::GauntletTournament(GameManager* gameManager,
				       QObject *parent)
	: Tournament(gameManager, parent),
	  m_opponent(-1)
{
}

QString GauntletTournament::type() const
{
	return "gauntlet";
}

void GauntletTournament::onGameAboutToStart(ChessGame* game,
					    const PlayerBuilder* white,
					    const PlayerBuilder* black)
{
	Q_UNUSED(black);
	const int blackIndex = playerIndex(game, Chess::Side::Black);
	if (!white->isHuman() && blackIndex == 0)
		game->setBoardShouldBeFlipped(true);
}

void GauntletTournament::initializePairing()
{
	m_opponent = 1;
}

int GauntletTournament::gamesPerCycle() const
{
	return playerCount() - 1;
}

TournamentPair* GauntletTournament::nextPair(int gameNumber)
{
	if (gameNumber >= finalGameCount())
		return nullptr;
	if (gameNumber % gamesPerEncounter() != 0)
		return currentPair();

	if (m_opponent >= playerCount())
	{
		m_opponent = 1;
		setCurrentRound(currentRound() + 1);
	}

	int white = 0;
	int black = m_opponent++;

	return pair(white, black);
}

bool GauntletTournament::hasGauntletRatingsOrder() const
{
	return true;
}
