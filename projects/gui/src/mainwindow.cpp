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
#include <pgnstream.h>
#include <tournament.h>

#include "cutechessapp.h"
#include "boardview/boardscene.h"
#include "boardview/boardview.h"
#include "movelist.h"
#include "newgamedlg.h"
#include "newtournamentdialog.h"
#include "chessclock.h"
#include "engineconfigurationmodel.h"
#include "enginemanagementdlg.h"
#include "plaintextlog.h"
#include "gamepropertiesdlg.h"
#include "autoverticalscroller.h"
#include "gamedatabasemanager.h"
#include "pgntagsmodel.h"

MainWindow::TabData::TabData(ChessGame* game, Tournament* tournament)
	: id(game),
	  game(game),
	  pgn(game->pgn()),
	  tournament(tournament)
{
}

MainWindow::MainWindow(ChessGame* game)
	: m_game(0),
	  m_closing(false)
{
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

	m_moveList = new MoveList(this);
	m_tagsModel = new PgnTagsModel(this);

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addLayout(clockLayout);
	mainLayout->addWidget(m_boardView);

	// The content margins look stupid when used with dock widgets
	mainLayout->setContentsMargins(0, 0, 0, 0);

	QWidget* mainWidget = new QWidget(this);
	mainWidget->setLayout(mainLayout);
	setCentralWidget(mainWidget);

	setStatusBar(new QStatusBar());

	createActions();
	createMenus();
	createToolBars();
	createDockWindows();

	addGame(game);
}

MainWindow::~MainWindow()
{
}

void MainWindow::createActions()
{
	m_newGameAct = new QAction(tr("&New..."), this);
	m_newGameAct->setShortcut(QKeySequence::New);

	m_closeGameAct = new QAction(tr("&Close"), this);
	#ifdef Q_OS_WIN32
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
	#ifdef Q_OS_WIN32
	m_quitGameAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
	#else
	m_quitGameAct->setShortcut(QKeySequence::Quit);
	#endif

	m_newTournamentAct = new QAction(tr("New..."), this);
	m_stopTournamentAct = new QAction(tr("Stop"), this);

	m_manageEnginesAct = new QAction(tr("Manage..."), this);

	m_showGameDatabaseWindowAct = new QAction(tr("&Game Database"), this);

	m_showGameWallAct = new QAction(tr("Game Wall"), this);

	connect(m_newGameAct, SIGNAL(triggered(bool)), this, SLOT(newGame()));
	connect(m_closeGameAct, SIGNAL(triggered(bool)), this, SLOT(close()));
	connect(m_saveGameAct, SIGNAL(triggered(bool)), this, SLOT(save()));
	connect(m_saveGameAsAct, SIGNAL(triggered(bool)), this, SLOT(saveAs()));
	connect(m_gamePropertiesAct, SIGNAL(triggered(bool)), this, SLOT(gameProperties()));
	connect(m_quitGameAct, SIGNAL(triggered(bool)), qApp, SLOT(closeAllWindows()));

	connect(m_newTournamentAct, SIGNAL(triggered()), this, SLOT(newTournament()));

	connect(m_manageEnginesAct, SIGNAL(triggered(bool)), this,
		SLOT(manageEngines()));

	connect(m_showGameDatabaseWindowAct, SIGNAL(triggered(bool)),
		CuteChessApplication::instance(), SLOT(showGameDatabaseDialog()));

	connect(m_showGameWallAct, SIGNAL(triggered()),
		CuteChessApplication::instance(), SLOT(showGameWall()));
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

	m_tournamentMenu = menuBar()->addMenu(tr("&Tournament"));
	m_tournamentMenu->addAction(m_newTournamentAct);
	m_tournamentMenu->addAction(m_stopTournamentAct);
	m_stopTournamentAct->setEnabled(false);

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

	m_tabBar = new QTabBar();
	m_tabBar->setDocumentMode(true);
	m_tabBar->setTabsClosable(true);
	m_tabBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	connect(m_tabBar, SIGNAL(currentChanged(int)),
		this, SLOT(onTabChanged(int)));
	connect(m_tabBar, SIGNAL(tabCloseRequested(int)),
		this, SLOT(onTabCloseRequested(int)));

	QToolBar* toolBar = new QToolBar(tr("Game Tabs"));
	toolBar->setFloatable(false);
	toolBar->setMovable(false);
	toolBar->setAllowedAreas(Qt::TopToolBarArea);
	toolBar->addWidget(m_tabBar);
	addToolBar(toolBar);
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
	QDockWidget* moveListDock = new QDockWidget(tr("Moves"), this);
	moveListDock->setWidget(m_moveList);

	addDockWidget(Qt::RightDockWidgetArea, moveListDock);

	// Tags
	QDockWidget* tagsDock = new QDockWidget(tr("Tags"), this);
	QTreeView* tagsView = new QTreeView(tagsDock);
	tagsView->setModel(m_tagsModel);
	tagsView->setAlternatingRowColors(true);
	tagsView->setRootIsDecorated(false);
	tagsDock->setWidget(tagsView);

	addDockWidget(Qt::RightDockWidgetArea, tagsDock);

	tabifyDockWidget(moveListDock, tagsDock);
	moveListDock->raise();

	// Add toggle view actions to the View menu
	m_viewMenu->addAction(moveListDock->toggleViewAction());
	m_viewMenu->addAction(tagsDock->toggleViewAction());
	m_viewMenu->addAction(engineDebugDock->toggleViewAction());
}

