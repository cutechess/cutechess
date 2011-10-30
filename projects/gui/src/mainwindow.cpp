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
#include <timecontrol.h>
#include <engineconfiguration.h>
#include <enginemanager.h>
#include <gamemanager.h>
#include <playerbuilder.h>
#include <humanbuilder.h>
#include <enginebuilder.h>
#include <chessplayer.h>

#include "cutechessapp.h"
#include "boardview/boardscene.h"
#include "boardview/boardview.h"
#include "movelistmodel.h"
#include "newgamedlg.h"
#include "chessclock.h"
#include "engineconfigurationmodel.h"
#include "enginemanagementdlg.h"
#include "plaintextlog.h"
#include "gamepropertiesdlg.h"
#include "autoverticalscroller.h"
#include "gamedatabasemanager.h"

MainWindow::MainWindow(ChessGame* game)
	: m_game(game),
	  m_pgn(0)
{
	Q_ASSERT(m_game != 0);

	setAttribute(Qt::WA_DeleteOnClose, true);

	QHBoxLayout* clockLayout = new QHBoxLayout();
	for (int i = 0; i < 2; i++)
	{
		m_chessClock[i] = new ChessClock();
		clockLayout->addWidget(m_chessClock[i]);

		Chess::Side side = Chess::Side::Type(i);
		m_chessClock[i]->setPlayerName(side.toString());
	}
	clockLayout->insertSpacing(1, 20);

	m_boardScene = new BoardScene(this);
	m_boardView = new BoardView(m_boardScene, this);

	m_moveListModel = new MoveListModel(this);

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addLayout(clockLayout);
	mainLayout->addWidget(m_boardView);

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

	m_game->lockThread();
	connect(m_game, SIGNAL(fenChanged(QString)),
		m_boardScene, SLOT(setFenString(QString)));
	connect(m_game, SIGNAL(moveMade(Chess::GenericMove, QString, QString)),
		m_boardScene, SLOT(makeMove(Chess::GenericMove)));
	connect(m_game, SIGNAL(humanEnabled(bool)),
			m_boardView, SLOT(setEnabled(bool)));

	for (int i = 0; i < 2; i++)
	{
		ChessPlayer* player(m_game->player(Chess::Side::Type(i)));

		if (player->isHuman())
			connect(m_boardScene, SIGNAL(humanMove(Chess::GenericMove, Chess::Side)),
				player, SLOT(onHumanMove(Chess::GenericMove, Chess::Side)));
		connect(player, SIGNAL(debugMessage(QString)),
			m_engineDebugLog, SLOT(appendPlainText(QString)));

		ChessClock* clock(m_chessClock[i]);

		clock->setPlayerName(player->name());
		connect(player, SIGNAL(nameChanged(QString)),
			clock, SLOT(setPlayerName(QString)));

		clock->setInfiniteTime(player->timeControl()->isInfinite());

		if (player->state() == ChessPlayer::Thinking)
			clock->start(player->timeControl()->activeTimeLeft());
		else
			clock->setTime(player->timeControl()->timeLeft());

		connect(player, SIGNAL(startedThinking(int)),
			clock, SLOT(start(int)));
		connect(player, SIGNAL(stoppedThinking()),
			clock, SLOT(stop()));
	}

	m_pgn = m_game->pgn();
	m_moveListModel->setGame(m_game);
	m_boardScene->setBoard(m_pgn->createBoard());
	m_boardScene->populate();

	updateWindowTitle();
	m_game->unlockThread();
}

MainWindow::~MainWindow()
{
	m_game->deleteLater();
	delete m_pgn;
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

	m_importGameAct = new QAction(tr("Import..."), this);

	m_quitGameAct = new QAction(tr("&Quit"), this);
	#ifdef Q_OS_WIN
	m_quitGameAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
	#else
	m_quitGameAct->setShortcut(QKeySequence::Quit);
	#endif

	m_manageEnginesAct = new QAction(tr("Manage..."), this);

	m_showGameDatabaseWindowAct = new QAction(tr("&Game Database"), this);

	connect(m_newGameAct, SIGNAL(triggered(bool)), this, SLOT(newGame()));
	connect(m_closeGameAct, SIGNAL(triggered(bool)), this, SLOT(close()));
	connect(m_saveGameAct, SIGNAL(triggered(bool)), this, SLOT(save()));
	connect(m_saveGameAsAct, SIGNAL(triggered(bool)), this, SLOT(saveAs()));
	connect(m_gamePropertiesAct, SIGNAL(triggered(bool)), this, SLOT(gameProperties()));
	connect(m_importGameAct, SIGNAL(triggered(bool)), this, SLOT(import()));
	connect(m_quitGameAct, SIGNAL(triggered(bool)), qApp, SLOT(closeAllWindows()));

	connect(m_manageEnginesAct, SIGNAL(triggered(bool)), this,
		SLOT(manageEngines()));

	connect(m_showGameDatabaseWindowAct, SIGNAL(triggered(bool)),
		CuteChessApplication::instance(), SLOT(showGameDatabaseDialog()));
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
	m_gameMenu->addAction(m_importGameAct);
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
}

