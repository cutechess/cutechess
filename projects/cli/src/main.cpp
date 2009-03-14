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
#include "enginematch.h"


void msgOutput(QtMsgType type, const char *msg)
 {
     switch (type) {
     case QtDebugMsg:
	 fprintf(stdout, "%s\n", msg);
	 break;
     case QtWarningMsg:
	 fprintf(stderr, "Warning: %s\n", msg);
	 break;
     case QtCriticalMsg:
	 fprintf(stderr, "Critical: %s\n", msg);
	 break;
     case QtFatalMsg:
	 fprintf(stderr, "Fatal: %s\n", msg);
	 abort();
     }
 }

struct CmdOption
{
	QString name;
	QString value;
	QStringList args;
};

static QList<CmdOption> getOptions(const QStringList& args)
{
	QList<CmdOption> options;

	QStringList::const_iterator it;
	for (it = args.constBegin(); it != args.constEnd(); ++it)
	{
		CmdOption option;
		option.name = *it;
		if (it->startsWith('-'))
		{
			for (++it; it != args.constEnd() && !it->startsWith('-'); ++it)
				option.args.append(*it);
			--it;
		}
		if (option.args.size() == 1)
			option.value = option.args.at(0);
		options.append(option);
	}

	return options;
}

struct EngineData
{
	EngineConfiguration config;
	EngineSettings settings;
};

static void parseEngine(const QStringList& args, EngineData& data)
{
	foreach (const QString& arg, args)
	{
		QStringList tmp = arg.split('=', QString::SkipEmptyParts);
		if (tmp.size() != 2)
			continue;

		const QString& name = tmp[0];
		const QString& val = tmp[1];

		if (name == "name")
			data.config.setName(val);
		else if (name == "cmd")
			data.config.setCommand(val);
		else if (name == "dir")
			data.config.setWorkingDirectory(val);
		else if (name == "proto")
		{
			if (val == "uci")
				data.config.setProtocol(ChessEngine::Uci);
			else if (val == "xboard")
				data.config.setProtocol(ChessEngine::Xboard);
			else
				qFatal("Usupported chess protocol: %s", qPrintable(val));
		}

		// Time control (moves/time+increment)
		else if (name == "tc")
		{
			TimeControl tc(val);
			if (!tc.isValid())
				qFatal("Invalid time control: %s", qPrintable(val));
			data.settings.setTimeControl(tc);
		}
		// Max. number of cpus the engine can use
		else if (name == "cpus")
		{
			if (val.toInt() <= 0)
				qFatal("Invalid cpu count: %s", qPrintable(val));
			data.settings.setConcurrency(val.toInt());
		}
		// Path to endgame bitbases
		else if (name == "egbbpath")
			data.settings.setEgbbPath(val);
		// Path to endgame tablebases
		else if (name == "egtbpath")
			data.settings.setEgtbPath(val);
		// Custom UCI option
		else if (name.startsWith("uci/"))
			data.settings.addUciSetting(name.section('/', 1), val);
	}
}

static EngineMatch* parseMatch(const QStringList& args, QObject* parent)
{
	EngineMatch* match = new EngineMatch(parent);
	QList<CmdOption> options = getOptions(args);

	EngineData fcp;
	EngineData scp;

	foreach (const CmdOption& opt, options)
	{
		// First chess program
		if (opt.name == "-fcp")
			parseEngine(opt.args, fcp);
		// Second chess program
		else if (opt.name == "-scp")
			parseEngine(opt.args, scp);
		// The engine options apply to both engines
		else if (opt.name == "-both")
		{
			parseEngine(opt.args, fcp);
			parseEngine(opt.args, scp);
		}
		// Chess variant (default: standard chess)
		else if (opt.name == "-variant")
			match->setVariant(Chess::Variant(opt.value));
		// Opening book file (must be in Polyglot format)
		else if (opt.name == "-book")
			match->setBookFile(opt.value);
		// Maximum book depth in plies (halfmoves)
		else if (opt.name == "-bookdepth")
			match->setBookDepth(opt.value.toInt());
		// Event name
		else if (opt.name == "-event")
			match->setEvent(opt.value);
		// Number of games to play
		else if (opt.name == "-games")
			match->setGameCount(opt.value.toInt());
		// Debugging mode. Prints all engine input and output.
		else if (opt.name == "-debug")
			match->setDebugMode(true);
		// PGN file where the games should be saved
		else if (opt.name == "-pgnout")
			match->setPgnOutput(opt.value);
		// Site/location name
		else if (opt.name == "-site")
			match->setSite(opt.value);
		else
			qFatal("Invalid argument: %s", qPrintable(opt.name));
	}

	match->addEngine(fcp.config, fcp.settings);
	match->addEngine(scp.config, scp.settings);

	return match;
}

int main(int argc, char* argv[])
{
	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
	qInstallMsgHandler(msgOutput);
	
	QCoreApplication::setOrganizationName("cutechess");
	QCoreApplication::setOrganizationDomain("cutechess.org");
	QCoreApplication::setApplicationName("cutechess");

	QCoreApplication app(argc, argv);

	// Use Ini format on all platforms
	QSettings::setDefaultFormat(QSettings::IniFormat);

	QStringList arguments = QCoreApplication::arguments();
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
	}

	EngineMatch* match = parseMatch(arguments, &app);
	QObject::connect(match, SIGNAL(finished()), &app, SLOT(quit()));

	if (!match->initialize())
		return 1;
	match->start();

	return app.exec();
}