void MainWindow::addGame(ChessGame* game)
{
	Tournament* tournament = qobject_cast<Tournament*>(QObject::sender());
	TabData tab(game, tournament);

	if (tournament)
	{
		int index = tabIndex(tournament, true);
		if (index != -1)
		{
			delete m_tabs[index].pgn;
			m_tabs[index] = tab;

			m_tabBar->setTabText(index, genericTitle(tab));
			if (!m_closing && m_tabBar->currentIndex() == index)
				setCurrentGame(tab);

			return;
		}
	}

	m_tabs.append(tab);
	m_tabBar->setCurrentIndex(m_tabBar->addTab(genericTitle(tab)));
}

void MainWindow::removeGame(int index)
{
	Q_ASSERT(index != -1);

	m_tabs.removeAt(index);
	m_tabBar->removeTab(index);
}

void MainWindow::destroyGame(ChessGame* game)
{
	Q_ASSERT(game != 0);

	int index = tabIndex(game);
	Q_ASSERT(index != -1);
	TabData tab = m_tabs.at(index);

	removeGame(index);

	if (tab.tournament == 0)
		game->deleteLater();
	delete tab.pgn;

	if (m_tabs.isEmpty())
		close();
}

void MainWindow::setCurrentGame(const TabData& gameData)
{
	if (gameData.game == m_game && m_game != 0)
		return;

	if (m_game != 0)
	{
		disconnect(m_game, 0, m_boardScene, 0);
		disconnect(m_game, 0, m_boardView, 0);
		disconnect(m_game, 0, m_moveList, 0);

		for (int i = 0; i < 2; i++)
		{
			ChessPlayer* player(m_game->player(Chess::Side::Type(i)));
			if (player != 0)
			{
				disconnect(m_boardScene, 0, player, 0);
				disconnect(player, 0, m_engineDebugLog, 0);
				disconnect(player, 0, m_chessClock[0], 0);
				disconnect(player, 0, m_chessClock[1], 0);
			}
		}

		ChessGame* tmp = m_game;
		m_game = 0;

		// QObject::disconnect() is not atomic, so we need to flush
		// all pending events from the previous game before switching
		// to the next one.
		tmp->lockThread();
		CuteChessApplication::processEvents();
		tmp->unlockThread();

		// If the call to CuteChessApplication::processEvents() caused
		// a new game to be selected as the current game, then our
		// work here is done.
		if (m_game != 0)
			return;
	}

	m_game = gameData.game;

	if (m_game != 0)
	{
		m_game->lockThread();
		connect(m_game, SIGNAL(fenChanged(QString)),
			m_boardScene, SLOT(setFenString(QString)));
		connect(m_game, SIGNAL(moveMade(Chess::GenericMove, QString, QString)),
			m_boardScene, SLOT(makeMove(Chess::GenericMove)));
		connect(m_game, SIGNAL(humanEnabled(bool)),
			m_boardView, SLOT(setEnabled(bool)));
	}

	m_engineDebugLog->clear();

	m_moveList->setGame(m_game, gameData.pgn);
	m_boardScene->setBoard(gameData.pgn->createBoard());
	m_boardScene->populate();

	foreach (const PgnGame::MoveData& moveData, gameData.pgn->moves())
		m_boardScene->makeMove(moveData.move);

	if (m_game == 0)
	{
		for (int i = 0; i < 2; i++)
		{
			ChessClock* clock(m_chessClock[i]);
			clock->stop();
			clock->setInfiniteTime(true);
			clock->setPlayerName(gameData.pgn->playerName(Chess::Side::Type(i)));
		}

		m_boardView->setEnabled(false);
		updateWindowTitle();

		return;
	}

	for (int i = 0; i < 2; i++)
	{
		ChessPlayer* player(m_game->player(Chess::Side::Type(i)));

		if (player->isHuman())
			connect(m_boardScene, SIGNAL(humanMove(Chess::GenericMove, Chess::Side)),
				player, SLOT(onHumanMove(Chess::GenericMove, Chess::Side)));
		connect(player, SIGNAL(debugMessage(QString)),
			m_engineDebugLog, SLOT(appendPlainText(QString)));

		ChessClock* clock(m_chessClock[i]);

		clock->stop();
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

	m_boardView->setEnabled(!m_game->isFinished() &&
				m_game->playerToMove()->isHuman());

	updateWindowTitle();
	m_game->unlockThread();
}

int MainWindow::tabIndex(ChessGame* game) const
{
	Q_ASSERT(game != 0);

	for (int i = 0; i < m_tabs.size(); i++)
	{
		if (m_tabs.at(i).id == game)
			return i;
	}

	return -1;
}

int MainWindow::tabIndex(Tournament* tournament, bool freeTab) const
{
	Q_ASSERT(tournament != 0);

	for (int i = 0; i < m_tabs.size(); i++)
	{
		const TabData& tab = m_tabs.at(i);

		if (tab.tournament == tournament
		&&  (!freeTab || (tab.game == 0 || tab.game->isFinished())))
			return i;
	}

	return -1;
}

void MainWindow::onTabChanged(int index)
{
	if (index == -1 || m_closing)
		m_game = 0;
	else
		setCurrentGame(m_tabs.at(index));
}

void MainWindow::onTabCloseRequested(int index)
{
	const TabData& tab = m_tabs.at(index);

	if (tab.game == 0)
	{
		delete tab.pgn;
		removeGame(index);

		if (m_tabs.isEmpty())
			close();

		return;
	}

	if (tab.game->isFinished())
		destroyGame(tab.game);
	else
	{
		connect(tab.game, SIGNAL(finished(ChessGame*)),
			this, SLOT(destroyGame(ChessGame*)));
		QMetaObject::invokeMethod(tab.game, "stop", Qt::QueuedConnection);
	}
}

void MainWindow::newGame()
{
	EngineManager* engineManager = CuteChessApplication::instance()->engineManager();
	NewGameDialog dlg(engineManager, this);
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
			int index = dlg.selectedEngineIndex(side);
			player[i] = new EngineBuilder(engineManager->engineAt(index));
		}
		else
		{
			player[i] = new HumanBuilder(CuteChessApplication::userName());
			if (side == Chess::Side::White)
				game->pause();
		}
	}

	// Start the game in a new tab
	connect(game, SIGNAL(started(ChessGame*)),
		this, SLOT(addGame(ChessGame*)));
	CuteChessApplication::instance()->gameManager()->newGame(game,
		player[0], player[1]);
}

