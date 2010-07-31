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

#include "mainwindow.h"

#include <QtGui>

#include <board/boardfactory.h>
#include <chessgame.h>
#include <chessplayer.h>
#include <timecontrol.h>
#include <engineconfiguration.h>
#include <enginemanager.h>
#include <enginefactory.h>
#include <engineprocess.h>
#include <humanplayer.h>

#include "cutechessapp.h"
#include "chessboardview.h"
#include "chessboardmodel.h"
#include "movelistmodel.h"
#include "newgamedlg.h"
#include "chessclock.h"
#include "engineconfigurationmodel.h"
#include "enginemanagementdlg.h"
#include "plaintextlog.h"
#include "gamedatabasemodel.h"
#include "gamepropertiesdlg.h"
#include "promotiondlg.h"


MainWindow::MainWindow()
{
	setWindowTitle("Cute Chess");

	QHBoxLayout* clockLayout = new QHBoxLayout();
	for (int i = 0; i < 2; i++)
	{
		m_chessClock[i] = new ChessClock();
		clockLayout->addWidget(m_chessClock[i]);
	}

	m_boardModel = new ChessboardModel(this);
	m_chessboardView = new ChessboardView(this);
	m_chessboardView->setModel(m_boardModel);
	m_moveListModel = new MoveListModel(this);
	connect(m_chessboardView, SIGNAL(humanMove(QModelIndex, QModelIndex)),
		m_boardModel, SLOT(onHumanMove(QModelIndex, QModelIndex)));
	connect(m_boardModel, SIGNAL(promotionNeeded(const Chess::Board*, Chess::Move, QList<int>)),
		this, SLOT(selectPromotion(const Chess::Board*, Chess::Move, QList<int>)));
	connect(this, SIGNAL(promotionMove(Chess::Move, Chess::Side)),
		m_boardModel, SIGNAL(humanMove(Chess::Move,Chess::Side)));

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addLayout(clockLayout);
	mainLayout->addWidget(m_chessboardView);

	// The content margins look stupid when used with dock widgets. Drop the
	// margins except from the top so that the chess clocks have spacing
	mainLayout->setContentsMargins(0,
		style()->pixelMetric(QStyle::PM_LayoutTopMargin), 0, 0);

	QWidget* mainWidget = new QWidget(this);
	mainWidget->setLayout(mainLayout);
	setCentralWidget(mainWidget);
	
	setStatusBar(new QStatusBar());

	createActions();
	createMenus();
	createToolBars();
	createDockWindows();
}

void MainWindow::createActions()
{
	m_newGameAct = new QAction(tr("&New game..."), this);
	m_newGameAct->setShortcut(QKeySequence::New);

	m_gamePropertiesAct = new QAction(tr("P&roperties..."), this);

	m_quitGameAct = new QAction(tr("&Quit"), this);
	m_quitGameAct->setShortcut(QKeySequence::Quit);

	m_manageEnginesAct = new QAction(tr("Manage..."), this);

	connect(m_newGameAct, SIGNAL(triggered(bool)), this, SLOT(newGame()));
	connect(m_gamePropertiesAct, SIGNAL(triggered(bool)), this, SLOT(gameProperties()));
	connect(m_quitGameAct, SIGNAL(triggered(bool)), this, SLOT(close()));

	connect (m_manageEnginesAct, SIGNAL(triggered(bool)), this,
		SLOT(manageEngines()));
}

void MainWindow::createMenus()
{
	m_gameMenu = menuBar()->addMenu(tr("&Game"));
	m_gameMenu->addAction(m_newGameAct);
	m_gameMenu->addSeparator();
	m_gameMenu->addAction(m_gamePropertiesAct);
	m_gameMenu->addSeparator();
	m_gameMenu->addAction(m_quitGameAct);

	m_viewMenu = menuBar()->addMenu(tr("&View"));

	m_enginesMenu = menuBar()->addMenu(tr("En&gines"));
	m_enginesMenu->addAction(m_manageEnginesAct);

	m_helpMenu = menuBar()->addMenu(tr("&Help"));
}