void MainWindow::newGame()
{
	NewGameDialog dlg(this);
	if (dlg.exec() != QDialog::Accepted)
		return;

	PlayerBuilder* player[2] = { 0, 0 };
	ChessGame* game = new ChessGame(Chess::BoardFactory::create(dlg.selectedVariant()),
		new PgnGame());

	game->setTimeControl(dlg.timeControl());

	for (int i = 0; i < 2; i++)
	{
		Chess::Side side = Chess::Side::Type(i);

		if (dlg.playerType(side) == NewGameDialog::CPU)
		{
			EngineConfiguration config =
				CuteChessApplication::instance()->engineManager()->engines().at(dlg.selectedEngineIndex(side));

			player[i] = new EngineBuilder(config);
		}
		else
			player[i] = new HumanBuilder();
	}

	CuteChessApplication::instance()->gameManager()->newGame(game,
		player[0], player[1]);
}

void MainWindow::gameProperties()
{
	GamePropertiesDialog dlg(this);

	m_game->lockThread();
	dlg.setWhite(m_pgn->playerName(Chess::Side::White));
	dlg.setBlack(m_pgn->playerName(Chess::Side::Black));
	dlg.setEvent(m_pgn->event());
	dlg.setSite(m_pgn->site());
	dlg.setRound(m_pgn->round());
	m_game->unlockThread();

	if (dlg.exec() != QDialog::Accepted)
		return;

	m_game->lockThread();
	m_pgn->setPlayerName(Chess::Side::White, dlg.white());
	m_pgn->setPlayerName(Chess::Side::Black, dlg.black());
	m_pgn->setEvent(dlg.event());
	m_pgn->setSite(dlg.site());
	m_pgn->setRound(dlg.round());

	updateWindowTitle();
	m_game->unlockThread();

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

	m_windowMenu->addAction(m_showGameDatabaseWindowAct);
	m_windowMenu->addSeparator();

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
		.arg(m_pgn->playerName(Chess::Side::White))
		.arg(m_pgn->playerName(Chess::Side::Black));
}

bool MainWindow::save()
{
	if (m_currentFile.isEmpty())
		return saveAs();

	return saveGame(m_currentFile);
}

bool MainWindow::saveAs()
{
	const QString fileName = QFileDialog::getSaveFileName(this, tr("Save Game"),
		QString(), tr("Portable Game Notation (*.pgn);;All Files (*.*)"));

	if (fileName.isEmpty())
		return false;

	return saveGame(fileName);
}

bool MainWindow::saveGame(const QString& fileName)
{
	m_game->lockThread();
	bool ok = m_pgn->write(fileName);
	m_game->unlockThread();

	if (!ok)
		return false;

	m_currentFile = fileName;
	setWindowModified(false);

	return true;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	if (askToSave())
	{
		if (m_game->isFinished())
			event->accept();
		else
		{
			connect(m_game, SIGNAL(finished()), this, SLOT(close()));
			QMetaObject::invokeMethod(m_game, "stop", Qt::QueuedConnection);
			event->ignore();
		}
	}
	else
		event->ignore();
}

bool MainWindow::askToSave()
{
	if (isWindowModified())
	{
		QMessageBox::StandardButton result;
		result = QMessageBox::warning(this, QApplication::applicationName(),
			tr("The game was modified.\nDo you want to save your changes?"),
				QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

		if (result == QMessageBox::Save)
			return save();
		else if (result == QMessageBox::Cancel)
			return false;
	}
	return true;
}

void MainWindow::import()
{
	const QString fileName = QFileDialog::getOpenFileName(this, tr("Import Game"),
		QString(), tr("Portable Game Notation (*.pgn);;All Files (*.*)"));

	if (fileName.isEmpty())
		return;

	CuteChessApplication::instance()->gameDatabaseManager()->importPgnFile(fileName);
}
