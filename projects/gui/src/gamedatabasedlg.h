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

#include <QDialog>
#include <QTimer>

#include <pgngame.h>

class QModelIndex;
class PgnDatabaseModel;
class PgnGameEntryModel;
class PgnDatabase;
class BoardView;
class BoardScene;

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
		GameDatabaseDialog(QWidget* parent = 0);
		/*! Destroys the dialog. */
		virtual ~GameDatabaseDialog();

	private slots:
		void databaseSelectionChanged(const QModelIndex& current,
		                              const QModelIndex& previous);
		void gameSelectionChanged(const QModelIndex& current,
		                          const QModelIndex& previous);
		void viewNextMove();
		void viewPreviousMove();
		void viewFirstMove();
		void viewLastMove();
		void updateSearch(const QString& terms = QString());
		void onSearchTimeout();
		void onAdvancedSearch();

	private:
		BoardView* m_boardView;
		BoardScene* m_boardScene;
		QVector<PgnGame::MoveData> m_moves;
		int m_moveIndex;

		PgnDatabaseModel* m_pgnDatabaseModel;
		PgnGameEntryModel* m_pgnGameEntryModel;
		PgnDatabase* m_selectedDatabase;

		QTimer m_searchTimer;
		QString m_searchTerms;
		Ui::GameDatabaseDialog* ui;
};

#endif // GAME_DATABASE_DIALOG_H
