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

#include <QApplication>
#include <cstdio>

#include "mainwindow.h"
#include "manager.h"
#include "logmanager.h"
#include "chessboard/chessboard.h"

int main(int argc, char* argv[])
{
	// Use trivial command-line parsing for now
	for (int i = 1; i < argc; i++)
	{
		if (QLatin1String("-v") == argv[i] ||
			QLatin1String("--version") == argv[i])
		{
			printf("SloppyGUI %s\n", SLOPPYGUI_VERSION);
			printf("Copyright (C) 2008 Ilari Pihlajisto and Arto Jonsson\n");
			printf("This is free software; see the source for copying ");
			printf("conditions.  There is NO\nwarranty; not even for ");
			printf("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\n");

			return 0;
		}
	}

	Chessboard::initialize();

	qInstallMsgHandler(LogManager::messageHandler);
	QApplication app(argc, argv);

	MainWindow mainWindow;
	mainWindow.show();

	int ret = app.exec();

	// Install the default message handler
	qInstallMsgHandler(0);

	// Release the managers
	Manager::quit();
	Manager::release();

	return ret;
}

