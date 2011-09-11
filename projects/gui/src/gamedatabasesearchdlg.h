/*
    This file is part of Cute Chess.

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

#ifndef GAMEDATABASESEARCHDIALOG_H
#define GAMEDATABASESEARCHDIALOG_H

#include "ui_gamedatabasesearchdlg.h"
#include <QDialog>
#include <pgngamefilter.h>

/*!
 * \brief Dialog for searching and filtering game databases.
 *
 * \sa GameDatabaseDialog
 */
class GameDatabaseSearchDialog : public QDialog, private Ui::GameDatabaseSearchDialog
{
	Q_OBJECT

	public:
		/*! Constructs a new GameDatabaseSearchDialog. */
		GameDatabaseSearchDialog(QWidget* parent = 0);
		virtual ~GameDatabaseSearchDialog();

		/*! Returns the PGN filter. */
		PgnGameFilter filter() const;
};

#endif // GAMEDATABASESEARCHDIALOG_H
