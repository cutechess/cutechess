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
#include <QFile>

#include <mersenne.h>
#include <enginemanager.h>
#include <enginebuilder.h>
#include <gamemanager.h>
#include <tournament.h>
#include <tournamentfactory.h>
#include <board/boardfactory.h>
#include <enginefactory.h>
#include <enginetextoption.h>
#include <openingsuite.h>

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
		// Line that are sent to the engine at startup, ie. before
		// starting the chess protocol.
		else if (name == "initstr")
			data.config.addInitString(val.replace("\\n", "\n"));
		// Should the engine be restarted after each game?
		else if (name == "restart")
		{
			EngineConfiguration::RestartMode mode;
			if (val == "auto")
				mode = EngineConfiguration::RestartAuto;
			else if (val == "on")
				mode = EngineConfiguration::RestartOn;
			else if (val == "off")
				mode = EngineConfiguration::RestartOff;
			else
			{
				qWarning() << "Invalid restart mode:" << val;
				return false;
			}

			data.config.setRestartMode(mode);
		}
		// Trust all result claims coming from the engine?
		else if (name == "trust")
		{
			data.config.setClaimsValidated(false);
		}
		// Time control (moves/time+increment)
		else if (name == "tc")
		{
			TimeControl tc(val);
			if (!tc.isValid())
			{
				qWarning() << "Invalid time control:" << val;
				return false;
			}

			data.tc.setInfinity(tc.isInfinite());
			data.tc.setTimePerTc(tc.timePerTc());
			data.tc.setMovesPerTc(tc.movesPerTc());
			data.tc.setTimeIncrement(tc.timeIncrement());
		}
		// Search time per move
		else if (name == "st")
		{
			bool ok = false;
			int moveTime = val.toDouble(&ok) * 1000.0;
			if (!ok || moveTime <= 0)
			{
				qWarning() << "Invalid search time:" << val;
				return false;
			}
			data.tc.setTimePerMove(moveTime);
		}
		// Time expiry margin
		else if (name == "timemargin")
		{
			bool ok = false;
			int margin = val.toInt(&ok);
			if (!ok || margin < 0)
			{
				qWarning() << "Invalid time margin:" << val;
				return false;
			}
			data.tc.setExpiryMargin(margin);
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
			data.tc.setPlyLimit(val.toInt());
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
			data.config.setOption(name.section('.', 1), val);
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
	parser.addOption("-engine", QVariant::StringList, 1, -1, true);
	parser.addOption("-each", QVariant::StringList, 1);
	parser.addOption("-variant", QVariant::String, 1, 1);
	parser.addOption("-book", QVariant::String, 1, 1);
	parser.addOption("-bookdepth", QVariant::Int, 1, 1);
	parser.addOption("-concurrency", QVariant::Int, 1, 1);
	parser.addOption("-draw", QVariant::StringList, 2, 2);
	parser.addOption("-resign", QVariant::StringList, 2, 2);
	parser.addOption("-tournament", QVariant::String, 1, 1);
	parser.addOption("-event", QVariant::String, 1, 1);
	parser.addOption("-games", QVariant::Int, 1, 1);
	parser.addOption("-rounds", QVariant::Int, 1, 1);
	parser.addOption("-ratinginterval", QVariant::Int, 1, 1);
	parser.addOption("-debug", QVariant::Bool, 0, 0);
	parser.addOption("-epdin", QVariant::String, 1, 1);
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

	GameManager* manager = CuteChessCoreApplication::instance()->gameManager();

	QString ttype = parser.takeOption("-tournament").toString();
	if (ttype.isEmpty())
		ttype = "round-robin";
	Tournament* tournament = TournamentFactory::create(ttype, manager, parent);
	if (tournament == 0)
	{
		qWarning("Invalid tournament type: %s", qPrintable(ttype));
		return 0;
	}

	EngineMatch* match = new EngineMatch(tournament, parent);

	QList<EngineData> engines;
	QStringList eachOptions;

	foreach (const MatchParser::Option& option, parser.options())
	{
		bool ok = true;
		const QString& name = option.name;
		const QVariant& value = option.value;
		Q_ASSERT(!value.isNull());

		// Chess engine
		if (name == "-engine")
		{
			EngineData engine;
			engine.bookDepth = 1000;
			ok = parseEngine(value.toStringList(), engine);
			if (ok)
				engines.append(engine);
		}
		// The engine options that apply to each engine
		else if (name == "-each")
			eachOptions = value.toStringList();
		// Chess variant (default: standard chess)
		else if (name == "-variant")
		{
			ok = Chess::BoardFactory::variants().contains(value.toString());
			if (ok)
				tournament->setVariant(value.toString());
		}
		else if (name == "-concurrency")
		{
			ok = value.toInt() > 0;
			if (ok)
				manager->setConcurrency(value.toInt());
		}
		// Threshold for draw adjudication
		else if (name == "-draw")
		{
			QStringList list = value.toStringList();
			bool numOk = false;
			bool scoreOk = false;
			int moveNumber = list.at(0).toInt(&numOk);
			int score = list.at(1).toInt(&scoreOk);

			ok = (numOk && scoreOk);
			if (ok)
				tournament->setDrawThreshold(moveNumber, score);
		}
		// Threshold for resign adjudication
		else if (name == "-resign")
		{
			QStringList list = value.toStringList();
			bool countOk = false;
			bool scoreOk = false;
			int moveCount = list.at(0).toInt(&countOk);
			int score = list.at(1).toInt(&scoreOk);

			ok = (countOk && scoreOk);
			if (ok)
				tournament->setResignThreshold(moveCount, -score);
		}
		// Event name
		else if (name == "-event")
			tournament->setName(value.toString());
		// Number of games per encounter
		else if (name == "-games")
		{
			ok = value.toInt() > 0;
			if (ok)
				tournament->setGamesPerEncounter(value.toInt());
		}
		// Multiplier for the number of tournament rounds
		else if (name == "-rounds")
		{
			ok = value.toInt() > 0;
			if (ok)
				tournament->setRoundMultiplier(value.toInt());
		}
		// Interval for rating list updates
		else if (name == "-ratinginterval")
			match->setRatingInterval(value.toInt());
		// Debugging mode. Prints all engine input and output.
		else if (name == "-debug")
			match->setDebugMode(true);
		// PGN input depth in plies
		else if (name == "-pgndepth")
		{
			ok = value.toInt() > 0;
			if (ok)
				tournament->setOpeningDepth(value.toInt());
		}
		// Use an EPD file as the opening book
		else if (name == "-epdin")
		{
			ok = QFile::exists(value.toString());
			if (ok)
			{
				tournament->setOpeningSuite(
					new OpeningSuite(value.toString(),
							 OpeningSuite::EpdFormat));
			}
		}
		// Use a PGN file as the opening book
		else if (name == "-pgnin")
		{
			ok = QFile::exists(value.toString());
			if (ok)
			{
				tournament->setOpeningSuite(
					new OpeningSuite(value.toString(),
							 OpeningSuite::PgnFormat));
			}
		}
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
				tournament->setPgnOutput(list.at(0), mode);
		}
		// Play every opening twice, just switch the players' sides
		else if (name == "-repeat")
			tournament->setOpeningRepetition(true);
		// Recover crashed/stalled engines
		else if (name == "-recover")
			tournament->setRecoveryMode(true);
		// Site/location name
		else if (name == "-site")
			tournament->setSite(value.toString());
		// Set the random seed manually
		else if (name == "-srand")
			Mersenne::initialize(value.toUInt());
		// Delay between games
		else if (name == "-wait")
		{
			ok = value.toInt() >= 0;
			if (ok)
				tournament->setStartDelay(value.toInt());
		}
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
			delete tournament;
			return 0;
		}
	}

	bool ok = true;

	if (!eachOptions.isEmpty())
	{
		QList<EngineData>::iterator it;
		for (it = engines.begin(); it != engines.end(); ++it)
		{
			ok = parseEngine(eachOptions, *it);
			if (!ok)
				break;
		}
	}

	foreach (const EngineData& engine, engines)
	{
		if (!engine.tc.isValid())
		{
			ok = false;
			qWarning("Invalid or missing time control");
			break;
		}

		if (engine.config.command().isEmpty())
		{
			ok = false;
			qCritical("missing chess engine command");
			break;
		}

		if (engine.config.protocol().isEmpty())
		{
			ok = false;
			qWarning("Missing chess protocol");
			break;
		}

		tournament->addPlayer(new EngineBuilder(engine.config),
				      engine.tc,
				      match->addOpeningBook(engine.book),
				      engine.bookDepth);
	}

	if (engines.size() < 2)
	{
		qWarning("At least two engines are needed");
		ok = false;
	}

	if (!ok)
	{
		delete match;
		delete tournament;
		return 0;
	}

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
			out << "Copyright (C) 2008-2011 Ilari Pihlajisto and Arto Jonsson" << endl;
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
		else if (arg == "--help")
		{
			out << "Usage: cutechess-cli -engine [eng_options] -engine [eng_options]... [options]\n"
			       "Options:\n"
			       "  --help		Display this information\n"
			       "  --version		Display the version number\n"
			       "  --engines		Display a list of configured engines and exit\n"
			       "  -engine <options>	Add an engine defined by <options> to the tournament\n"
			       "  -each <options>	Apply <options> to each engine in the tournament\n"
			       "  -variant <arg>	Set the chess variant to <arg>\n"
			       "  -concurrency <n>	Set the maximum number of concurrent games to <n>\n"
			       "  -draw <n> <score>	Adjudicate the game as a draw if the score of both\n"
			       "			engines is within <score> centipawns from zero after\n"
			       "			<n> full moves have been played\n"
			       "  -resign <n> <score>	Adjudicate the game as a loss if an engine's score is\n"
			       "			at least <score> centipawns below zero for at least\n"
			       "			<n> consecutive moves\n"
			       "  -tournament <arg>	Set the tournament type to <arg>. Supported types are:\n"
			       "			round-robin (default)\n"
			       "			gauntlet\n"
			       "  -event <arg>		Set the event name to <arg>\n"
			       "  -games <n>		Play <n> games per encounter. This value should be set\n"
			       "			to an even number in tournaments with more than two\n"
			       "			players to make sure that each player plays an equal\n"
			       "			number of games with white and black pieces.\n"
			       "  -rounds <n>		Multiply the number of rounds to play by <n>.\n"
			       "			For two-player tournaments this option should be used\n"
			       "			to set the total number of games to play.\n"
			       "  -ratinginterval <n>	Set the interval for printing the ratings to <n> games\n"
			       "  -debug		Display all engine input and output\n"
			       "  -epdin <file>		Use <file> as the opening book in EPD format\n"
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
			       "  restart=<arg>		Set the restart mode to <arg> which can be:\n"
			       "			'auto': the engine decides whether to restart (default)\n"
			       "			'on': the engine is always restarted between games\n"
			       "			'off': the engine is never restarted between games\n"
			       "  trust			Trust result claims from the engine without validation.\n"
			       "			By default all claims are validated.\n"
			       "  proto=<arg>		Set the chess protocol to <arg>\n"
			       "  tc=<arg>		Set the time control to <arg>. The format is\n"
			       "			moves/time+increment, where 'moves' is the number of\n"
			       "			moves per tc, 'time' is time per tc (either seconds or\n"
			       "			minutes:seconds), and 'increment' is time increment\n"
			       "			per move in seconds.\n"
			       "			Infinite time control can be set with 'tc=inf'.\n"
			       "  st=<n>		Set the time limit for each move to <n> seconds.\n"
			       "			This option can't be used in combination with \"tc\".\n"
			       "  timemargin=<n>	Let engines go <n> milliseconds over the time limit.\n"
			       "  book=<file>		Use <file> (Polyglot book file) as the opening book\n"
			       "  bookdepth=<n>		Set the maximum book depth (in fullmoves) to <n>\n"
			       "  whitepov		Invert the engine's scores when it plays black. This\n"
			       "			option should be used with engines that always report\n"
			       "			scores from white's perspective.\n"
			       "  depth=<n>		Set the search depth limit to <n> plies\n"
			       "  nodes=<n>		Set the node count limit to <n> nodes\n"
			       "  option.<name>=<arg>	Set custom option <name> to value <arg>\n";
			return 0;
		}
	}

	match = parseMatch(arguments, &app);
	if (match == 0)
		return 1;
	QObject::connect(match, SIGNAL(finished()), &app, SLOT(quit()));

	match->start();
	return app.exec();
}
