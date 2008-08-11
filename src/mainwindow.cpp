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
#include <QVBoxLayout>
#include <QHBoxLayout>

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
#include "chessclock.h"

MainWindow::MainWindow()
{
	m_whiteClock = new ChessClock();
	m_blackClock = new ChessClock();
	
	QHBoxLayout* clockLayout = new QHBoxLayout();
	clockLayout->addWidget(m_whiteClock);
	clockLayout->addWidget(m_blackClock);

	m_chessboardView = new ChessboardView;
	m_chessboardScene = new QGraphicsScene(m_chessboardView);
	m_chessboardView->setScene(m_chessboardScene);
	m_chessboardView->setMinimumSize(400, 400);

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addLayout(clockLayout);
	mainLayout->addWidget(m_chessboardView);

	m_visualChessboard = new GraphicsChessboardItem();
	m_chessboardScene->addItem(m_visualChessboard);

	QWidget* mainWidget = new QWidget(this);
	mainWidget->setLayout(mainLayout);
	setCentralWidget(mainWidget);
	
	setStatusBar(new QStatusBar());

	m_engineConfigurations = new EngineConfigurationModel(this);
	m_engineConfigurations->addConfiguration(
		EngineConfiguration("Sloppy", "sloppy", EngineConfiguration::Xboard));
	m_engineConfigurations->addConfiguration(
		EngineConfiguration("Fruit", "fruit", EngineConfiguration::UCI));

	createActions();
	createMenus();
	createToolBars();
	createDockWindows();
}

void MainWindow::createActions()
{
	m_newGameAct = new QAction(tr("&New game..."), this);

	m_printGameAct = new QAction(tr("&Print..."), this);

	m_quitGameAct = new QAction(tr("&Quit"), this);
	m_quitGameAct->setShortcut(QKeySequence(tr("Ctrl+Q")));

	connect(m_newGameAct, SIGNAL(triggered(bool)), this, SLOT(newGame()));
	connect(m_printGameAct, SIGNAL(triggered(bool)), this, SLOT(printGame()));
	connect(m_quitGameAct, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
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
		m_engineConfigurations->configurations().at(dlg.selectedWhiteEngine());

	const EngineConfiguration blackEngineConfig =
		m_engineConfigurations->configurations().at(dlg.selectedBlackEngine());

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
	tc.setTimePerTc(180000);
	tc.setMovesPerTc(40);

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

	connect(whitePlayer, SIGNAL(startedThinking(int)),
		m_whiteClock, SLOT(start(int)));
	connect(blackPlayer, SIGNAL(startedThinking(int)),
		m_blackClock, SLOT(start(int)));

	connect(whitePlayer, SIGNAL(moveMade(const ChessMove&)),
		m_whiteClock, SLOT(stop()));
	connect(blackPlayer, SIGNAL(moveMade(const ChessMove&)),
		m_blackClock, SLOT(stop()));

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

