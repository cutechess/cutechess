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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>

namespace Chess { class Board; class Move; }
class QMenu;
class QAction;
class QTextEdit;
class QCloseEvent;
class QGraphicsView;
class QTabBar;
class BoardScene;
class MoveListModel;
class EngineConfigurationModel;
class ChessClock;
class PlainTextLog;
class ChessGame;

/**
 * MainWindow
*/
class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		explicit MainWindow(ChessGame* game);
		virtual ~MainWindow();
		QString windowListTitle() const;

	public slots:
		void addGame(ChessGame* game);

	protected:
		virtual void closeEvent(QCloseEvent* event);

	private slots:
		void newGame();
		void gameProperties();
		void manageEngines();
		void saveLogToFile();
		void onWindowMenuAboutToShow();
		void showGameWindow();
		void updateWindowTitle();
		bool save();
		bool saveAs();
		void onTabChanged(int index);
		void onTabCloseRequested(int index);
		void removeGame(ChessGame* game);
		void destroyGame(ChessGame* game);

	private:
		void createActions();
		void createMenus();
		void createToolBars();
		void createDockWindows();
		void readSettings();
		QString genericTitle(ChessGame* game) const;
		bool saveGame(const QString& fileName);
		bool askToSave();
		void setCurrentGame(ChessGame* game);

		QMenu* m_gameMenu;
		QMenu* m_viewMenu;
		QMenu* m_enginesMenu;
		QMenu* m_windowMenu;
		QMenu* m_helpMenu;

		QTabBar* m_tabs;

		BoardScene* m_boardScene;
		QGraphicsView* m_boardView;
		MoveListModel* m_moveListModel;
		ChessClock* m_chessClock[2];

		QAction* m_quitGameAct;
		QAction* m_gamePropertiesAct;
		QAction* m_newGameAct;
		QAction* m_closeGameAct;
		QAction* m_saveGameAct;
		QAction* m_saveGameAsAct;
		QAction* m_manageEnginesAct;
		QAction* m_showGameDatabaseWindowAct;
		QAction* m_showGameWallAct;

		PlainTextLog* m_engineDebugLog;

		QPointer<ChessGame> m_game;
		QList<ChessGame*> m_games;

		QString m_currentFile;
		bool m_closing;
};

#endif // MAINWINDOW_H
