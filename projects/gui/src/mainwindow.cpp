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
#include "autoverticalscroller.h"

MainWindow::MainWindow(ChessGame* game)
	: m_game(game)
{
	Q_ASSERT(m_game != 0);

	setAttribute(Qt::WA_DeleteOnClose, true);

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

	// Attach the game to the GUI
	connect(m_game, SIGNAL(humanEnabled(bool)),
			m_chessboardView, SLOT(setEnabled(bool)));

	for (int i = 0; i < 2; i++)
	{
		Chess::Side side = Chess::Side::Type(i);
		connect(m_game->player(side), SIGNAL(startedThinking(int)),
			m_chessClock[i], SLOT(start(int)));
		connect(m_game->player(side), SIGNAL(stoppedThinking()),
			m_chessClock[i], SLOT(stop()));
		connect(m_game->player(side), SIGNAL(debugMessage(QString)),
			m_engineDebugLog, SLOT(appendPlainText(QString)));

		if (m_game->player(side)->isHuman())
			connect(m_boardModel, SIGNAL(humanMove(Chess::Move, Chess::Side)),
				m_game->player(side), SLOT(onHumanMove(Chess::Move, Chess::Side)));
	}

	m_moveListModel->setGame(m_game);
	m_boardModel->setBoard(m_game->board());
	m_game->start();

	updateWindowTitle();
}

void MainWindow::createActions()
{
	m_newGameAct = new QAction(tr("&New..."), this);
	m_newGameAct->setShortcut(QKeySequence::New);

	m_closeGameAct = new QAction(tr("&Close"), this);
	#ifdef Q_WS_WIN
	m_closeGameAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
	#else
	m_closeGameAct->setShortcut(QKeySequence::Close);
	#endif

	m_saveGameAct = new QAction(tr("&Save"), this);
	m_saveGameAct->setShortcut(QKeySequence::Save);

	m_saveGameAsAct = new QAction(tr("Save &As..."), this);
	m_saveGameAsAct->setShortcut(QKeySequence::SaveAs);

	m_gamePropertiesAct = new QAction(tr("P&roperties..."), this);

	m_quitGameAct = new QAction(tr("&Quit"), this);
	#ifdef Q_OS_WIN
	m_quitGameAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
	#else
	m_quitGameAct->setShortcut(QKeySequence::Quit);
	#endif

	m_manageEnginesAct = new QAction(tr("Manage..."), this);

	connect(m_newGameAct, SIGNAL(triggered(bool)), this, SLOT(newGame()));
	connect(m_closeGameAct, SIGNAL(triggered(bool)), this, SLOT(close()));
	connect(m_saveGameAct, SIGNAL(triggered(bool)), this, SLOT(save()));
	connect(m_saveGameAsAct, SIGNAL(triggered(bool)), this, SLOT(saveAs()));
	connect(m_gamePropertiesAct, SIGNAL(triggered(bool)), this, SLOT(gameProperties()));
	connect(m_quitGameAct, SIGNAL(triggered(bool)), qApp, SLOT(closeAllWindows()));

	connect(m_manageEnginesAct, SIGNAL(triggered(bool)), this,
		SLOT(manageEngines()));
}

void MainWindow::createMenus()
{
	m_gameMenu = menuBar()->addMenu(tr("&Game"));
	m_gameMenu->addAction(m_newGameAct);
	m_gameMenu->addSeparator();
	m_gameMenu->addAction(m_closeGameAct);
	m_gameMenu->addAction(m_saveGameAct);
	m_gameMenu->addAction(m_saveGameAsAct);
	m_gameMenu->addAction(m_gamePropertiesAct);
	m_gameMenu->addSeparator();
	m_gameMenu->addAction(m_quitGameAct);

	m_viewMenu = menuBar()->addMenu(tr("&View"));

	m_enginesMenu = menuBar()->addMenu(tr("En&gines"));
	m_enginesMenu->addAction(m_manageEnginesAct);

	m_windowMenu = menuBar()->addMenu(tr("&Window"));
	connect(m_windowMenu, SIGNAL(aboutToShow()), this,
		SLOT(onWindowMenuAboutToShow()));

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

	AutoVerticalScroller* moveListScroller =
		new AutoVerticalScroller(moveListView, this);
	Q_UNUSED(moveListScroller);

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
	CuteChessApplication::instance()->newGameWindow();
}

void MainWindow::gameProperties()
{
	GamePropertiesDialog dlg(this);

	dlg.setEvent(m_game->pgn()->event());
	dlg.setSite(m_game->pgn()->site());
	dlg.setRound(m_game->pgn()->round());

	if (dlg.exec() != QDialog::Accepted)
		return;

	m_game->pgn()->setEvent(dlg.event());
	m_game->pgn()->setSite(dlg.site());
	m_game->pgn()->setRound(dlg.round());

	setWindowModified(true);
}

void MainWindow::manageEngines()
{
	EngineManagementDialog dlg(this);

	if (dlg.exec() == QDialog::Accepted)
	{
		CuteChessApplication::instance()->engineManager()->setEngines(dlg.engines());
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

void MainWindow::onWindowMenuAboutToShow()
{
	m_windowMenu->clear();

	const QList<MainWindow*> gameWindows =
		CuteChessApplication::instance()->gameWindows();

	for (int i = 0; i < gameWindows.size(); i++)
	{
		MainWindow* gameWindow = gameWindows.at(i);

		QAction* showWindowAction = m_windowMenu->addAction(
			gameWindow->windowListTitle(), this, SLOT(showGameWindow()));
		showWindowAction->setData(i);
		showWindowAction->setCheckable(true);

		if (gameWindow == this)
			showWindowAction->setChecked(true);
	}
}

void MainWindow::showGameWindow()
{
	if (QAction* action = qobject_cast<QAction*>(sender()))
		CuteChessApplication::instance()->showGameWindow(action->data().toInt());
}

void MainWindow::updateWindowTitle()
{
	// setWindowTitle() requires "[*]" (see docs)
	setWindowTitle(genericWindowTitle() + QLatin1String("[*]"));
}

QString MainWindow::windowListTitle() const
{
	#ifndef Q_WS_MAC
	if (isWindowModified())
		return genericWindowTitle() + QLatin1String("*");
	#endif

	return genericWindowTitle();
}

QString MainWindow::genericWindowTitle() const
{
	return QString("%1 - %2")
		.arg(m_game->player(Chess::Side::White)->name())
			.arg(m_game->player(Chess::Side::Black)->name());
}

bool MainWindow::save()
{
	if (m_currentFile.isEmpty())
		return saveAs();

	return saveGame(m_currentFile);
}

bool MainWindow::saveAs()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save Game"),
		QString(), tr("Portable Game Notation (*.pgn);;All Files (*.*)"));

	if (fileName.isEmpty())
		return false;

	return saveGame(fileName);
}

bool MainWindow::saveGame(const QString& fileName)
{
	if (!m_game->pgn()->write(fileName))
		return false;

	m_currentFile = fileName;
	setWindowModified(false);

	return true;
}
