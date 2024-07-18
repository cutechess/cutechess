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

#ifndef PAIRTIMECONTROLDIALOG_H
#define PAIRTIMECONTROLDIALOG_H

#include <QDialog>
#include <timecontrol.h>
#include <board/side.h>

namespace Ui {
        class PairTimeControlDialog;
}

/*!
 * \brief A dialog for setting a chess game's time controls
 */
class PairTimeControlDialog : public QDialog
{
	Q_OBJECT

	public:
		/*!
		 * Creates a new time control dialog for pairs (white and black)
		 *
		 * The dialog is initialized according to \a tcWhite,
		 * and \a tcBlack
		 */
		explicit PairTimeControlDialog(const TimeControl& tcWhite,
					       const TimeControl& tcBlack = TimeControl(),
					       QWidget* parent = nullptr);
		/*! Destroys the dialog. */
		virtual ~PairTimeControlDialog();

		/*!
		 * Returns the time control that was set in the dialog
		 * for the specified \a side.
		 */
		TimeControl timeControl(Chess::Side side = Chess::Side::NoSide) const;

	private:
		void syncTimeControls();
		void unsyncTimeControls();

		Ui::PairTimeControlDialog *ui;
};

#endif // PAIRTIMECONTROLDIALOG_H
