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
		explicit TimeControlWidget(QWidget* parent = nullptr);

		/*! Destroys the dialog. */
		virtual ~TimeControlWidget();

		/*! Time control mode */
		enum Mode
		{
			Tournament,	//!< Tournament time controls, e.g. 1h per 40 moves
			TimePerMove,	//!< Same amount of time for each move
			Infinite,	//!< Infinite thinking time
			Hourglass	//!< Player B's time left goes up as A's goes down
		};
		Q_ENUM(Mode)

		/*! Initialise time contol, signals and slots */
		void init(const TimeControl& tc);

		/*! Returns the time control that was set in the dialog. */
		TimeControl timeControl() const;

		/*!
		 * Starts propagating all inputs from \a other to this widget.
		 *
		 * \note The current state of \a other is NOT automatically
		 * copied to this widget.
		 */
		void syncWith(TimeControlWidget* other);

		/*! Stops propagating inputs from \a other to this widget. */
		void unsyncWith(TimeControlWidget* other);

	public slots:
		/*! Sets the time control mode to \a mode. */
		void setTimeControlMode(Mode mode);

	signals:
		/*! Emitted when the time control mode changes. */
		void timeControlModeChanged(Mode mode);

	private slots:
		void onOtherTimeControlModeChanged(Mode mode);

	private:
		void onTournamentSelected();
		void onTimePerMoveSelected();
		void onInfiniteSelected();
		void onHourglassSelected();

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
		Mode m_tcMode;
};

#endif // TIMECONTROLWIDGET_H
