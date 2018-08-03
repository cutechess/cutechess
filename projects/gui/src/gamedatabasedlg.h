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

#ifndef GAME_DATABASE_DIALOG_H
#define GAME_DATABASE_DIALOG_H

#include <QDialog>
#include <QTimer>
#include <QItemSelection>

#include <pgngame.h>

class GameDatabaseManager;
class PgnDatabaseModel;
class PgnGameEntryModel;
class PgnDatabase;
class GameViewer;

namespace Ui {
	class GameDatabaseDialog;
}

/*!
 * \brief Dialog for viewing game databases.
 *
 * \sa GameDatabaseManager
 */
class GameDatabaseDialog : public QDialog
{
	Q_OBJECT

	public:
		/*! Constructs a new GameDatabaseDialog. */
		GameDatabaseDialog(GameDatabaseManager* dbManager,
				   QWidget* parent = nullptr);
		/*! Destroys the dialog. */
		virtual ~GameDatabaseDialog();

	private slots:
		void databaseSelectionChanged(const QItemSelection& selected,
		                              const QItemSelection& deselected);
		void gameSelectionChanged(const QModelIndex& current,
		                          const QModelIndex& previous);
		void updateSearch(const QString& terms = QString());
		void onSearchTimeout();
		void onAdvancedSearch();
		void exportPgn(const QString& filename);
		void createOpeningBook();
		void copyGame();
		void copyFen();
		void updateUi();

	private:
		friend class PgnGameIterator;
		int databaseIndexFromGame(int game) const;

		GameViewer* m_gameViewer;
		PgnGame m_game;
		QVector<PgnGame::MoveData> m_moves;

		GameDatabaseManager* m_dbManager;
		PgnDatabaseModel* m_pgnDatabaseModel;
		PgnGameEntryModel* m_pgnGameEntryModel;
		QMap<int, PgnDatabase*> m_selectedDatabases;

		QTimer m_searchTimer;
		QString m_searchTerms;
		Ui::GameDatabaseDialog* ui;
};

#endif // GAME_DATABASE_DIALOG_H
