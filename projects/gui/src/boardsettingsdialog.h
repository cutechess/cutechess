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

#ifndef BOARDSETTINGSDIALOG_H
#define BOARDSETTINGSDIALOG_H

#include <QDialog>
#include "boardview/boardsettings.h"

namespace Ui {
	class BoardSettingsDlg;
}


/*!
 * \brief Dialog for configuring global settings
 */
class BoardSettingsDialog : public QDialog
{
	Q_OBJECT

	public:
		/*! Creates a new BoardSettingsDialog. */
		BoardSettingsDialog(QWidget* parent = nullptr);
		/*! Destroys the dialog. */
		virtual ~BoardSettingsDialog();

	public slots:
		void onContextChanged(const BoardSettings* boardSettings);
		BoardSettings boardSettings();

	protected:
		// Inherited from QDialog
		virtual void closeEvent(QCloseEvent* event);

	private:
		void readSettings();

		Ui::BoardSettingsDlg* ui;
};

#endif // BOARDSETTINGSDIALOG_H
