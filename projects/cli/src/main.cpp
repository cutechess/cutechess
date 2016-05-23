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
#include <sprt.h>
#include <board/gaviotatablebase.h>

#include "cutechesscoreapp.h"
#include "matchparser.h"
#include "enginematch.h"


static EngineMatch* match = nullptr;

void sigintHandler(int param)
{
	Q_UNUSED(param);
	if (match != nullptr)
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
		else if (name == "ponder")
		{
			data.config.setPondering(true);
		}
		// Custom engine option
		else if (name.startsWith("option."))
			data.config.setOption(name.section('.', 1), val);
		else if (name == "stderr")
			data.config.setStderrFile(val);
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
	parser.addOption("-srand", QVariant::UInt, 1, 1);
	parser.addOption("-tournament", QVariant::String, 1, 1);
	parser.addOption("-engine", QVariant::StringList, 1, -1, true);
	parser.addOption("-each", QVariant::StringList, 1);
	parser.addOption("-variant", QVariant::String, 1, 1);
	parser.addOption("-concurrency", QVariant::Int, 1, 1);
	parser.addOption("-draw", QVariant::StringList);
	parser.addOption("-resign", QVariant::StringList);
	parser.addOption("-gtb", QVariant::String, 1, 1);
	parser.addOption("-event", QVariant::String, 1, 1);
	parser.addOption("-games", QVariant::Int, 1, 1);
	parser.addOption("-rounds", QVariant::Int, 1, 1);
	parser.addOption("-sprt", QVariant::StringList);
	parser.addOption("-ratinginterval", QVariant::Int, 1, 1);
	parser.addOption("-debug", QVariant::Bool, 0, 0);
	parser.addOption("-openings", QVariant::StringList);
	parser.addOption("-bookmode", QVariant::String);
	parser.addOption("-pgnout", QVariant::StringList, 1, 2);
	parser.addOption("-repeat", QVariant::Bool, 0, 0);
	parser.addOption("-recover", QVariant::Bool, 0, 0);
	parser.addOption("-site", QVariant::String, 1, 1);
	parser.addOption("-wait", QVariant::Int, 1, 1);
	parser.addOption("-seeds", QVariant::UInt, 1, 1);
	if (!parser.parse())
		return nullptr;

	GameManager* manager = CuteChessCoreApplication::instance()->gameManager();

	QString ttype = parser.takeOption("-tournament").toString();
	if (ttype.isEmpty())
		ttype = "round-robin";
	Tournament* tournament = TournamentFactory::create(ttype, manager, parent);
	if (tournament == nullptr)
	{
		qWarning("Invalid tournament type: %s", qPrintable(ttype));
		return nullptr;
	}

	EngineMatch* match = new EngineMatch(tournament, parent);

	QList<EngineData> engines;
	QStringList eachOptions;
	GameAdjudicator adjudicator;

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
			QMap<QString, QString> params =
				option.toMap("movenumber|movecount|score");
			bool numOk = false;
			bool countOk = false;
			bool scoreOk = false;
			int moveNumber = params["movenumber"].toInt(&numOk);
			int moveCount = params["movecount"].toInt(&countOk);
			int score = params["score"].toInt(&scoreOk);

			ok = (numOk && countOk && scoreOk);
			if (ok)
				adjudicator.setDrawThreshold(moveNumber, moveCount, score);
		}
		// Threshold for resign adjudication
		else if (name == "-resign")
		{
			QMap<QString, QString> params = option.toMap("movecount|score");
			bool countOk = false;
			bool scoreOk = false;
			int moveCount = params["movecount"].toInt(&countOk);
			int score = params["score"].toInt(&scoreOk);

			ok = (countOk && scoreOk);
			if (ok)
				adjudicator.setResignThreshold(moveCount, -score);
		}
		// Gaviota tablebase adjudication
		else if (name == "-gtb")
		{
			adjudicator.setTablebaseAdjudication(true);
			QStringList paths = QStringList() << value.toString();

			ok = GaviotaTablebase::initialize(paths) &&
			     GaviotaTablebase::tbAvailable(3);
			if (!ok)
				qWarning("Could not load Gaviota tablebases");
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
			if (!tournament->canSetRoundMultiplier())
			{
				qWarning("Tournament \"%s\" does not support "
					 "user-defined round multipliers",
					 qPrintable(tournament->type()));
				ok = false;
			}
			else
			{
				int rounds = value.toInt(&ok);
				if (rounds <= 0)
					ok = false;
				else
					tournament->setRoundMultiplier(rounds);
			}
		}
		// SPRT-based stopping rule
		else if (name == "-sprt")
		{
			QMap<QString, QString> params = option.toMap("elo0|elo1|alpha|beta");
			bool sprtOk[4];
			double elo0 = params["elo0"].toDouble(sprtOk);
			double elo1 = params["elo1"].toDouble(sprtOk + 1);
			double alpha = params["alpha"].toDouble(sprtOk + 2);
			double beta = params["beta"].toDouble(sprtOk + 3);

			ok = (sprtOk[0] && sprtOk[1] && sprtOk[2] && sprtOk[3]);
			if (ok)
				tournament->sprt()->initialize(elo0, elo1, alpha, beta);
		}
		// Interval for rating list updates
		else if (name == "-ratinginterval")
			match->setRatingInterval(value.toInt());
		// Debugging mode. Prints all engine input and output.
		else if (name == "-debug")
			match->setDebugMode(true);
		// Use an opening suite
		else if (name == "-openings")
		{
			QMap<QString, QString> params =
				option.toMap("file|format=pgn|order=sequential|plies=1024|start=1");
			ok = !params.isEmpty();

			OpeningSuite::Format format = OpeningSuite::EpdFormat;
			if (params["format"] == "epd")
				format = OpeningSuite::EpdFormat;
			else if (params["format"] == "pgn")
				format = OpeningSuite::PgnFormat;
			else if (ok)
			{
				qWarning("Invalid opening suite format: \"%s\"",
					 qPrintable(params["format"]));
				ok = false;
			}

			OpeningSuite::Order order = OpeningSuite::SequentialOrder;
			if (params["order"] == "sequential")
				order = OpeningSuite::SequentialOrder;
			else if (params["order"] == "random")
				order = OpeningSuite::RandomOrder;
			else if (ok)
			{
				qWarning("Invalid opening selection order: \"%s\"",
					 qPrintable(params["order"]));
				ok = false;
			}

			int plies = params["plies"].toInt();
			int start = params["start"].toInt();

			ok = ok && plies > 0 && start > 0;
			if (ok)
			{
				tournament->setOpeningDepth(plies);

				OpeningSuite* suite = new OpeningSuite(params["file"],
								       format,
								       order,
								       start - 1);
				if (order == OpeningSuite::RandomOrder)
					qDebug("Indexing opening suite...");
				ok = suite->initialize();
				if (ok)
					tournament->setOpeningSuite(suite);
				else
					delete suite;
			}
		}
		else if (name == "-bookmode")
		{
			QString val = value.toString();
			if (val == "ram")
				match->setBookMode(OpeningBook::Ram);
			else if (val == "disk")
				match->setBookMode(OpeningBook::Disk);
			else
				ok = false;
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
		{
			uint seed = value.toUInt(&ok);
			if (ok)
				Mersenne::initialize(seed);
		}
		// Delay between games
		else if (name == "-wait")
		{
			ok = value.toInt() >= 0;
			if (ok)
				tournament->setStartDelay(value.toInt());
		}
		// How many players should be seeded?
		else if (name == "-seeds")
		{
			uint seedCount = value.toUInt(&ok);
			if (ok)
				tournament->setSeedCount(seedCount);
		}
		else
			qFatal("Unknown argument: \"%s\"", qPrintable(name));

		if (!ok)
		{
			// Empty values default to boolean type
			if (value.isValid() && value.type() == QVariant::Bool)
				qWarning("Empty value for option \"%s\"",
					 qPrintable(name));
			else
			{
				QString val;
				if (value.type() == QVariant::StringList)
					val = value.toStringList().join(" ");
				else
					val = value.toString();
				qWarning("Invalid value for option \"%s\": \"%s\"",
					 qPrintable(name), qPrintable(val));
			}

			delete match;
			delete tournament;
			return nullptr;
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
		return nullptr;
	}

	tournament->setAdjudicator(adjudicator);

	return match;
}

