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

#include <QDebug>
#include <QTextStream>
#include <QStringList>
#include <csignal>
#include <cstdlib>
#include <enginemanager.h>
#include "enginematch.h"
#include "cutechesscoreapp.h"

static EngineMatch* match = 0;

void sigintHandler(int param)
{
	Q_UNUSED(param);
	if (match != 0)
		match->stop();
	else
		abort();
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
		if (option.args.size() > 0)
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

static void listEngines()
{
	const QList<EngineConfiguration> engines =
		CuteChessCoreApplication::engineManager()->engines();

	QStringList engineNames;

	foreach (const EngineConfiguration& engine, engines)
	{
		engineNames << engine.name();
	}

	engineNames.sort();
	foreach (const QString& name, engineNames)
		qDebug("%s", qPrintable(name));
}

static bool readEngineConfig(const QString& name, EngineConfiguration& config)
{
	const QList<EngineConfiguration> engines =
		CuteChessCoreApplication::engineManager()->engines();

	foreach (const EngineConfiguration& engine, engines)
	{
		if (engine.name() == name)
		{
			config = engine;
			return true;
		}
	}
	return false;
}

static bool parseEngine(const QStringList& args, EngineData& data)
{
	foreach (const QString& arg, args)
	{
		QString name = arg.section('=', 0, 0);
		QString val = arg.section('=', 1);
		if (name.isEmpty())
			continue;

		if (name == "conf")
		{
			if (!readEngineConfig(val, data.config))
			{
				qWarning() << "Unknown engine configuration:" << val;
				return false;
			}
		}
		else if (name == "name")
			data.config.setName(val);
		else if (name == "cmd")
		{
			if (val.contains(' '))
			{
				val.push_front('\"');
				val.push_back('\"');
			}
			data.config.setCommand(val);
		}
		else if (name == "dir")
			data.config.setWorkingDirectory(val);
		else if (name == "arg")
			data.settings.addArgument(val);
		else if (name == "proto")
		{
			if (val == "uci")
				data.config.setProtocol(ChessEngine::Uci);
			else if (val == "xboard")
				data.config.setProtocol(ChessEngine::Xboard);
			else
			{
				qWarning()<< "Usupported chess protocol:" << val;
				return false;
			}
		}
		else if (name == "initstr")
			data.settings.addInitString(val);

		// Time control (moves/time+increment)
		else if (name == "tc")
		{
			TimeControl tc(val);
			if (!tc.isValid())
			{
				qWarning() << "Invalid time control:" << val;
				return false;
			}
			tc.setMaxDepth(data.settings.timeControl().maxDepth());
			tc.setNodeLimit(data.settings.timeControl().nodeLimit());
			data.settings.setTimeControl(tc);
		}
		else if (name == "invertscores")
		{
			data.settings.setWhiteEvalPov(true);
		}
		else if (name == "depth")
		{
			if (val.toInt() <= 0)
			{
				qWarning() << "Invalid depth limit:" << val;
				return false;
			}
			data.settings.timeControl().setMaxDepth(val.toInt());
		}
		else if (name == "nodes")
		{
			if (val.toInt() <= 0)
			{
				qWarning() << "Invalid node limit:" << val;
				return false;
			}
			data.settings.timeControl().setNodeLimit(val.toInt());
		}
		// Custom engine option
		else if (name.startsWith("option."))
			data.settings.addCustomSetting(name.section('.', 1), val);
		else
		{
			qWarning() << "Invalid engine option:" << name;
			return false;
		}
	}

	return true;
}

static EngineMatch* parseMatch(const QStringList& args, QObject* parent)
{
	EngineMatch* match = new EngineMatch(parent);
	QList<CmdOption> options = getOptions(args);

	EngineData fcp;
	EngineData scp;
	bool ok = false;

	foreach (const CmdOption& opt, options)
	{
		bool optOk = true;

		// First chess program
		if (opt.name == "-fcp")
			optOk = ok = parseEngine(opt.args, fcp);
		// Second chess program
		else if (opt.name == "-scp")
			optOk = ok = parseEngine(opt.args, scp);
		// The engine options apply to both engines
		else if (opt.name == "-both")
		{
			optOk = ok = (parseEngine(opt.args, fcp) &&
				      parseEngine(opt.args, scp));
		}
		// Chess variant (default: standard chess)
		else if (opt.name == "-variant")
			match->setVariant(Chess::Variant(opt.value));
		// Opening book file (must be in Polyglot format)
		else if (opt.name == "-book")
			optOk = ok = match->setBookFile(opt.value);
		// Maximum book depth in plies (halfmoves)
		else if (opt.name == "-bookdepth")
			optOk = ok = match->setBookDepth(opt.value.toInt());
		// Threshold for draw adjudication
		else if (opt.name == "-draw")
		{
			if (opt.args.size() != 2)
			{
				qWarning() << "-draw needs two arguments";
				ok = false;
				break;
			}
			int moveNumber = opt.args[0].toInt();
			int score = opt.args[1].toInt();
			match->setDrawThreshold(moveNumber, score);
		}
		// Threshold for resign adjudication
		else if (opt.name == "-resign")
		{
			if (opt.args.size() != 2)
			{
				qWarning() << "-resign needs two arguments";
				ok = false;
				break;
			}
			int moveCount = opt.args[0].toInt();
			int score = opt.args[1].toInt();
			match->setResignThreshold(moveCount, -score);
		}
		// Event name
		else if (opt.name == "-event")
			match->setEvent(opt.value);
		// Number of games to play
		else if (opt.name == "-games")
			match->setGameCount(opt.value.toInt());
		// Debugging mode. Prints all engine input and output.
		else if (opt.name == "-debug")
			match->setDebugMode(true);
		// Use a PGN file as the opening book
		else if (opt.name == "-pgnin")
			optOk = ok = match->setPgnInput(opt.value);
		// PGN file where the games should be saved
		else if (opt.name == "-pgnout")
		{
			PgnGame::PgnMode mode = PgnGame::Verbose;
			if (opt.args.size() > 1 && opt.args[1] == "min")
				mode = PgnGame::Minimal;
			match->setPgnOutput(opt.value, mode);
		}
		// Play every opening twice, just switch the players' sides
		else if (opt.name == "-repeat")
			match->setRepeatOpening(true);
		// Site/location name
		else if (opt.name == "-site")
			match->setSite(opt.value);
		// Delay between games
		else if (opt.name == "-wait")
			match->setWait(opt.value.toInt());
		else
		{
			qWarning() << "Invalid argument:" << opt.name;
			optOk = ok = false;
		}

		if (!optOk)
			break;
	}

	if (!ok)
	{
		delete match;
		return 0;
	}

	match->addEngine(fcp.config, fcp.settings);
	match->addEngine(scp.config, scp.settings);

	return match;
}

int main(int argc, char* argv[])
{
	signal(SIGTERM, sigintHandler);
	signal(SIGINT, sigintHandler);

	CuteChessCoreApplication app(argc, argv);

	// Load the engines
	CuteChessCoreApplication::engineManager()->loadEngines();

	QStringList arguments = CuteChessCoreApplication::arguments();
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
		else if (arg == "--engines")
		{
			listEngines();			
			return 0;
		}
		else if (arg == "--help")
		{
			out << "Usage: cutechess-cli -fcp [eng_options] -scp [eng_options] [options]\n"
			       "Options:\n"
			       "  --help		Display this information\n"
			       "  --version		Display the version number\n"
			       "  --engines		Display the list of configured engines and exit\n\n"
			       "  -fcp <options>	Apply <options> to the first engine\n"
			       "  -scp <options>	Apply <options> to the second engine\n"
			       "  -both <options>	Apply <options> to both engines\n"
			       "  -variant <arg>	Set chess variant to <arg>. Must be Standard,\n"
			       "			Fischerandom, Capablanca, Gothic or Caparandom\n"
			       "  -book <file>		Use <file> (Polyglot book file) as the opening book\n"
			       "  -bookdepth <n>	Set the maximum book depth (in plies) to <n>\n"
			       "  -draw <n> <score>	Adjudicate the game as a draw if the score of both\n"
			       "			engines is within <score> centipawns from zero after\n"
			       "			<n> full moves have been played\n"
			       "  -resign <n> <score>	Adjudicate the game as a loss if an engine's score is\n"
			       "			at least <score> centipawns below zero for at least\n"
			       "			<n> consecutive moves\n"
			       "  -event <arg>		Set the event name to <arg>\n"
			       "  -games <n>		Play <n> games\n"
			       "  -debug		Display all engine input and output\n"
			       "  -pgnin <file>		Use <file> as the opening book in PGN format\n"
			       "  -pgnout <file> [min]	Save the games to <file> in PGN format. Use the 'min'\n"
			       "			argument to save in a minimal PGN format.\n"
			       "  -repeat		Play each opening twice so that both players get\n"
			       "			to play it on both sides\n"
			       "  -site <arg>		Set the site/location to <arg>\n"
			       "  -wait <n>		Wait <n> milliseconds between games. The default is 0.\n\n"
			       "Engine options:\n"
			       "  conf=<arg>		Use an engine with the name <arg> from Cute Chess'\n"
			       "			configuration file.\n"
			       "  name=<arg>		Set the name to <arg>\n"
			       "  cmd=<arg>		Set the command to <arg>\n"
			       "  dir=<arg>		Set the working directory to <arg>\n"
			       "  arg=<arg>		Pass <arg> to the engine as a command line argument\n"
			       "  initstr=<arg>		Send <arg> to the engine's standard input at startup\n"
			       "  proto=<arg>		Set the chess protocol to <arg>. Must be xboard or uci\n"
			       "  tc=<arg>		Set the time control to <arg>. The format is\n"
			       "			moves/time+increment, where 'moves' is the number of\n"
			       "			moves per tc, 'time' is time per tc (either seconds or\n"
			       "			minutes:seconds), and 'increment' is time increment\n"
			       "			per move in seconds\n"
			       "  invertscores		Inverts the engine's scores when it plays black\n"
			       "  depth=<arg>		Set the search depth limit to <arg>\n"
			       "  nodes=<arg>		Set the node count limit to <arg>\n"
			       "  option.<name>=<arg>	Set custom option <name> to value <arg>\n";
			return 0;
		}
	}

	match = parseMatch(arguments, &app);
	if (match == 0)
		return 1;
	QObject::connect(match, SIGNAL(finished()), &app, SLOT(quit()));

	if (!match->initialize())
		return 1;
	match->start();

	return app.exec();
}
