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

#include "cutechessapp.h"

#include <QCoreApplication>
#include <QDir>
#include <QTime>
#include <QSettings>
#include <QFileInfo>

#include <enginemanager.h>
#include <gamemanager.h>
#include <board/boardfactory.h>
#include <chessgame.h>
#include <timecontrol.h>
#include <humanbuilder.h>

#include "mainwindow.h"
#include "gamedatabasedlg.h"
#include "gamedatabasemanager.h"
#include "importprogressdlg.h"
#include "pgnimporter.h"


CuteChessApplication::CuteChessApplication(int& argc, char* argv[])
	: QApplication(argc, argv),
	  m_engineManager(0),
	  m_gameManager(0),
	  m_gameDatabaseManager(0),
	  m_gameDatabaseDialog(0)
{
	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

	QCoreApplication::setOrganizationName("cutechess");
	QCoreApplication::setOrganizationDomain("cutechess.org");
	QCoreApplication::setApplicationName("cutechess");

	// Use Ini format on all platforms
	QSettings::setDefaultFormat(QSettings::IniFormat);

	// Load the engines
	engineManager()->loadEngines(configPath() + QLatin1String("/engines.json"));

	// Read the game database state
	gameDatabaseManager()->readState(configPath() + QLatin1String("/gamedb.bin"));

	connect(gameManager(), SIGNAL(gameStarted(ChessGame*)), this,
		SLOT(newGameWindow(ChessGame*)));

	connect(this, SIGNAL(aboutToQuit()), this, SLOT(onAboutToQuit()));
}

CuteChessApplication::~CuteChessApplication()
{
	delete m_gameDatabaseDialog;
}

CuteChessApplication* CuteChessApplication::instance()
{
	return static_cast<CuteChessApplication*>(QApplication::instance());
}

QString CuteChessApplication::configPath()
{
	// We want to have the exact same config path in "gui" and
	// "cli" applications so that they can share resources
	QSettings settings;
	QFileInfo fi(settings.fileName());
	QDir dir(fi.absolutePath());

	if (!dir.exists())
		dir.mkpath(fi.absolutePath());

	return fi.absolutePath();
}

EngineManager* CuteChessApplication::engineManager()
{
	if (m_engineManager == 0)
		m_engineManager = new EngineManager(this);

	return m_engineManager;
}

GameManager* CuteChessApplication::gameManager()
{
	if (m_gameManager == 0)
		m_gameManager = new GameManager(this);

	return m_gameManager;
}

QList<MainWindow*> CuteChessApplication::gameWindows()
{
	cleanGameWindows();

	QList<MainWindow*> gameWindowList;
	for (int i = 0; i < m_gameWindows.size(); i++)
		gameWindowList.append(m_gameWindows.at(i));

	return gameWindowList;
}

MainWindow* CuteChessApplication::newGameWindow(ChessGame* game)
{
	MainWindow* mainWindow = new MainWindow(game);
	m_gameWindows.prepend(mainWindow);
	mainWindow->show();

	return mainWindow;
}

void CuteChessApplication::newDefaultGame()
{
	// default game is a human versus human game using standard variant and
	// infinite time control
	ChessGame* game = new ChessGame(Chess::BoardFactory::create("standard"),
		new PgnGame());

	game->setTimeControl(TimeControl("inf"));

	gameManager()->newGame(game, new HumanBuilder(), new HumanBuilder());
}


void CuteChessApplication::cleanGameWindows()
{
	for (int i = m_gameWindows.size() - 1; i >= 0; i--)
	{
		if (m_gameWindows.at(i).isNull())
			m_gameWindows.removeAt(i);
	}
}

void CuteChessApplication::showGameWindow(int index)
{
	MainWindow* gameWindow = m_gameWindows.at(index);
	gameWindow->activateWindow();
	gameWindow->raise();
}

GameDatabaseManager* CuteChessApplication::gameDatabaseManager()
{
	if (m_gameDatabaseManager == 0)
	{
		m_gameDatabaseManager = new GameDatabaseManager(this);

		connect(m_gameDatabaseManager, SIGNAL(importStarted(PgnImporter*)),
			this, SLOT(showImportProgressDialog(PgnImporter*)));
	}

	return m_gameDatabaseManager;
}

void CuteChessApplication::showGameDatabaseDialog()
{
	if (m_gameDatabaseDialog == 0)
		m_gameDatabaseDialog = new GameDatabaseDialog();

	m_gameDatabaseDialog->show();
	m_gameDatabaseDialog->raise();
	m_gameDatabaseDialog->activateWindow();
}

void CuteChessApplication::onAboutToQuit()
{
	if (gameDatabaseManager()->isModified())
		gameDatabaseManager()->writeState(configPath() + QLatin1String("/gamedb.bin"));
}

void CuteChessApplication::showImportProgressDialog(PgnImporter* importer)
{
	ImportProgressDialog* dlg = new ImportProgressDialog(importer);

	dlg->show();
	dlg->raise();
	dlg->activateWindow();
}
