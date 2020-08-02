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

#include "tournamentplayer.h"


TournamentPlayer::TournamentPlayer(PlayerBuilder* builder,
				   const TimeControl& timeControl,
				   const OpeningBook* book,
				   int bookDepth)
	: m_builder(builder),
	  m_timeControl(timeControl),
	  m_book(book),
	  m_bookDepth(bookDepth),
	  m_wins(0),
	  m_draws(0),
	  m_losses(0),
	  m_whiteWins(0),
	  m_whiteDraws(0),
	  m_whiteLosses(0),
	  m_terminations(24),
	  m_outcome()
{
	Q_ASSERT(builder != nullptr);
}

const PlayerBuilder* TournamentPlayer::builder() const
{
	return m_builder;
}

QString TournamentPlayer::name() const
{
	return m_builder->name();
}

void TournamentPlayer::setName(const QString& name)
{
	if (m_builder)
		m_builder->setName(name);
}

const TimeControl& TournamentPlayer::timeControl() const
{
	return m_timeControl;
}

const OpeningBook* TournamentPlayer::book() const
{
	return m_book;
}

int TournamentPlayer::bookDepth() const
{
	return m_bookDepth;
}

int TournamentPlayer::wins() const
{
	return m_wins;
}

int TournamentPlayer::draws() const
{
	return m_draws;
}

int TournamentPlayer::losses() const
{
	return m_losses;
}

int TournamentPlayer::whiteWins() const
{
	return m_whiteWins;
}

int TournamentPlayer::whiteDraws() const
{
	return m_whiteDraws;
}

int TournamentPlayer::whiteLosses() const
{
	return m_whiteLosses;
}

int TournamentPlayer::blackWins() const
{
	return m_wins - m_whiteWins;
}

int TournamentPlayer::blackDraws() const
{
	return m_draws - m_whiteDraws;
}

int TournamentPlayer::blackLosses() const
{
	return m_losses - m_whiteLosses;
}

int TournamentPlayer::score() const
{
	return m_wins * 2 + m_draws;
}

void TournamentPlayer::addScore(Chess::Side side, int score)
{
	if (side == Chess::Side::NoSide)
		Q_UNREACHABLE();

	switch (score)
	{
	case 0:
		m_losses++;

		if (side == Chess::Side::White)
			m_whiteLosses++;
		break;
	case 1:
		m_draws++;

		if (side == Chess::Side::White)
			m_whiteDraws++;
		break;
	case 2:
		m_wins++;

		if (side == Chess::Side::White)
			m_whiteWins++;
		break;
	default:
		Q_UNREACHABLE();
		break;
	}
}

int TournamentPlayer::gamesFinished() const
{
	return m_wins + m_draws + m_losses;
}

void TournamentPlayer::addOutcome(int type, QString str)
{
	m_outcome[str]++;
	m_terminations[type]++;
}

int TournamentPlayer::outcomes(int type) const
{
	return m_terminations.at(type);
}

const QMap<QString, int> & TournamentPlayer::outcomeMap() const
{
	return m_outcome;
}
