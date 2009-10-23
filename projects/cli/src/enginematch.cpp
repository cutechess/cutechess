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
#include <enginefactory.h>
#include <enginebuilder.h>
#include <polyglotbook.h>
#include <QDir>
#include <QTimer>
#include <QtDebug>


EngineMatch::EngineMatch(QObject* parent)
	: QObject(parent),
	  m_bookDepth(1000),
	  m_gameCount(1),
	  m_drawCount(0),
	  m_currentGame(0),
	  m_finishedGames(0),
	  m_pgnGamesRead(0),
	  m_drawMoveNum(0),
	  m_drawScore(0),
	  m_resignMoveCount(0),
	  m_resignScore(0),
	  m_wait(0),
	  m_book(0),
	  m_debug(false),
	  m_finishing(false),
	  m_pgnMode(PgnGame::Verbose),
	  m_repeatOpening(false),
	  m_variant(Chess::Variant::Standard)
{
	m_startTime.start();
}

EngineMatch::~EngineMatch()
{
	delete m_book;
}

void EngineMatch::stop()
{
	if (m_finishing)
		return;

	m_finishing = true;
	disconnect(&m_manager, SIGNAL(ready()), this, SLOT(onManagerReady()));
	m_manager.finish();

	emit stopGame();
}

void EngineMatch::addEngine(const EngineConfiguration& engineConfig,
			    const EngineSettings& settings)
{
	// We don't allow more than 2 engines at this point
	if (m_engines.size() >= 2)
	{
		qWarning() << "Only two engines can be added";
		return;
	}
	if (engineConfig.command().isEmpty())
		return;
	EngineData data = { engineConfig, settings, 0, 0 };
	m_engines.append(data);
}

bool EngineMatch::setBookDepth(int bookDepth)
{
	if (bookDepth <= 0)
	{
		qWarning() << "Book depth must be bigger than zero";
		return false;
	}
	m_bookDepth = bookDepth;

	return true;
}

bool EngineMatch::setBookFile(const QString& filename)
{
	delete m_book;
	m_book = 0;

	m_book = new PolyglotBook;
	if (!m_book->read(filename))
	{
		delete m_book;
		m_book = 0;
		qWarning() << "Can't open book file" << filename;
		return false;
	}

	return true;
}

bool EngineMatch::setConcurrency(int concurrency)
{
	if (concurrency <= 0)
	{
		qWarning() << "Concurrency must be bigger than zero";
		return false;
	}
	m_manager.setConcurrency(concurrency);
	return true;
}

void EngineMatch::setDebugMode(bool debug)
{
	m_debug = debug;
}

void EngineMatch::setDrawThreshold(int moveNumber, int score)
{
	m_drawMoveNum = moveNumber;
	m_drawScore = score;
}

void EngineMatch::setEvent(const QString& event)
{
	m_event = event;
}

bool EngineMatch::setGameCount(int gameCount)
{
	if (gameCount <= 0)
		return false;
	m_gameCount = gameCount;
	return true;
}

bool EngineMatch::setPgnInput(const QString& filename)
{
	m_pgnInputFile.setFileName(filename);
	if (!m_pgnInputFile.open(QIODevice::ReadOnly))
	{
		qWarning() << "Can't open PGN file:" << filename;
		return false;
	}

	m_pgnInputStream.setDevice(&m_pgnInputFile);
	return true;
}

void EngineMatch::setPgnOutput(const QString& filename,
			       PgnGame::PgnMode mode)
{
	m_pgnOutput = filename;
	m_pgnMode = mode;
}

void EngineMatch::setRepeatOpening(bool repeatOpening)
{
	m_repeatOpening = repeatOpening;
}

void EngineMatch::setResignThreshold(int moveCount, int score)
{
	m_resignMoveCount = moveCount;
	m_resignScore = score;
}

void EngineMatch::setSite(const QString& site)
{
	m_site = site;
}

bool EngineMatch::setVariant(Chess::Variant variant)
{
	if (variant == Chess::Variant::NoVariant)
		return false;
	m_variant = variant;
	return true;
}

bool EngineMatch::setWait(int msecs)
{
	if (msecs < 0)
		return false;
	m_wait = msecs;
	return true;
}

bool EngineMatch::initialize()
{
	if (m_engines.size() < 2)
	{
		qWarning() << "Two engines are needed";
		return false;
	}

	m_finishing = false;
	m_currentGame = 0;
	m_finishedGames = 0;
	m_drawCount = 0;
	m_games.clear();

	QString path(QDir::currentPath());

	QVector<EngineData>::iterator it;
	for (it = m_engines.begin(); it != m_engines.end(); ++it)
	{
		if (!it->settings.timeControl().isValid())
		{
			qWarning() << "Invalid or missing time control";
			return false;
		}

		it->wins = 0;
		it->builder = new EngineBuilder(it->config, it->settings);
	}

	m_pgnInputStream.setVariant(m_variant);
	connect(&m_manager, SIGNAL(ready()), this, SLOT(onManagerReady()));
	connect(&m_manager, SIGNAL(finished()), this, SLOT(cleanup()));

	if (m_debug)
		connect(&m_manager, SIGNAL(debugMessage(const QString&)),
			this, SLOT(print(const QString&)));

	return true;
}