int main(int argc, char* argv[])
{
	setvbuf(stdout, nullptr, _IONBF, 0);
	signal(SIGINT, sigintHandler);

	CuteChessCoreApplication app(argc, argv);

	QStringList arguments = CuteChessCoreApplication::arguments();
	arguments.takeFirst(); // application name

	// Use trivial command-line parsing for now
	QTextStream out(stdout);
	foreach (const QString& arg, arguments)
	{
		if (arg == "-v" || arg == "--version" || arg == "-version")
		{
			out << "cutechess-cli " << CUTECHESS_CLI_VERSION << endl;
			out << "Using Qt version " << qVersion() << endl << endl;
			out << "Copyright (C) 2008-2016 Ilari Pihlajisto and Arto Jonsson" << endl;
			out << "This is free software; see the source for copying ";
			out << "conditions.  There is NO" << endl << "warranty; not even for ";
			out << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.";
			out << endl << endl;

			return 0;
		}
		else if (arg == "--engines" || arg == "-engines")
		{
			const QList<EngineConfiguration> engines =
				CuteChessCoreApplication::instance()->engineManager()->engines();

			foreach (const EngineConfiguration& engine, engines)
				out << engine.name() << endl;

			return 0;
		}
		else if (arg == "--help" || arg == "-help")
		{
			QFile file(":/help.txt");
			if (file.open(QIODevice::ReadOnly | QIODevice::Text))
				out << file.readAll();
			return 0;
		}
	}

	match = parseMatch(arguments, &app);
	if (match == nullptr)
		return 1;
	QObject::connect(match, SIGNAL(finished()), &app, SLOT(quit()));

	match->start();
	return app.exec();
}
