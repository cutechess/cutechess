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

#include "cutechessapp.h"

#include <QLoggingCategory>
#include <QTextStream>
#include <QStringList>
#include <QMetaType>
#include <QTranslator>
#include <QSysInfo>

#include <board/genericmove.h>
#include <board/move.h>
#include <board/side.h>
#include <board/result.h>
#include <moveevaluation.h>

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString& msg)
{
	CuteChessApplication::instance()->messageHandler(type, context, msg);
}

int main(int argc, char* argv[])
{
	// Register types for signal / slot connections
	qRegisterMetaType<Chess::GenericMove>("Chess::GenericMove");
	qRegisterMetaType<Chess::Move>("Chess::Move");
	qRegisterMetaType<Chess::Side>("Chess::Side");
	qRegisterMetaType<Chess::Result>("Chess::Result");
	qRegisterMetaType<MoveEvaluation>("MoveEvaluation");
	qRegisterMetaType<QtMsgType>("QtMsgType");

	QLoggingCategory::defaultCategory()->setEnabled(QtDebugMsg, true);

	qSetMessagePattern("%{time yyyy-MM-dd h:mm:ss.zzz} %{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif} %{message}");

	CuteChessApplication app(argc, argv);

	// Setup message handler for console output
	QObject::connect(&app, &CuteChessApplication::applicationMessage,
	    &app, &CuteChessApplication::printApplicationMessage);

	qInstallMessageHandler(messageHandler);

	QTranslator translator;
	translator.load(QLocale(), "cutechess", "_", "translations", ".qm");
	app.installTranslator(&translator);

	QStringList arguments = app.arguments();
	arguments.takeFirst(); // application name

	// Use trivial command-line parsing for now
	QTextStream out(stdout);
	while (!arguments.isEmpty())
	{
		if (arguments.first() == QLatin1String("-v") ||
			arguments.first() == QLatin1String("--version"))
		{
			out << "Cute Chess " << CUTECHESS_VERSION << '\n';
			out << "Using Qt version " << qVersion() << '\n';
			out << "Running on " << QSysInfo::prettyProductName();
			out << "/" << QSysInfo::currentCpuArchitecture() << '\n' << '\n';
			out << "Copyright (C) 2008-2020 Ilari Pihlajisto, Arto Jonsson ";
			out << "and contributors" << '\n';
			out << "This is free software; see the source for copying ";
			out << "conditions.  There is NO" << '\n' << "warranty; not even for ";
			out << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.";
			out << '\n' << '\n';

			return 0;
		}
		else
		{
			out << "Unknown argument: " << arguments.first() << '\n';
		}
		arguments.takeFirst();
	}
	app.newDefaultGame();
	return app.exec();
}

