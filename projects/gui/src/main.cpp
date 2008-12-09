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

#include <QtGlobal>

#if QT_VERSION < 0x040400
  #error "Qt version 4.4.0 or later is required"
#endif

#include <QApplication>
#include <QTextStream>
#include <QDebug>
#include <QtGlobal>
#include <QTime>

#include "mainwindow.h"

int main(int argc, char* argv[])
{
	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
	
	QCoreApplication::setOrganizationName("SloppyGUI");
	QCoreApplication::setOrganizationDomain("sloppygui.org");
	QCoreApplication::setApplicationName("SloppyGUI");

	QApplication app(argc, argv);

	QStringList arguments = app.arguments();
	arguments.takeFirst(); // application name

	// Use trivial command-line parsing for now
	while (!arguments.empty())
	{
		QTextStream out(stdout);

		if (arguments.first() == QLatin1String("-v") ||
			arguments.first() == QLatin1String("--version"))
		{
			out << "SloppyGUI " << SLOPPYGUI_VERSION << endl;
			out << "Copyright (C) 2008 Ilari Pihlajisto and Arto Jonsson" << endl;
			out << "This is free software; see the source for copying ";
			out << "conditions.  There is NO" << endl << "warranty; not even for ";
			out << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.";
			out << endl << endl;

			return 0;
		}
		else
		{
			qWarning() << "Unknown argument:" << arguments.first();
		}
		arguments.takeFirst();
	}

	MainWindow mainWindow;
	mainWindow.show();

	int ret = app.exec();

	return ret;
}

