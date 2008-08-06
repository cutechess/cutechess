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
#include "uciengine.h"
#include "timecontrol.h"
#include "engineconfigurationmodel.h"
#include "engineconfiguration.h"
#include "newgamedlg.h"

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

	m_engineConfigurations = new EngineConfigurationModel(this);

	createActions();
	createMenus();
	createToolBars();
	createDockWindows();

	m_process1 = 0;
	m_process2 = 0;
}

void MainWindow::createActions()
{
	m_newGameAct = new QAction(tr("&New game..."), this);

	m_printGameAct = new QAction(tr("&Print..."), this);

	m_quitGameAct = new QAction(tr("&Quit"), this);
	m_quitGameAct->setShortcut(QKeySequence(tr("Ctrl+Q")));

	// Debugging actions
	m_sloppyVersusAct = new QAction("Sloppy vs. Sloppy", this);
	m_killFirstEngineAct = new QAction("Kill First Engine", this);
	m_killSecondEngineAct = new QAction("Kill Second Engine", this);

	connect(m_newGameAct, SIGNAL(triggered(bool)), this, SLOT(newGame()));
	connect(m_printGameAct, SIGNAL(triggered(bool)), this, SLOT(printGame()));
	connect(m_quitGameAct, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
	connect(m_sloppyVersusAct, SIGNAL(triggered(bool)), this, SLOT(sloppyVersus()));
	connect(m_killFirstEngineAct, SIGNAL(triggered(bool)), this, SLOT(killEngine()));
	connect(m_killSecondEngineAct, SIGNAL(triggered(bool)), this, SLOT(killEngine()));
}

void MainWindow::createMenus()
{
	m_gameMenu = menuBar()->addMenu(tr("&Game"));
	m_gameMenu->addAction(m_newGameAct);
	m_gameMenu->addSeparator();
	m_gameMenu->addAction(m_printGameAct);
	m_gameMenu->addSeparator();
	m_gameMenu->addAction(m_quitGameAct);

	m_viewMenu = menuBar()->addMenu(tr("&View"));
	m_helpMenu = menuBar()->addMenu(tr("&Help"));

	m_debugMenu = menuBar()->addMenu("&Debug");
	m_debugMenu->addAction(m_sloppyVersusAct);
	m_debugMenu->addSeparator();
	m_debugMenu->addAction(m_killFirstEngineAct);
	m_debugMenu->addAction(m_killSecondEngineAct);
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

void MainWindow::newGame()
{
	NewGameDialog dlg(m_engineConfigurations, this);
	if (dlg.exec() != QDialog::Accepted)
		return;

	if (dlg.whitePlayerType() == NewGameDialog::Human ||
	    dlg.blackPlayerType() == NewGameDialog::Human)
	{
		qDebug() << "Human players are not currently supported.";
		return;
	}

	const EngineConfiguration whiteEngineConfig =
		m_engineConfigurations->engineConfigurations().at(dlg.selectedWhiteEngine());

	const EngineConfiguration blackEngineConfig =
		m_engineConfigurations->engineConfigurations().at(dlg.selectedBlackEngine());

	QProcess* whiteEngineProcess = new QProcess(this);
	QProcess* blackEngineProcess = new QProcess(this);

	whiteEngineProcess->start(whiteEngineConfig.command());
	blackEngineProcess->start(blackEngineConfig.command());

	if (!whiteEngineProcess->waitForStarted())
	{
		qDebug() << "Cannot start the engine process:" << whiteEngineConfig.command();
		return;
	}

	if (!blackEngineProcess->waitForStarted())
	{
		qDebug() << "Cannot start the engine process:" << blackEngineConfig.command();
		return;
	}

	ChessGame* chessgame = new ChessGame(this);

	connect(chessgame, SIGNAL(moveHappened(const ChessMove&)),
	        m_visualChessboard, SLOT(makeMove(const ChessMove&)));

	ChessPlayer* whitePlayer = 0;
	ChessPlayer* blackPlayer = 0;

	TimeControl tc;
	tc.setTimePerTc(10000);
	tc.setIncrement(1000);

	switch (whiteEngineConfig.protocol())
	{
		case EngineConfiguration::UCI:
			whitePlayer = new UciEngine(whiteEngineProcess, chessgame->chessboard(),
				tc, this);
		break;

		default:
			whitePlayer = new XboardEngine(whiteEngineProcess, chessgame->chessboard(),
				tc, this);
		break;
	}

	switch (blackEngineConfig.protocol())
	{
		case EngineConfiguration::UCI:
			blackPlayer = new UciEngine(blackEngineProcess, chessgame->chessboard(),
				tc, this);
		break;

		default:
			blackPlayer = new XboardEngine(blackEngineProcess, chessgame->chessboard(),
				tc, this);
		break;
	}

	connect(whitePlayer, SIGNAL(debugMessage(const QString&)),
	        m_engineDebugTextEdit, SLOT(append(const QString&)));
	connect(blackPlayer, SIGNAL(debugMessage(const QString&)),
	        m_engineDebugTextEdit, SLOT(append(const QString&)));

	chessgame->newGame(whitePlayer, blackPlayer);
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
	m_process1 = new QProcess(this);
	m_process2 = new QProcess(this);

	m_process1->setWorkingDirectory("engines/sloppy-020");
	m_process2->setWorkingDirectory("engines/sloppy-020");
	
	m_process1->start("./sloppy");
	m_process2->start("./sloppy");

	if (!m_process1->waitForStarted())
	{
		qDebug() << "Cannot start the first engine process.";
		return;
	}

	if (!m_process2->waitForStarted())
	{
		qDebug() << "Cannot start the second engine process.";
		return;
	}

	ChessGame* chessgame = new ChessGame(this);

	connect(chessgame, SIGNAL(moveHappened(const ChessMove&)),
	        m_visualChessboard, SLOT(makeMove(const ChessMove&)));

	TimeControl tc;
	tc.setTimePerTc(10000);
	tc.setIncrement(1000);

	ChessPlayer* player1 = new XboardEngine(m_process1, chessgame->chessboard(), tc, this);

	ChessPlayer* player2 = new XboardEngine(m_process2, chessgame->chessboard(), tc, this);

	connect(player1, SIGNAL(debugMessage(const QString&)),
	        m_engineDebugTextEdit, SLOT(append(const QString&)));
	connect(player2, SIGNAL(debugMessage(const QString&)),
	        m_engineDebugTextEdit, SLOT(append(const QString&)));
	
	chessgame->newGame(player1, player2);
}

void MainWindow::killEngine()
{
	QAction* engineAct = qobject_cast<QAction *>(QObject::sender());

	if (engineAct == m_killFirstEngineAct && m_process1)
	{
		m_process1->kill();
		qDebug() << "Terminated.";
	}
	else if (engineAct == m_killSecondEngineAct && m_process2)
	{
		m_process2->kill();
		qDebug() << "Terminated.";
	}
	else
	{
		qDebug() << "Can't do that.";
	}
}

