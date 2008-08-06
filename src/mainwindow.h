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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QMenu;
class QAction;
class QTextEdit;
class ChessboardView;
class QGraphicsScene;
class GraphicsChessboardItem;
class QProcess;
class EngineConfigurationModel;

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
		void printGame();
		void sloppyVersus();
		void killEngine();

	private:
		void createActions();
		void createMenus();
		void createToolBars();
		void createDockWindows();

		QMenu* m_gameMenu;
		QMenu* m_viewMenu;
		QMenu* m_helpMenu;
		QMenu* m_debugMenu;

		ChessboardView* m_chessboardView;
		QGraphicsScene* m_chessboardScene;
		GraphicsChessboardItem* m_visualChessboard;

		EngineConfigurationModel* m_engineConfigurations;

		QAction* m_quitGameAct;
		QAction* m_printGameAct;
		QAction* m_newGameAct;

		QTextEdit* m_engineDebugTextEdit;

		// Debugging
		QAction* m_sloppyVersusAct;
		QAction* m_killFirstEngineAct;
		QAction* m_killSecondEngineAct;

		QProcess* m_process1;
		QProcess* m_process2;

};

#endif // MAINWINDOW_H

