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

class QMenu;
class QAction;
class QTextEdit;
class ChessboardView;
class ChessboardModel;
class MoveListModel;
class EngineConfigurationModel;
class ChessClock;
class PlainTextLog;

/**
 * MainWindow
*/
class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow();

	private slots:
		void newGame();
		void manageEngines();
		void saveLogToFile();

	private:
		void createActions();
		void createMenus();
		void createToolBars();
		void createDockWindows();
		void readSettings();

		QMenu* m_gameMenu;
		QMenu* m_viewMenu;
		QMenu* m_enginesMenu;
		QMenu* m_helpMenu;

		ChessboardView* m_chessboardView;
		ChessboardModel* m_boardModel;
		MoveListModel* m_moveListModel;
		ChessClock* m_chessClock[2];

		EngineConfigurationModel* m_engineConfigurations;

		QAction* m_quitGameAct;
		QAction* m_newGameAct;
		QAction* m_manageEnginesAct;

		PlainTextLog* m_engineDebugLog;

};

#endif // MAINWINDOW_H
