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

#ifndef CUTE_CHESS_APPLICATION_H
#define CUTE_CHESS_APPLICATION_H

#include <QApplication>
#include <QPointer>

class EngineManager;
class GameManager;
class MainWindow;
class GameDatabaseManager;
class GameDatabaseDialog;
class PgnImporter;
class ChessGame;

class CuteChessApplication : public QApplication
{
	Q_OBJECT

	public:
		CuteChessApplication(int& argc, char* argv[]);
		~CuteChessApplication();

		QString configPath();
		EngineManager* engineManager();
		GameManager* gameManager();
		QList<MainWindow*> gameWindows();
		void showGameWindow(int index);
		GameDatabaseManager* gameDatabaseManager();
		static CuteChessApplication* instance();

	public slots:
		MainWindow* newGameWindow(ChessGame* game);
		MainWindow* newDefaultGameWindow();
		void showGameDatabaseDialog();

	private slots:
		void showImportProgressDialog(PgnImporter* importer);

	private:
		EngineManager* m_engineManager;
		GameManager* m_gameManager;
		GameDatabaseManager* m_gameDatabaseManager;
		QList<QPointer<MainWindow> > m_gameWindows;
		void cleanGameWindows();
		GameDatabaseDialog* m_gameDatabaseDialog;

	private slots:
		void onAboutToQuit();

};

#endif // CUTE_CHESS_APPLICATION_H