void MainWindow::createToolBars()
{
	// Create tool bars here, use actions from createActions()
	// See: createActions(), QToolBar documentation
}

void MainWindow::createDockWindows()
{
	// Engine debug
	QDockWidget* engineDebugDock = new QDockWidget(tr("Engine Debug"), this);
	m_engineDebugLog = new PlainTextLog(engineDebugDock);
	connect(m_engineDebugLog, SIGNAL(saveLogToFileRequest()), this,
		SLOT(saveLogToFile()));
	engineDebugDock->setWidget(m_engineDebugLog);

	addDockWidget(Qt::BottomDockWidgetArea, engineDebugDock);

	// Game database
	QDockWidget* gameDatabaseDock = new QDockWidget(tr("Game Database"), this);
	QTreeView* gameDatabaseView = new QTreeView(gameDatabaseDock);
	gameDatabaseView->setModel(new GameDatabaseModel(this));
	gameDatabaseView->setAlternatingRowColors(true);
	gameDatabaseView->setRootIsDecorated(false);
	gameDatabaseView->setUniformRowHeights(true);
	gameDatabaseDock->setWidget(gameDatabaseView);

	addDockWidget(Qt::BottomDockWidgetArea, gameDatabaseDock);
	tabifyDockWidget(engineDebugDock, gameDatabaseDock);

	// Move list
	QDockWidget* moveListDock = new QDockWidget(tr("Move List"), this);
	QTreeView* moveListView = new QTreeView(moveListDock);
	moveListView->setModel(m_moveListModel);
	moveListView->setAlternatingRowColors(true);
	moveListView->setRootIsDecorated(false);
	moveListView->header()->setResizeMode(0, QHeaderView::ResizeToContents);
	moveListDock->setWidget(moveListView);

	connect(m_moveListModel, SIGNAL(rowsInserted(QModelIndex, int, int)),
		moveListView, SLOT(scrollToBottom()));

	addDockWidget(Qt::RightDockWidgetArea, moveListDock);

	// Add toggle view actions to the View menu
	m_viewMenu->addAction(moveListDock->toggleViewAction());
	m_viewMenu->addAction(engineDebugDock->toggleViewAction());
	m_viewMenu->addAction(gameDatabaseDock->toggleViewAction());
}

void MainWindow::selectPromotion(const Chess::Board* board,
			      const Chess::Move& move,
			      const QList<int>& promotions)
{
	Q_ASSERT(board != 0);
	Q_ASSERT(!move.isNull());
	Q_ASSERT(promotions.size() > 1);

	PromotionDialog dlg(board, promotions, this);
	if (dlg.exec() != QDialog::Accepted)
		return;

	Chess::Move newMove(move.sourceSquare(),
			    move.targetSquare(),
			    dlg.promotionType());
	emit promotionMove(newMove, board->sideToMove());
}

