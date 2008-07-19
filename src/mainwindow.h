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

#include "gameeventlistener.h"

class QMenu;
class QAction;
class ChessboardView;
class QGraphicsScene;
class GraphicsChessboardItem;

/**
 * MainWindow
*/
class MainWindow : public QMainWindow, public GameEventListener
{
	Q_OBJECT

	public:
		MainWindow();
		void moveEvent(const ChessMove& move);

	private slots:
		void printGame();

	private:
		void createActions();
		void createMenus();
		void createToolBars();
		void createDockWindows();

		QMenu* m_gameMenu;
		QMenu* m_viewMenu;
		QMenu* m_helpMenu;

		ChessboardView* m_chessboardView;
		QGraphicsScene* m_chessboardScene;
		GraphicsChessboardItem* m_visualChessboard;

		QAction* m_quitGameAct;
		QAction* m_printGameAct;

};

#endif // MAINWINDOW_H

