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

#ifndef GAMESETTINGSWIDGET_H
#define GAMESETTINGSWIDGET_H

#include <QWidget>
#include <timecontrol.h>

namespace Ui {
	class GameSettingsWidget;
}

class GameSettingsWidget : public QWidget
{
	Q_OBJECT

	public:
		explicit GameSettingsWidget(QWidget *parent = nullptr);
		virtual ~GameSettingsWidget();

		QString chessVariant() const;
		TimeControl timeControl() const;

	signals:
		void variantChanged(const QString& variant);

	private slots:
		void showTimeControlDialog();

	private:
		Ui::GameSettingsWidget *ui;
		TimeControl m_timeControl;
};

#endif // GAMESETTINGSWIDGET_H
