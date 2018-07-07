/*
    This file is part of Cute Chess.
    Copyright (C) 2008-2018 Cute Chess authors

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

#ifndef CUTE_CHESS_APPLICATION_H
#define CUTE_CHESS_APPLICATION_H

#include <QApplication>
#include <QPointer>

class EngineManager;
class GameManager;
class MainWindow;
class SettingsDialog;
class TournamentResultsDialog;
class GameDatabaseManager;
class GameDatabaseDialog;
class PgnImporter;
class ChessGame;
class GameWall;

class CuteChessApplication : public QApplication
{
	Q_OBJECT

	public:
		CuteChessApplication(int& argc, char* argv[]);
		virtual ~CuteChessApplication();

		QString configPath();
		EngineManager* engineManager();
		GameManager* gameManager();
		GameDatabaseManager* gameDatabaseManager();
		QList<MainWindow*> gameWindows();
		void showGameWindow(int index);
		TournamentResultsDialog* tournamentResultsDialog();

		static CuteChessApplication* instance();
		static QString userName();

	public slots:
		MainWindow* newGameWindow(ChessGame* game);
		void newDefaultGame();
		void showSettingsDialog();
		void showTournamentResultsDialog();
		void showGameDatabaseDialog();
		void showGameWall();
		void closeDialogs();
		void onQuitAction();

	private:
		void showDialog(QWidget* dlg);

		SettingsDialog* m_settingsDialog;
		TournamentResultsDialog* m_tournamentResultsDialog;
		EngineManager* m_engineManager;
		GameManager* m_gameManager;
		GameDatabaseManager* m_gameDatabaseManager;
		QList<QPointer<MainWindow> > m_gameWindows;
		GameDatabaseDialog* m_gameDatabaseDialog;
		QPointer<GameWall> m_gameWall;
		bool m_initialWindowCreated;

	private slots:
		void onLastWindowClosed();
		void onAboutToQuit();
};

#endif // CUTE_CHESS_APPLICATION_H
