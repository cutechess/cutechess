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

class NewGameDialog : public QDialog, private Ui::NewGameDialog
{
	Q_OBJECT

	public:
		enum PlayerType
		{
			Human,
			CPU
		};

		NewGameDialog(EngineConfigurationModel* engineConfigurations,
		              QWidget* parent = 0);

		PlayerType whitePlayerType() const;
		PlayerType blackPlayerType() const;
		int selectedWhiteEngine() const;
		int selectedBlackEngine() const;

};

#endif // NEWGAMEDIALOG_H

