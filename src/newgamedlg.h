/*
    This file is part of SloppyGUI.

    SloppyGUI is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SloppyGUI is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SloppyGUI.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef NEWGAMEDIALOG_H
#define NEWGAMEDIALOG_H

#include <QDialog>

#include "ui_newgamedlg.h"

class EngineConfigurationModel;

/*!
 * \brief The NewGameDialog class provides a dialog for creating a new game.
*/
class NewGameDialog : public QDialog, private Ui::NewGameDialog
{
	Q_OBJECT

	public:
		/*! Player's type. */
		enum PlayerType
		{
			/*! Human player. */
			Human,
			/*! Computer controlled player. */
			CPU
		};

		/*!
		 * Creates a new game dialog with \a engineConfigurations as the
		 * list of chess engines and given \a parent.
		*/
		NewGameDialog(EngineConfigurationModel* engineConfigurations,
		              QWidget* parent = 0);

		/*! Returns the user selected player type for white. */
		PlayerType whitePlayerType() const;
		/*! Returns the user selected player type for black. */
		PlayerType blackPlayerType() const;

		/*!
		 * Returns the user selected chess engine for white.
		 *
		 * The return value is an index to the engine configurations model.
		*/
		int selectedWhiteEngine() const;
		/*!
		 * Returns the user selected chess engine for black.
		 *
		 * The return value is an index to the engine configurations model.
		*/
		int selectedBlackEngine() const;

};

#endif // NEWGAMEDIALOG_H

