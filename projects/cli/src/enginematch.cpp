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

#include "enginematch.h"
#include <chessgame.h>
#include <uciengine.h>
#include <xboardengine.h>
#include <pgngame.h>
#include <polyglotbook.h>
#include <QProcess>
#include <QFile>
#include <QDataStream>
#include <QDir>
#include <QTimer>
#include <QtDebug>


EngineMatch::EngineMatch(QObject* parent)
	: QObject(parent),
	  m_bookDepth(30),
	  m_gameCount(1),
	  m_drawCount(0),
	  m_currentGame(0),
	  m_white(0),
	  m_black(0),
	  m_book(0),
	  m_debug(false),
	  m_pondering(false),
	  m_variant(Chess::Variant::Standard)
{
}

EngineMatch::~EngineMatch()
{
	if (m_book != 0)
		delete m_book;
}

void EngineMatch::addEngine(const EngineConfiguration& engineConfig)
{
	// We don't allow more than 2 engines at this point
	if (m_engines.size() >= 2)
	{
		qWarning() << "Only two engines can be added";
		return;
	}
	if (engineConfig.command().isEmpty())
		return;
	EngineData data = { engineConfig, 0, 0, 0 };
	m_engines.append(data);
}

void EngineMatch::setBookDepth(int bookDepth)
{
	if (bookDepth <= 0)
	{
		qWarning() << "Book depth must be bigger than zero";
		return;
	}
	m_bookDepth = bookDepth;
}

void EngineMatch::setBookFile(const QString& filename)
{
	if (m_book != 0)
	{
		delete m_book;
		m_book = 0;
	}

	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
	{
		qWarning() << "Can't open book file" << filename;
		return;
	}

	m_book = new PolyglotBook;
	QDataStream in(&file);
	in >> m_book;
}

void EngineMatch::setDebugMode(bool debug)
{
	m_debug = debug;
}

void EngineMatch::setEvent(const QString& event)
{
	m_event = event;
}

void EngineMatch::setGameCount(int gameCount)
{
	m_gameCount = gameCount;
}

void EngineMatch::setPgnOutput(const QString& filename)
{
	m_pgnOutput = filename;
}

void EngineMatch::setPondering(bool pondering)
{
	m_pondering = pondering;
}

void EngineMatch::setSite(const QString& site)
{
	m_site = site;
}

void EngineMatch::setVariant(Chess::Variant variant)
{
	m_variant = variant;
}

void EngineMatch::setTimeControl(const TimeControl& timeControl)
{
	m_timeControl = timeControl;
}

bool EngineMatch::initialize()
{
	if (m_engines.size() < 2)
	{
		qWarning() << "Two engines are needed";
		return false;
	}
	if (!m_timeControl.isValid())
	{
		qWarning() << "Invalid or non-existant time control";
		return false;
	}
	m_currentGame = 0;
	m_drawCount = 0;
	m_white = 0;
	m_black = 0;

	QVector<EngineData>::iterator it;
	for (it = m_engines.begin(); it != m_engines.end(); ++it)
	{
		it->wins = 0;

		QProcess* process = new QProcess(this);
		it->process = process;
		const EngineConfiguration& config = it->engineConfig;
		
		if (config.workingDirectory().isEmpty())
			process->setWorkingDirectory(QDir::tempPath());
		else
			process->setWorkingDirectory(config.workingDirectory());

		process->start(config.command());
		if (!process->waitForStarted())
		{
			qWarning() << "Cannot start engine" << config.command();
			return false;
		}

		ChessEngine* engine = 0;
		switch (config.protocol())
		{
		case ChessEngine::Uci:
			engine = new UciEngine(process, this);
			break;
		case ChessEngine::Xboard:
			engine = new XboardEngine(process, this);
			break;
		default:
			return false;
		}
		if (!config.name().isEmpty())
			engine->setName(config.name());
		it->engine = engine;

		if (m_debug)
			connect(engine, SIGNAL(debugMessage(const QString&)),
				this, SLOT(print(const QString&)));
	}

	return true;
}

void EngineMatch::killEngines()
{
	foreach(const EngineData& data, m_engines)
	{
		data.engine->quit();
		data.process->waitForFinished();
	}
}

void EngineMatch::onGameEnded()
{
	ChessGame* game = qobject_cast<ChessGame*>(QObject::sender());
	Q_ASSERT(game != 0);

	Chess::Result result = game->result();
	qDebug("Game %d ended: %s", m_currentGame + 1, qPrintable(result.toString()));
	if (result.isDraw())
		m_drawCount++;
	else
	{
		if (result.winner() == Chess::White)
		{
			m_white->wins++;
			qDebug("%s wins the game as white",
			       qPrintable(m_white->engine->name()));
		}
		else if (result.winner() == Chess::Black)
		{
			m_black->wins++;
			qDebug("%s wins the game as black",
			       qPrintable(m_black->engine->name()));
		}
	}
	if (!m_pgnOutput.isEmpty())
	{
		game->setEvent(m_event);
		game->setSite(m_site);
		game->setRound(m_currentGame + 1);
		game->write(m_pgnOutput);
	}

	delete game;

	m_currentGame++;
	qDebug("Score of %s vs %s: %d - %d - %d",
	       qPrintable(m_engines[0].engine->name()),
	       qPrintable(m_engines[1].engine->name()),
	       m_engines[0].wins, m_engines[1].wins, m_drawCount);

	if (m_currentGame < m_gameCount
	&&  result.code() != Chess::Result::WinByDisconnection)
		QTimer::singleShot(2000, this, SLOT(start()));
	else
	{
		killEngines();

		qDebug() << "Finished match";
		emit finished();
	}
}

void EngineMatch::start()
{
	qDebug() << "Started game" << m_currentGame + 1 << "of" << m_gameCount;

	ChessGame* game = new ChessGame(m_variant, this);

	if ((m_currentGame % 2) == 0)
	{
		m_white = &m_engines[0];
		m_black = &m_engines[1];
	}
	else
	{
		m_white = &m_engines[1];
		m_black = &m_engines[0];
	}
	game->setPlayer(Chess::White, m_white->engine);
	game->setPlayer(Chess::Black, m_black->engine);

	if (m_book != 0)
		game->setOpeningMoves(m_book, m_bookDepth);

	foreach(const EngineData& data, m_engines)
		data.engine->setTimeControl(m_timeControl);

	connect(game, SIGNAL(gameEnded()), this, SLOT(onGameEnded()));
	game->start();
}

void EngineMatch::print(const QString& msg)
{
	qDebug("%s", qPrintable(msg));
}
