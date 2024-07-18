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

#ifndef TIMECONTROLDLG_H
#define TIMECONTROLDLG_H

#include <QDialog>
#include <timecontrol.h>

namespace Ui {
	class TimeControlDialog;
}

/*!
 * \brief A dialog for setting a chess player's, or tournament's time controls
 */
class TimeControlDialog : public QDialog
{
	Q_OBJECT

	public:
		/*! Creates a new time control dialog */
		explicit TimeControlDialog(const TimeControl& tc,
								   QWidget *parent = nullptr);
		/*! Destroys the dialog */
		~TimeControlDialog();

		/*! Returns the time control that was set in the dialog */
		TimeControl timeControl() const;

	private:
		Ui::TimeControlDialog *ui;
};

#endif // TIMECONTROLDLG_H
