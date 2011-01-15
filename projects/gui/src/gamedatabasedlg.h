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

#ifndef GAME_DATABASE_DIALOG_H
#define GAME_DATABASE_DIALOG_H

#include "ui_gamedatabasedlg.h"
#include <QDialog>

#include <pgngame.h>

class PgnDatabaseModel;
class PgnGameEntryModel;
class PgnDatabase;
class BoardView;
class BoardScene;

class GameDatabaseDialog : public QDialog, private Ui::GameDatabaseDialog
{
	Q_OBJECT

	public:
		GameDatabaseDialog();
		~GameDatabaseDialog();

	private slots:
		void databaseSelectionChanged(const QModelIndex& current,
		                              const QModelIndex& previous);
		void gameSelectionChanged(const QModelIndex& current,
		                          const QModelIndex& previous);
		void viewNextMove();
		void viewPreviousMove();

	private:
		BoardView* m_boardView;
		BoardScene* m_boardScene;
		QVector<PgnGame::MoveData> m_moves;
		int m_moveIndex;

		PgnDatabaseModel* m_pgnDatabaseModel;
		PgnGameEntryModel* m_pgnGameEntryModel;
		PgnDatabase* m_selectedDatabase;

};

#endif // GAME_DATABASE_DIALOG_H
