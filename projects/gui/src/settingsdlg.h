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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
	class SettingsDialog;
}

/*!
 * \brief Dialog for configuring global settings
 */
class SettingsDialog : public QDialog
{
	Q_OBJECT

	public:
		/*! Creates a new SettingsDialog. */
		SettingsDialog(QWidget* parent = nullptr);
		/*! Destroys the dialog. */
		virtual ~SettingsDialog();

	protected:
		// Inherited from QDialog
		virtual void closeEvent(QCloseEvent* event);

	private slots:
		void browseTbPath();
		void browseDefaultPgnOutFile();
		void browseTournamentDefaultPgnOutFile();
		void browseTournamentDefaultEpdOutFile();

	private:
		void readSettings();

		Ui::SettingsDialog* ui;
};

#endif // SETTINGSDIALOG_H
