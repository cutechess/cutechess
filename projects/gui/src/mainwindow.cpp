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

#include <QtGui>

#include <chessgame.h>
#include <chessplayer.h>
#include <timecontrol.h>
#include <engineconfiguration.h>
#include <enginefactory.h>

#include "mainwindow.h"
#include "chessboardview.h"
#include "chessboardmodel.h"
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

	m_boardModel = new ChessboardModel(this);
	m_chessboardView = new ChessboardView(this);
	m_chessboardView->setModel(m_boardModel);
	connect(m_boardModel, SIGNAL(moveMade(const QModelIndex&, const QModelIndex&)),
		m_chessboardView, SLOT(onMoveMade(const QModelIndex&, const QModelIndex&)));

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addLayout(clockLayout);
	mainLayout->addWidget(m_chessboardView);

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

void MainWindow::createActions()
{
	m_newGameAct = new QAction(tr("&New game..."), this);
	m_newGameAct->setShortcut(QKeySequence("Ctrl+N"));

	m_printGameAct = new QAction(tr("&Print..."), this);

	m_quitGameAct = new QAction(tr("&Quit"), this);
	m_quitGameAct->setShortcut(QKeySequence(tr("Ctrl+Q")));

	m_manageEnginesAct = new QAction(tr("Manage..."), this);

	m_openConfigurationFileAct = new QAction("Open configuration file", this);

	connect(m_newGameAct, SIGNAL(triggered(bool)), this, SLOT(newGame()));
	connect(m_printGameAct, SIGNAL(triggered(bool)), this, SLOT(printGame()));
	connect(m_quitGameAct, SIGNAL(triggered(bool)), this, SLOT(close()));

	connect (m_manageEnginesAct, SIGNAL(triggered(bool)), this,
		SLOT(manageEngines()));

	connect(m_openConfigurationFileAct, SIGNAL(triggered(bool)), this,
		SLOT(openConfigurationFile()));
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
	m_debugMenu->addAction(m_openConfigurationFileAct);
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
		config.setProtocol(ChessEngine::Protocol(
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
		m_engineConfigurations->configuration(dlg.selectedWhiteEngine()),
		m_engineConfigurations->configuration(dlg.selectedBlackEngine())
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

	ChessGame* chessgame = new ChessGame(Chess::Variant::Standard, this);

	ChessPlayer* player[2] = { 0, 0 };

	TimeControl tc;
	tc.setTimePerTc(180000);
	tc.setMovesPerTc(40);

	for (int i = 0; i < 2; i++)
	{
		player[i] = EngineFactory::createEngine(engineConfig[i].protocol(),
			engineProcess[i], this);
		
		player[i]->setName(engineConfig[i].name());
		player[i]->setTimeControl(tc);
		chessgame->setPlayer((Chess::Side)i, player[i]);
		
		connect(player[i], SIGNAL(startedThinking(int)),
			m_chessClock[i], SLOT(start(int)));
		connect(player[i], SIGNAL(moveMade(const Chess::Move&)),
			m_chessClock[i], SLOT(stop()));
		connect(chessgame, SIGNAL(gameEnded()),
		        m_chessClock[i], SLOT(stop()));
		connect(player[i], SIGNAL(debugMessage(const QString&)),
			m_engineDebugTextEdit, SLOT(append(const QString&)));
	}

	m_boardModel->setBoard(chessgame->board());
	chessgame->start();
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

	if (dlg.exec() == QDialog::Accepted)
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

		// Make sure that the settings are flushed to disk now
		settings.sync();
	}
	else
	{
		// Release the engine configurations model and use
		// the old configurations as base for the new model
		delete m_engineConfigurations;
		m_engineConfigurations = new EngineConfigurationModel(oldConfigurations);
	}
}

void MainWindow::openConfigurationFile()
{
	QSettings settings;

	if (QFile::exists(settings.fileName()))
		QDesktopServices::openUrl(QUrl("file://" + settings.fileName()));
	else
		qDebug() << "Cannot open configuration file:" << settings.fileName();
}