void MainWindow::newTournament()
{
	NewTournamentDialog dlg(CuteChessApplication::instance()->engineManager(), this);
	if (dlg.exec() != QDialog::Accepted)
		return;

	GameManager* manager = CuteChessApplication::instance()->gameManager();

	Tournament* t = dlg.createTournament(manager);
	connect(t, SIGNAL(finished()),
		this, SLOT(onTournamentFinished()));
	connect(t, SIGNAL(gameStarted(ChessGame*, int, int, int)),
		this, SLOT(addGame(ChessGame*)));
	t->start();

	connect(m_stopTournamentAct, SIGNAL(triggered()), t, SLOT(stop()));
	m_stopTournamentAct->setEnabled(true);
}

void MainWindow::onTournamentFinished()
{
	Tournament* tournament = qobject_cast<Tournament*>(QObject::sender());
	Q_ASSERT(tournament != 0);

	QMessageBox::information(this,
				 tr("Tournament finished"),
				 tr("Tournament \"%1\" is finished")
					.arg(tournament->name()));
	tournament->deleteLater();
	m_stopTournamentAct->setEnabled(false);
}

void MainWindow::gameProperties()
{
	GamePropertiesDialog dlg(this);
	PgnGame* pgn = m_tabs.at(m_tabBar->currentIndex()).pgn;

	lockCurrentGame();
	dlg.setWhite(pgn->playerName(Chess::Side::White));
	dlg.setBlack(pgn->playerName(Chess::Side::Black));
	dlg.setEvent(pgn->event());
	dlg.setSite(pgn->site());
	dlg.setRound(pgn->round());
	unlockCurrentGame();

	if (dlg.exec() != QDialog::Accepted)
		return;

	lockCurrentGame();
	pgn->setPlayerName(Chess::Side::White, dlg.white());
	pgn->setPlayerName(Chess::Side::Black, dlg.black());
	pgn->setEvent(dlg.event());
	pgn->setSite(dlg.site());
	pgn->setRound(dlg.round());

	updateWindowTitle();
	unlockCurrentGame();

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

	m_windowMenu->addAction(m_showGameWallAct);
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
	const TabData& gameData(m_tabs.at(m_tabBar->currentIndex()));
	setWindowTitle(genericTitle(gameData) + QLatin1String("[*]"));
}

