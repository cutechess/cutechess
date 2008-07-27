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
#include "chessgame.h"
#include "chessplayer.h"
#include "xboardengine.h"

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
	m_printGameAct = new QAction(tr("&Print..."), this);

	m_quitGameAct = new QAction(tr("&Quit"), this);
	m_quitGameAct->setShortcut(QKeySequence(tr("Ctrl+Q")));

	// Debugging actions
	m_sloppyVersusAct = new QAction("Sloppy vs. Sloppy", this);

	connect(m_printGameAct, SIGNAL(triggered(bool)), this, SLOT(printGame()));
	connect(m_quitGameAct, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
	connect(m_sloppyVersusAct, SIGNAL(triggered(bool)), this, SLOT(sloppyVersus()));
}

void MainWindow::createMenus()
{
	m_gameMenu = menuBar()->addMenu(tr("&Game"));
	m_gameMenu->addAction(m_printGameAct);
	m_gameMenu->addSeparator();
	m_gameMenu->addAction(m_quitGameAct);

	m_viewMenu = menuBar()->addMenu(tr("&View"));
	m_helpMenu = menuBar()->addMenu(tr("&Help"));

	m_debugMenu = menuBar()->addMenu("&Debug");
	m_debugMenu->addAction(m_sloppyVersusAct);
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

	// Engine debug
	QDockWidget* engineDebugDock = new QDockWidget(tr("Engine Debug"), this);
	m_engineDebugTextEdit = new QTextEdit(engineDebugDock);
	m_engineDebugTextEdit->setReadOnly(true);
	engineDebugDock->setWidget(m_engineDebugTextEdit);

	addDockWidget(Qt::BottomDockWidgetArea, engineDebugDock);
}

void MainWindow::printGame()
{
	QPrinter printer(QPrinter::HighResolution);

	QPrintDialog* printDialog = new QPrintDialog(&printer, this);
	printDialog->setWindowTitle(tr("Print game"));

	if (printDialog->exec() != QDialog::Accepted)
		return;
	
	QPainter painter;
	painter.begin(&printer);

	m_chessboardView->render(&painter);

	painter.end();
}

void MainWindow::sloppyVersus()
{
	QProcess* process1 = new QProcess(this);
	QProcess* process2 = new QProcess(this);

	process1->setWorkingDirectory("/home/ilari/apps/sloppy/src");
	process2->setWorkingDirectory("/home/ilari/apps/sloppy/pak/sloppy-linux64");
	
	process1->start("./sloppy");
	process2->start("./sloppy");

	if (!process1->waitForStarted() || !process2->waitForStarted())
	{
		qDebug() << "Cannot start sloppy process.";
		return;
	}

	ChessGame* chessgame = new ChessGame(this);
	connect(chessgame, SIGNAL(debugMessage(const QString&)),
	        m_engineDebugTextEdit, SLOT(append(const QString&)));

	ChessPlayer* player1 = new XboardEngine(process1, chessgame->chessboard(), this);
	ChessPlayer* player2 = new XboardEngine(process2, chessgame->chessboard(), this);
	
	chessgame->newGame(player1, player2);
}

