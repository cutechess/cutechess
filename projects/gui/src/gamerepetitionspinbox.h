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

#ifndef GAMEREPETITIONSPINBOX_H
#define GAMEREPETITIONSPINBOX_H

#include <QSpinBox>
#include <QRadioButton>
#include <QPointer>

class GameRepetitionSpinBox : public QSpinBox
{
	Q_OBJECT

	public:
		/*! Creates a new GameRepetitionSpinBox with parent \a parent. */
		explicit GameRepetitionSpinBox(QWidget *parent = nullptr);

		// Inherited from QSpinBox
		virtual QValidator::State validate(QString & input, int & pos) const override;
		virtual void fixup(QString & input) const override;
		virtual void stepBy(int steps) override;

	public slots:
		/*!
		 * Adjusts the step value and maximum value based on \a gamesPerEncounter.
		 *
		 * If the spin box's current value is no longer acceptable, the closest
		 * acceptable value is set.
		 */
		void setGamesPerEncounter(int gamesPerEncounter);
		/*!
		 * Adjusts the step value and maximum value based on \a rounds.
		 *
		 * If the spin box's current value is no longer acceptable, the closest
		 * acceptable value is set.
		 */
		void setRounds(int rounds);
		/*!
		 * Adjusts the current value based on \a tournamentType.
		 *
		 * If the spin box's current value is no longer acceptable, the closest
		 * acceptable value is set.
		 */
		void setTournamentType(const QString& tournamentType);

	private:
		int limit() const;
		QValidator::State examine(int value) const;

		int m_gamesPerEncounter;
		int m_rounds;
		QString m_tournamentType;
};

#endif // GAMEREPETITIONSPINBOX_H
