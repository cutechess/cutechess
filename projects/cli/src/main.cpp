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

#include <csignal>
#include <cstdlib>
#include <QtGlobal>
#include <QDebug>
#include <QTextStream>
#include <QStringList>
#include <enginemanager.h>
#include <board/boardfactory.h>
#include <enginefactory.h>
#include "cutechesscoreapp.h"
#include "matchparser.h"
#include "enginematch.h"


static EngineMatch* match = 0;

void sigintHandler(int param)
{
	Q_UNUSED(param);
	if (match != 0)
		match->stop();
	else
		abort();
}


struct EngineData
{
	EngineConfiguration config;
	TimeControl tc;
	QString book;
	int bookDepth;
};

static bool readEngineConfig(const QString& name, EngineConfiguration& config)
{
	const QList<EngineConfiguration> engines =
		CuteChessCoreApplication::instance()->engineManager()->engines();

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
			data.config.setCommand(val);
		else if (name == "dir")
			data.config.setWorkingDirectory(val);
		else if (name == "arg")
			data.config.addArgument(val);
		else if (name == "proto")
		{
			if (EngineFactory::protocols().contains(val))
				data.config.setProtocol(val);
			else
			{
				qWarning()<< "Usupported chess protocol:" << val;
				return false;
			}
		}
		else if (name == "initstr")
			data.config.addInitString(val.replace("\\n", "\n"));
		// Time control (moves/time+increment)
		else if (name == "tc")
		{
			TimeControl tc(val);
			if (!tc.isValid())
			{
				qWarning() << "Invalid time control:" << val;
				return false;
			}
			// Preserve previously set depth and node limits
			tc.setMaxDepth(data.tc.maxDepth());
			tc.setNodeLimit(data.tc.nodeLimit());

			data.tc = tc;
		}
		else if (name == "book")
			data.book = val;
		else if (name == "bookdepth")
		{
			if (val.toInt() <= 0)
			{
				qWarning() << "Invalid book depth limit:" << val;
				return false;
			}
			data.bookDepth = val.toInt();
		}
		else if (name == "whitepov")
		{
			data.config.setWhiteEvalPov(true);
		}
		else if (name == "depth")
		{
			if (val.toInt() <= 0)
			{
				qWarning() << "Invalid depth limit:" << val;
				return false;
			}
			data.tc.setMaxDepth(val.toInt());
		}
		else if (name == "nodes")
		{
			if (val.toInt() <= 0)
			{
				qWarning() << "Invalid node limit:" << val;
				return false;
			}
			data.tc.setNodeLimit(val.toInt());
		}
		// Custom engine option
		else if (name.startsWith("option."))
			data.config.addCustomOption(name.section('.', 1), val);
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
	MatchParser parser(args);
	parser.addOption("-fcp", QVariant::StringList, 1);
	parser.addOption("-scp", QVariant::StringList, 1);
	parser.addOption("-both", QVariant::StringList, 1);
	parser.addOption("-variant", QVariant::String, 1, 1);
	parser.addOption("-book", QVariant::String, 1, 1);
	parser.addOption("-bookdepth", QVariant::Int, 1, 1);
	parser.addOption("-concurrency", QVariant::Int, 1, 1);
	parser.addOption("-draw", QVariant::StringList, 2, 2);
	parser.addOption("-resign", QVariant::StringList, 2, 2);
	parser.addOption("-event", QVariant::String, 1, 1);
	parser.addOption("-games", QVariant::Int, 1, 1);
	parser.addOption("-debug", QVariant::Bool, 0, 0);
	parser.addOption("-pgnin", QVariant::String, 1, 1);
	parser.addOption("-pgndepth", QVariant::Int, 1, 1);
	parser.addOption("-pgnout", QVariant::StringList, 1, 2);
	parser.addOption("-repeat", QVariant::Bool, 0, 0);
	parser.addOption("-recover", QVariant::Bool, 0, 0);
	parser.addOption("-site", QVariant::String, 1, 1);
	parser.addOption("-srand", QVariant::UInt, 1, 1);
	parser.addOption("-wait", QVariant::Int, 1, 1);
	if (!parser.parse())
		return 0;

	EngineMatch* match = new EngineMatch(parent);
	EngineData fcp;
	EngineData scp;

	fcp.bookDepth = 1000;
	scp.bookDepth = 1000;

	QMap<QString, QVariant> options(parser.options());
	QMap<QString, QVariant>::const_iterator it;
	for (it = options.constBegin(); it != options.constEnd(); ++it)
	{
		bool ok = true;
		QString name = it.key();
		QVariant value = it.value();
		Q_ASSERT(!value.isNull());

		// First chess program
		if (name == "-fcp")
			ok = parseEngine(value.toStringList(), fcp);
		// Second chess program
		else if (name == "-scp")
			ok = parseEngine(value.toStringList(), scp);
		// The engine options apply to both engines
		else if (name == "-both")
		{
			ok = (parseEngine(value.toStringList(), fcp) &&
			      parseEngine(value.toStringList(), scp));
		}
		// Chess variant (default: standard chess)
		else if (name == "-variant")
			ok = match->setVariant(value.toString());
		else if (name == "-concurrency")
			ok = match->setConcurrency(value.toInt());
		// Threshold for draw adjudication
		else if (name == "-draw")
		{
			QStringList list = value.toStringList();
			bool numOk = false;
			bool scoreOk = false;
			int moveNumber = list[0].toInt(&numOk);
			int score = list[1].toInt(&scoreOk);

			ok = (numOk && scoreOk);
			if (ok)
				match->setDrawThreshold(moveNumber, score);
		}
		// Threshold for resign adjudication
		else if (name == "-resign")
		{
			QStringList list = value.toStringList();
			bool countOk = false;
			bool scoreOk = false;
			int moveCount = list[0].toInt(&countOk);
			int score = list[1].toInt(&scoreOk);

			ok = (countOk && scoreOk);
			if (ok)
				match->setResignThreshold(moveCount, -score);
		}
		// Event name
		else if (name == "-event")
			match->setEvent(value.toString());
		// Number of games to play
		else if (name == "-games")
			ok = match->setGameCount(value.toInt());
		// Debugging mode. Prints all engine input and output.
		else if (name == "-debug")
			match->setDebugMode(true);
		// PGN input depth in plies
		else if (name == "-pgndepth")
			ok = match->setPgnDepth(value.toInt());
		// Use a PGN file as the opening book
		else if (name == "-pgnin")
			ok = match->setPgnInput(value.toString());
		// PGN file where the games should be saved
		else if (name == "-pgnout")
		{
			PgnGame::PgnMode mode = PgnGame::Verbose;
			QStringList list = value.toStringList();
			if (list.size() == 2)
			{
				if (list.at(1) == "min")
					mode = PgnGame::Minimal;
				else
					ok = false;
			}
			if (ok)
				match->setPgnOutput(list[0], mode);
		}
		// Play every opening twice, just switch the players' sides
		else if (name == "-repeat")
			match->setRepeatOpening(true);
		// Recover crashed/stalled engines
		else if (name == "-recover")
			match->setRecoveryMode(true);
		// Site/location name
		else if (name == "-site")
			match->setSite(value.toString());
		// Set the random seed manually
		else if (name == "-srand")
			qsrand(value.toUInt());
		// Delay between games
		else if (name == "-wait")
			ok = match->setWait(value.toInt());
		else
			qFatal("Unknown argument: \"%s\"", qPrintable(name));

		if (!ok)
		{
			QString val;
			if (value.type() == QVariant::StringList)
				val = value.toStringList().join(" ");
			else
				val = value.toString();
			qWarning("Invalid value for option \"%s\": \"%s\"",
				 qPrintable(name), qPrintable(val));

			delete match;
			return 0;
		}
	}

	match->addEngine(fcp.config, fcp.tc, fcp.book, fcp.bookDepth);
	match->addEngine(scp.config, scp.tc, scp.book, scp.bookDepth);

	return match;
}

