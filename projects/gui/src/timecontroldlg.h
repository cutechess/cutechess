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

#ifndef TIMECONTROLDIALOG_H
#define TIMECONTROLDIALOG_H

#include <QDialog>
#include <timecontrol.h>

namespace Ui {
	class TimeControlDialog;
}

/*!
 * \brief A dialog for setting a chess game's time controls
 */
class TimeControlDialog : public QDialog
{
	Q_OBJECT

	public:
		/*!
		 * Creates a new time control dialog.
		 *
		 * The dialog is initialized according to \a tc.
		 */
		explicit TimeControlDialog(const TimeControl& tc,
					   QWidget *parent = nullptr);
		/*! Destroys the dialog. */
		virtual ~TimeControlDialog();

		/*! Returns the time control that was set in the dialog. */
		TimeControl timeControl() const;

	private slots:
		void onTournamentSelected();
		void onTimePerMoveSelected();
		void onInfiniteSelected();

	private:
		enum TimeUnit
		{
			Seconds,
			Minutes,
			Hours
		};

		int timeToMs() const;
		void setTime(int ms);

		Ui::TimeControlDialog *ui;
};

#endif // TIMECONTROLDIALOG_H
