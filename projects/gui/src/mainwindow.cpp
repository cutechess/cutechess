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

#include <chessgame.h>
#include <chessplayer.h>
#include <xboardengine.h>
#include <uciengine.h>
#include <timecontrol.h>
#include <engineconfiguration.h>

#include "mainwindow.h"
#include "graphicschessboarditem.h"
#include "chessboardview.h"
#include "newgamedlg.h"
#include "chessclock.h"
#include "engineconfigurationmodel.h"
#include "enginemanagementdlg.h"

MainWindow::MainWindow()
{
	QHBoxLayout* clockLayout = new QHBoxLayout();
	for (int i = 0; i < 2; i++)
	{
		m_chessClock[i] = new ChessClock();
		clockLayout->addWidget(m_chessClock[i]);
	}

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

	createActions();
	createMenus();
	createToolBars();
	createDockWindows();

	readSettings();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	writeSettings();
	event->accept();
}

void MainWindow::createActions()
{
	m_newGameAct = new QAction(tr("&New game..."), this);
	m_newGameAct->setShortcut(QKeySequence("Ctrl+N"));

	m_printGameAct = new QAction(tr("&Print..."), this);

	m_quitGameAct = new QAction(tr("&Quit"), this);
	m_quitGameAct->setShortcut(QKeySequence(tr("Ctrl+Q")));

	m_manageEnginesAct = new QAction(tr("Manage..."), this);

	connect(m_newGameAct, SIGNAL(triggered(bool)), this, SLOT(newGame()));
	connect(m_printGameAct, SIGNAL(triggered(bool)), this, SLOT(printGame()));
	connect(m_quitGameAct, SIGNAL(triggered(bool)), this, SLOT(close()));

	connect (m_manageEnginesAct, SIGNAL(triggered(bool)), this,
		SLOT(manageEngines()));
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

	m_enginesMenu = menuBar()->addMenu(tr("En&gines"));
	m_enginesMenu->addAction(m_manageEnginesAct);

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

	// Engine debug
	QDockWidget* engineDebugDock = new QDockWidget(tr("Engine Debug"), this);
	m_engineDebugTextEdit = new QTextEdit(engineDebugDock);
	m_engineDebugTextEdit->setReadOnly(true);
	engineDebugDock->setWidget(m_engineDebugTextEdit);

	addDockWidget(Qt::BottomDockWidgetArea, engineDebugDock);
}

void MainWindow::writeSettings()
{
	QSettings settings;

	const QList<EngineConfiguration> engines = m_engineConfigurations->configurations();

	settings.beginWriteArray("engines");
	for (int i = 0; i < engines.size(); i++)
	{
		settings.setArrayIndex(i);
		settings.setValue("name", engines.at(i).name());
		settings.setValue("command", engines.at(i).command());
		settings.setValue("working_directory", engines.at(i).workingDirectory());
		settings.setValue("protocol", engines.at(i).protocol());
	}
	settings.endArray();
}

void MainWindow::readSettings()
{
	QSettings settings;

	int size = settings.beginReadArray("engines");
	for (int i = 0; i < size; i++)
	{
		settings.setArrayIndex(i);
		EngineConfiguration config;
		config.setName(settings.value("name").toString());
		config.setCommand(settings.value("command").toString());
		config.setWorkingDirectory(
			settings.value("working_directory").toString());
		config.setProtocol(EngineConfiguration::ChessEngineProtocol(
			settings.value("protocol").toInt()));

		m_engineConfigurations->addConfiguration(config);
	}
	settings.endArray();
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

	const EngineConfiguration engineConfig[2] =
	{
		m_engineConfigurations->configurations().at(dlg.selectedWhiteEngine()),
		m_engineConfigurations->configurations().at(dlg.selectedBlackEngine())
	};
	
	QProcess* engineProcess[2] = { new QProcess(this), new QProcess(this) };

	// Set up working directories for the engines:
	// If user hasn't set any directory, use temp
	for (int i = 0; i < 2; i++)
	{
		QProcess* engine = engineProcess[i];
		const EngineConfiguration& config = engineConfig[i];

		if (config.workingDirectory().isEmpty())
			engine->setWorkingDirectory(QDir::tempPath());
		else
			engine->setWorkingDirectory(config.workingDirectory());
		
		engine->start(config.command());
		if (!engine->waitForStarted())
		{
			qDebug() << "Cannot start the engine process:" << config.command();
			return;
		}
	}

	ChessGame* chessgame = new ChessGame(this);

	connect(chessgame, SIGNAL(moveHappened(const Chess::Move&)),
	        m_visualChessboard, SLOT(makeMove(const Chess::Move&)));

	ChessPlayer* player[2] = { 0, 0 };

	TimeControl tc;
	tc.setTimePerTc(180000);
	tc.setMovesPerTc(40);

	for (int i = 0; i < 2; i++)
	{
		switch (engineConfig[i].protocol())
		{
		case EngineConfiguration::UCI:
			player[i] = new UciEngine(engineProcess[i], chessgame->chessboard(), tc, this);
			break;
		default:
			player[i] = new XboardEngine(engineProcess[i], chessgame->chessboard(), tc, this);
			break;
		}
		
		connect(player[i], SIGNAL(startedThinking(int)),
			m_chessClock[i], SLOT(start(int)));

		connect(player[i], SIGNAL(moveMade(const Chess::Move&)),
			m_chessClock[i], SLOT(stop()));
		
		connect(player[i], SIGNAL(resign()),
		        m_chessClock[i], SLOT(stop()));

		connect(player[i], SIGNAL(debugMessage(const QString&)),
			m_engineDebugTextEdit, SLOT(append(const QString&)));
	}

	chessgame->newGame(player[0], player[1]);
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

void MainWindow::manageEngines()
{
	QList<EngineConfiguration> oldConfigurations =
		m_engineConfigurations->configurations();

	EngineManagementDialog dlg(m_engineConfigurations, this);
	if (dlg.exec() != QDialog::Accepted)
	{
		// Release the engine configurations model and use
		// the old configurations as base for the new model
		delete m_engineConfigurations;
		m_engineConfigurations = new EngineConfigurationModel(oldConfigurations);
	}
}