int main(int argc, char* argv[])
{
	signal(SIGINT, sigintHandler);

	CuteChessCoreApplication app(argc, argv);

	QStringList arguments = CuteChessCoreApplication::arguments();
	arguments.takeFirst(); // application name

	// Use trivial command-line parsing for now
	QTextStream out(stdout);
	foreach (const QString& arg, arguments)
	{
		if (arg == "-v" || arg == "--version")
		{
			out << "cutechess-cli " << CUTECHESS_CLI_VERSION << endl;
			out << "Using Qt version " << qVersion() << endl << endl;
			out << "Copyright (C) 2008-2010 Ilari Pihlajisto and Arto Jonsson" << endl;
			out << "This is free software; see the source for copying ";
			out << "conditions.  There is NO" << endl << "warranty; not even for ";
			out << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.";
			out << endl << endl;

			return 0;
		}
		else if (arg == "--engines")
		{
			const QList<EngineConfiguration> engines =
				CuteChessCoreApplication::instance()->engineManager()->engines();

			foreach (const EngineConfiguration& engine, engines)
				out << engine.name() << endl;

			return 0;
		}
		else if (arg == "--protocols")
		{
			foreach (const QString& protocol, EngineFactory::protocols())
				out << protocol << endl;

			return 0;
		}
		else if (arg == "--variants")
		{
			foreach (const QString& variant, Chess::BoardFactory::variants())
				out << variant << endl;

			return 0;
		}
		else if (arg == "--help")
		{
			out << "Usage: cutechess-cli -fcp [eng_options] -scp [eng_options] [options]\n"
			       "Options:\n"
			       "  --help		Display this information\n"
			       "  --version		Display the version number\n"
			       "  --engines		Display a list of configured engines and exit\n"
			       "  --protocols		Display a list of supported chess protocols and exit\n"
			       "  --variants		Display a list of supported chess variants and exit\n\n"
			       "  -fcp <options>	Apply <options> to the first engine\n"
			       "  -scp <options>	Apply <options> to the second engine\n"
			       "  -both <options>	Apply <options> to both engines\n"
			       "  -variant <arg>	Set the chess variant to <arg>\n"
			       "  -concurrency <n>	Set the maximum number of concurrent games to <n>\n"
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
			       "  -pgndepth <n>		Set the maximum depth for PGN input to <n> plies\n"
			       "  -pgnout <file> [min]	Save the games to <file> in PGN format. Use the 'min'\n"
			       "			argument to save in a minimal PGN format.\n"
			       "  -recover		Restart crashed engines instead of stopping the match\n"
			       "  -repeat		Play each opening twice so that both players get\n"
			       "			to play it on both sides\n"
			       "  -site <arg>		Set the site/location to <arg>\n"
			       "  -srand <n>		Set the random seed for the book move selector to <n>\n"
			       "  -wait <n>		Wait <n> milliseconds between games. The default is 0.\n\n"
			       "Engine options:\n"
			       "  conf=<arg>		Use an engine with the name <arg> from Cute Chess'\n"
			       "			configuration file.\n"
			       "  name=<arg>		Set the name to <arg>\n"
			       "  cmd=<arg>		Set the command to <arg>\n"
			       "  dir=<arg>		Set the working directory to <arg>\n"
			       "  arg=<arg>		Pass <arg> to the engine as a command line argument\n"
			       "  initstr=<arg>		Send <arg> to the engine's standard input at startup\n"
			       "  proto=<arg>		Set the chess protocol to <arg>\n"
			       "  tc=<arg>		Set the time control to <arg>. The format is\n"
			       "			moves/time+increment, where 'moves' is the number of\n"
			       "			moves per tc, 'time' is time per tc (either seconds or\n"
			       "			minutes:seconds), and 'increment' is time increment\n"
			       "			per move in seconds\n"
			       "  book=<file>		Use <file> (Polyglot book file) as the opening book\n"
			       "  bookdepth=<n>		Set the maximum book depth (in fullmoves) to <n>\n"
			       "  whitepov		Invert the engine's scores when it plays black. This\n"
			       "			option should be used with engines that always report\n"
			       "			scores from white's perspective.\n"
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

	int ret = app.exec();
	qDebug() << "Finished match";
	return ret;
}
