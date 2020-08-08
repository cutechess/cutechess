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

#ifndef TIMECONTROLWIDGET_H
#define TIMECONTROLWIDGET_H

#include <QWidget>
#include <timecontrol.h>

namespace Ui {
	class TimeControlWidget;
}

/*!
 * \brief A dialog for setting a chess game's time controls
 */
class TimeControlWidget : public QWidget
{
	Q_OBJECT

	public:
		/*!
		 * Creates a new time control widget.
		 *
		 * The widget is initialized according to \a tc.
		 */
		explicit TimeControlWidget(QWidget *parent = nullptr);
		/*! Destroys the dialog. */
		virtual ~TimeControlWidget();

		/*! Initialise time contol, signals and slots */
		void init(const TimeControl& tc);
		/*! Returns the time control that was set in the dialog. */
		TimeControl timeControl() const;

	signals:
		void hourglassToggled(bool);

	public slots: void setHourglassMode(bool enabled);
		      void disableHourglassRadio();

	private slots:
		void onTournamentSelected();
		void onTimePerMoveSelected();
		void onInfiniteSelected();
		void onHourglassSelected();

	private:
		enum TimeUnit
		{
			Seconds,
			Minutes,
			Hours
		};

		int timeToMs() const;
		void setTime(int ms);

		Ui::TimeControlWidget *ui;
		TimeControl m_tc;
};

#endif // TIMECONTROLWIDGET_H