void MainWindow::newGame()
{
	NewGameDialog dlg(this);
	if (dlg.exec() != QDialog::Accepted)
		return;

	ChessPlayer* player[2] = { 0, 0 };
	QString variant = dlg.selectedVariant();
	Chess::Board* board = Chess::BoardFactory::create(variant);
	Q_ASSERT(board != 0);
	ChessGame* chessgame = new ChessGame(board, this);
	board->setParent(chessgame);
	connect(chessgame, SIGNAL(humanEnabled(bool)),
		m_chessboardView, SLOT(setEnabled(bool)));

	TimeControl tc;
	tc.setTimePerTc(180000);
	tc.setMovesPerTc(40);
	chessgame->setTimeControl(tc);

	for (int i = 0; i < 2; i++)
	{
		Chess::Side side = Chess::Side::Type(i);
		if (dlg.playerType(side) == NewGameDialog::CPU)
		{
			EngineConfiguration config =
				CuteChessApplication::instance()->engineManager()->engines().at(dlg.selectedEngineIndex(side));

			EngineProcess* process = new EngineProcess(this);

			if (config.workingDirectory().isEmpty())
				process->setWorkingDirectory(QDir::tempPath());
			else
				process->setWorkingDirectory(config.workingDirectory());

			process->start(config.command());
			if (!process->waitForStarted())
			{
				qDebug() << "Cannot start the engine process:" << config.command();
				return;
			}
			ChessEngine* engine = EngineFactory::create(config.protocol(), this);
			Q_ASSERT(engine != 0);
			engine->setDevice(process);

			engine->start();
			engine->setName(config.name());
			player[i] = engine;
		}
		else
		{
			player[i] = new HumanPlayer(this);
			connect(m_boardModel, SIGNAL(humanMove(Chess::Move, Chess::Side)),
				player[i], SLOT(onHumanMove(Chess::Move, Chess::Side)));
		}

		chessgame->setPlayer(side, player[i]);
		
		connect(player[i], SIGNAL(startedThinking(int)),
			m_chessClock[i], SLOT(start(int)));
		connect(player[i], SIGNAL(stoppedThinking()),
			m_chessClock[i], SLOT(stop()));
		connect(player[i], SIGNAL(debugMessage(QString)),
			m_engineDebugLog, SLOT(appendPlainText(QString)));
	}
	m_moveListModel->setGame(chessgame);

	m_boardModel->setBoard(board);
	chessgame->start();
}

void MainWindow::gameProperties()
{
	GamePropertiesDialog dlg(this);

	// TODO: Apply game information

	if (dlg.exec() != QDialog::Accepted)
		return;

	// TODO: Use game information
}

void MainWindow::manageEngines()
{
	EngineManagementDialog dlg(this);

	if (dlg.exec() == QDialog::Accepted)
	{
		CuteChessApplication::instance()->engineManager()->setEngines(dlg.engines());

		// We could use QDesktopServices but then this would be inconsistent with
		// CuteChessCoreApp
		QSettings settings;
		QFileInfo fi(settings.fileName());
		CuteChessApplication::instance()->engineManager()->saveEngines(
			CuteChessApplication::instance()->configPath() + QLatin1String("/engines.json"));
	}
}

void MainWindow::saveLogToFile()
{
	PlainTextLog* log = qobject_cast<PlainTextLog*>(QObject::sender());
	Q_ASSERT(log != 0);

	const QString fileName = QFileDialog::getSaveFileName(this, tr("Save Log"),
		QString(), tr("Text Files (*.txt);;All Files (*.*)"));

	if (fileName.isEmpty())
		return;

	QFile file(fileName);
	if (!file.open(QFile::WriteOnly | QFile::Text))
	{
		QFileInfo fileInfo(file);

		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.setWindowTitle("Cute Chess");

		switch (file.error())
		{
			case QFile::OpenError:
			case QFile::PermissionsError:
				msgBox.setText(
					tr("The file \"%1\" could not be saved because "
					   "of insufficient privileges.")
					.arg(fileInfo.fileName()));

				msgBox.setInformativeText(
					tr("Try selecting a location where you have "
					   "the permissions to create files."));
			break;

			case QFile::TimeOutError:
				msgBox.setText(
					tr("The file \"%1\" could not be saved because "
					   "the operation timed out.")
					.arg(fileInfo.fileName()));

				msgBox.setInformativeText(
					tr("Try saving the file to a local or another "
					   "network disk."));
			break;

			default:
				msgBox.setText(tr("The file \"%1\" could not be saved.")
					.arg(fileInfo.fileName()));

				msgBox.setInformativeText(file.errorString());
			break;
		}
		msgBox.exec();

		return;
	}

	QTextStream out(&file);
	out << log->toPlainText();
}
