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

#ifndef GAME_PROPERTIES_DIALOG_H
#define GAME_PROPERTIES_DIALOG_H

#include <QDialog>

#include "ui_gamepropertiesdlg.h"

class GamePropertiesDialog : public QDialog, private Ui::GamePropertiesDialog
{
	Q_OBJECT

	public:
		GamePropertiesDialog(QWidget* parent = 0);

		void setEvent(const QString& event);
		void setSite(const QString& site);
		void setRound(int round);

		QString event() const;
		QString site() const;
		int round() const;

};

#endif // GAME_PROPERTIES_DIALOG_H