void EngineMatch::cleanup()
{
	foreach(const EngineData& data, m_engines)
		delete data.builder;
	emit finished();
}

void EngineMatch::onGameEnded()
{
	ChessGame* game = qobject_cast<ChessGame*>(QObject::sender());
	Q_ASSERT(game != 0);
	Q_ASSERT(game->thread() == thread());

	disconnect(this, SIGNAL(stopGame()), game, SLOT(kill()));

	QString name1;
	QString name2;
	EngineData* white = 0;
	EngineData* black = 0;
	if ((game->round() % 2) != 0)
	{
		white = &m_engines[0];
		black = &m_engines[1];
		name1 = game->player(Chess::White)->name();
		name2 = game->player(Chess::Black)->name();
	}
	else
	{
		white = &m_engines[1];
		black = &m_engines[0];
		name1 = game->player(Chess::Black)->name();
		name2 = game->player(Chess::White)->name();
	}

	Chess::Result result = game->result();
	qDebug("Game %d ended: %s", game->round(), qPrintable(result.toString()));
	if (result.isDraw())
		m_drawCount++;
	else
	{
		if (result.winner() == Chess::White)
		{
			white->wins++;
			qDebug("%s wins the game as white",
			       qPrintable(game->player(Chess::White)->name()));
		}
		else if (result.winner() == Chess::Black)
		{
			black->wins++;
			qDebug("%s wins the game as black",
			       qPrintable(game->player(Chess::Black)->name()));
		}
	}

	qDebug("Score of %s vs %s: %d - %d - %d",
	       qPrintable(name1),
	       qPrintable(name2),
	       m_engines[0].wins, m_engines[1].wins, m_drawCount);

	m_games[game->round()] = game;
	while (m_games.contains(m_finishedGames + 1))
	{
		m_finishedGames++;
		game = m_games.take(m_finishedGames);

		if (!m_pgnOutput.isEmpty())
			game->write(m_pgnOutput, m_pgnMode);

		game->deleteLater();
	}

	if (m_finishing)
		return;
	if (m_finishedGames >= m_gameCount
	||  result.code() == Chess::Result::ResultError
	||  result.code() == Chess::Result::NoResult
	||  result.code() == Chess::Result::WinByDisconnection
	||  result.code() == Chess::Result::WinByStalledConnection)
		stop();
}

void EngineMatch::onManagerReady()
{
	if (!m_finishing && m_currentGame < m_gameCount)
	{
		if (m_wait <= 0)
			start();
		else
			QTimer::singleShot(m_wait, this, SLOT(start()));
	}
}

void EngineMatch::start()
{
	m_currentGame++;
	qDebug() << "Started game" << m_currentGame << "of" << m_gameCount;

	ChessGame* game = new ChessGame(m_variant);
	connect(this, SIGNAL(stopGame()), game, SLOT(kill()), Qt::QueuedConnection);

	EngineData* white = 0;
	EngineData* black = 0;
	if ((m_currentGame % 2) != 0)
	{
		white = &m_engines[0];
		black = &m_engines[1];
	}
	else
	{
		white = &m_engines[1];
		black = &m_engines[0];
	}

	if (!m_fen.isEmpty() || !m_openingMoves.isEmpty())
	{
		if (!m_fen.isEmpty())
		{
			game->setFenString(m_fen);
			m_fen.clear();
		}
		if (!m_openingMoves.isEmpty())
		{
			game->setOpeningMoves(m_openingMoves);
			m_openingMoves.clear();
		}
	}
	else if (m_book != 0)
	{
		game->setOpeningBook(m_book, m_bookDepth);
	}
	else if (m_pgnInputStream.isOpen())
	{
		bool ok = game->read(m_pgnInputStream, PgnGame::Minimal, m_bookDepth);
		if (ok)
			m_pgnGamesRead++;
		// Rewind the PGN input file
		else if (m_pgnGamesRead > 0)
		{
			m_pgnInputStream.rewind();
			ok = game->read(m_pgnInputStream, PgnGame::Minimal, m_bookDepth);
			Q_ASSERT(ok);
			m_pgnGamesRead++;
		}
	}

	if (m_repeatOpening && (m_currentGame % 2) != 0)
	{
		m_fen = game->startingFen();
		m_openingMoves = game->moves();
	}

	game->setRound(m_currentGame);
	game->setEvent(m_event);
	game->setSite(m_site);

	game->setDrawThreshold(m_drawMoveNum, m_drawScore);
	game->setResignThreshold(m_resignMoveCount, m_resignScore);

	connect(game, SIGNAL(gameEnded()), this, SLOT(onGameEnded()));
	m_manager.newGame(game, white->builder, black->builder);
}

void EngineMatch::print(const QString& msg)
{
	qDebug("%d %s", m_startTime.elapsed(), qPrintable(msg));
}
