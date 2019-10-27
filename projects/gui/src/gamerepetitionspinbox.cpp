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

#include "gamerepetitionspinbox.h"

GameRepetitionSpinBox::GameRepetitionSpinBox(QWidget *parent)
	: QSpinBox(parent),
	  m_gamesPerEncounter(1),
	  m_rounds(1)
{
}

void GameRepetitionSpinBox::setGamesPerEncounter(int gamesPerEncounter)
{
	m_gamesPerEncounter = gamesPerEncounter;
	setMaximum(limit());
	interpretText();
}

void GameRepetitionSpinBox::setRounds(int rounds)
{
	m_rounds = rounds;
	setMaximum(limit());
	interpretText();
}

void GameRepetitionSpinBox::setTournamentType(const QString& tournamentType)
{
	m_tournamentType = tournamentType;
	setMaximum(limit());
	interpretText();
}

// Returns the highest matching number of games
int GameRepetitionSpinBox::limit() const
{
	int games = qMax(m_gamesPerEncounter, 1);
	if (m_tournamentType != "gauntlet")
		return games;

	int factor = qMin(m_rounds, 10000000 / games);
	while (m_rounds % factor)
	{
		factor--;
		// Avoid division by zero
		if (factor < 1)
			return games;
	}

	return games * factor;
}

QValidator::State GameRepetitionSpinBox::examine(int value) const
{
	int games = m_gamesPerEncounter;
	if (games == 0 || value == 0)
		return QValidator::State::Invalid;
	if (games % value == 0)
		return QValidator::State::Acceptable;

	if (m_tournamentType != "gauntlet")
		return QValidator::State::Intermediate;

	if (value % games == 0 && (long)m_rounds * (long)games % value == 0)
		return QValidator::State::Acceptable;

	return QValidator::State::Intermediate;
}

QValidator::State GameRepetitionSpinBox::validate(QString& input, int&) const
{
	int value = valueFromText(input);
	if (value < 1)
		return QValidator::State::Invalid;

	if (m_gamesPerEncounter <= 0
	||  m_rounds <= 0
	||  m_tournamentType.isEmpty())
		return QValidator::State::Invalid;

	return examine(value);
}

void GameRepetitionSpinBox::fixup(QString& input) const
{
	int value = valueFromText(input);
	value = qMin(value, limit());
	while (examine(value) != QValidator::State::Acceptable)
	{
		value--;
		// No infinite loops
		if (value < 1)
		{
			input = "1";
			return;
		}
	}

	input = textFromValue(value);
}

void GameRepetitionSpinBox::stepBy(int steps)
{
	QSpinBox::stepBy(steps);

	int val = value();
	int upperLimit = limit();
	val = qMin(val, upperLimit);
	int step = steps > 0 ? 1 : -1;

	while (examine(val) != QValidator::State::Acceptable
	&&     val < upperLimit && val > 1)
	{
		val += step;
	}
	setValue(qBound(1, val, upperLimit));
}
