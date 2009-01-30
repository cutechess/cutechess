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

#include <QtGlobal>

#if QT_VERSION < 0x040400
  #error "Qt version 4.4.0 or later is required"
#endif

#include <QCoreApplication>
#include <QDebug>
#include <QTextStream>
#include <QTime>
#include <QSettings>
#include <QStringList>


int main(int argc, char* argv[])
{
	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
	
	QCoreApplication::setOrganizationName("Cute Chess");
	QCoreApplication::setOrganizationDomain("cutechess.org");
	QCoreApplication::setApplicationName("Cute Chess");

	QCoreApplication app(argc, argv);

	// Use Ini format on all platforms
	QSettings::setDefaultFormat(QSettings::IniFormat);

	QStringList arguments = app.arguments();
	arguments.takeFirst(); // application name

	// Use trivial command-line parsing for now
	QTextStream out(stdout);
	foreach (const QString& arg, arguments)
	{
		if (arg == "-v" || arg == "--version")
		{
			out << "cutechess-cli " << CUTECHESS_CLI_VERSION << endl;
			out << "Copyright (C) 2008-2009 Ilari Pihlajisto and Arto Jonsson" << endl;
			out << "This is free software; see the source for copying ";
			out << "conditions.  There is NO" << endl << "warranty; not even for ";
			out << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.";
			out << endl << endl;

			return 0;
		}
		else
		{
			qWarning() << "Unknown argument:" << arg;
		}
	}

	return app.exec();
}

