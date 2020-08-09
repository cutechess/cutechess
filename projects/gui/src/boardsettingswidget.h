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

#ifndef BOARDSETTINGSWIDGET_H
#define BOARDSETTINGSWIDGET_H

#include <QWidget>
#include <QVariant>
#include "boardview/boardsettings.h"

namespace Ui {
	class BoardSettingsWidget;
}
namespace Chess {
	class Board;
}
class GameViewer;
class QLineEdit;

class BoardSettingsWidget : public QWidget
{
	Q_OBJECT

	public:
		explicit BoardSettingsWidget(QWidget *parent = nullptr);
		virtual ~BoardSettingsWidget();

		void enableSettingsUpdates();
		BoardSettings boardSettings();

	public slots:
		void onContextChanged(const BoardSettings* boardSettings);
		void restore();
		void onDialogColorChanged(QColor c);

	signals:
		void colorSelectionChanged(QColor color);
		void colorChanged(QString s);

	private slots:
		void updatePreview();
		void onLightSquareColorBtnClicked();
		void onDarkSquareColorBtnClicked();
		void onBorderColorBtnClicked();
		void onCoordTextColorBtnClicked();

	private:
		void readSettings();
		void readBoardSettings(const BoardSettings *boardSettinsgs);
		void readPieceSetEntries();
		void readPieceSet(QVariant value = QVariant());
		void enableButtons(bool enabled);
		void showColorDialog(QLineEdit* edit);

		Ui::BoardSettingsWidget *ui;
		GameViewer* m_gameViewer;
		BoardSettings m_testSettings;
		BoardSettings m_oldSettings;
		bool m_updateRequested;
};

#endif // BOARDSETTINGSWIDGET_H
