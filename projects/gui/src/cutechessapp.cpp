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
#include <chessplayer.h>
#include <timecontrol.h>
#include <humanplayer.h>
#include "mainwindow.h"


CuteChessApplication::CuteChessApplication(int& argc, char* argv[])
	: QApplication(argc, argv),
	  m_engineManager(0),
	  m_gameManager(0)
{
	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

	QCoreApplication::setOrganizationName("cutechess");
	QCoreApplication::setOrganizationDomain("cutechess.org");
	QCoreApplication::setApplicationName("cutechess");

	// Use Ini format on all platforms
	QSettings::setDefaultFormat(QSettings::IniFormat);

	// Load the engines
	engineManager()->loadEngines(configPath() + QLatin1String("/engines.json"));
}

CuteChessApplication::~CuteChessApplication()
{
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

MainWindow* CuteChessApplication::newGameWindow()
{
	// TODO: This should be placed in a class that manages games

	ChessPlayer* player[2] = { 0, 0 };
	ChessGame* game = new ChessGame(Chess::BoardFactory::create("standard"), new PgnGame(), this);

	game->setTimeControl(TimeControl("inf"));

	for (int i = 0; i < 2; i++)
	{
		Chess::Side side = Chess::Side::Type(i);
		player[i] = new HumanPlayer(this);
		game->setPlayer(side, player[i]);
	}

	MainWindow* mainWindow = new MainWindow(game);
	m_gameWindows.prepend(mainWindow);
	mainWindow->show();

	return mainWindow;
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
