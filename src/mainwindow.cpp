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

#include <QtGui>
#include <QDebug>
#include <QApplication>

#include "mainwindow.h"
#include "manager.h"
#include "logmanager.h"
#include "guilogger.h"
#include "graphicschessboarditem.h"
#include "chessboardview.h"

MainWindow::MainWindow()
{
	m_chessboardView = new ChessboardView(this);
	m_chessboardScene = new QGraphicsScene(m_chessboardView);
	m_chessboardView->setScene(m_chessboardScene);
	m_chessboardView->setMinimumSize(400, 400);

	setCentralWidget(m_chessboardView);

	m_visualChessboard = new GraphicsChessboardItem();
	m_chessboardScene->addItem(m_visualChessboard);
	
	setStatusBar(new QStatusBar());

	createActions();
	createMenus();
	createToolBars();
	createDockWindows();
}

void MainWindow::createActions()
{
	m_quitGameAct = new QAction(tr("&Quit"), this);
	m_quitGameAct->setShortcut(QKeySequence(tr("Ctrl+Q")));

	connect(m_quitGameAct, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
}

void MainWindow::createMenus()
{
	m_gameMenu = menuBar()->addMenu(tr("&Game"));
	m_gameMenu->addAction(m_quitGameAct);

	m_viewMenu = menuBar()->addMenu(tr("&View"));
	m_helpMenu = menuBar()->addMenu(tr("&Help"));
}

void MainWindow::createToolBars()
{
	// Create tool bars here, use actions from createActions()
	// See: createActions(), QToolBar documentation
}

void MainWindow::createDockWindows()
{
	// Log
	QDockWidget* logDock = new QDockWidget(tr("Log"), this);
	QTextEdit* logTextEdit = new QTextEdit(logDock);
	logTextEdit->setReadOnly(true);
	logDock->setWidget(logTextEdit);

	addDockWidget(Qt::BottomDockWidgetArea, logDock);

	// Set up GUI logging
	Manager::get()->getLogManager()->addLogger(new GuiLogger(logTextEdit));
}