QString MainWindow::windowListTitle() const
{
	const TabData& gameData(m_tabs.at(m_tabBar->currentIndex()));

#ifndef Q_OS_MAC
	if (isWindowModified())
		return genericTitle(gameData) + QLatin1String("*");
	#endif

	return genericTitle(gameData);
}

QString MainWindow::genericTitle(const TabData& gameData) const
{
	if (gameData.game != 0)
		return tr("%1 vs %2")
			.arg(gameData.game->player(Chess::Side::White)->name())
			.arg(gameData.game->player(Chess::Side::Black)->name());

	return tr("%1 vs %2")
		.arg(gameData.pgn->playerName(Chess::Side::White))
		.arg(gameData.pgn->playerName(Chess::Side::Black));
}

void MainWindow::lockCurrentGame()
{
	if (m_game != 0)
		m_game->lockThread();
}

void MainWindow::unlockCurrentGame()
{
	if (m_game != 0)
		m_game->unlockThread();
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
	lockCurrentGame();
	bool ok = m_tabs.at(m_tabBar->currentIndex()).pgn->write(fileName);
	unlockCurrentGame();

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
		m_closing = true;
		if (m_tabs.isEmpty())
			event->accept();
		else
		{
			for (int i = m_tabs.size() - 1; i >= 0; i--)
				onTabCloseRequested(i);

			if (m_tabs.isEmpty())
				event->accept();
			else
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
