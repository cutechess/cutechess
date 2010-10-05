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
#include <QTimer>
#include <QtDebug>
#include <board/boardfactory.h>
#include <chessplayer.h>
#include <chessgame.h>
#include <enginebuilder.h>
#include <polyglotbook.h>


EngineMatch::EngineMatch(QObject* parent)
	: QObject(parent),
	  m_gameCount(1),
	  m_drawCount(0),
	  m_currentGame(0),
	  m_finishedGames(0),
	  m_pgnDepth(1000),
	  m_pgnGamesRead(0),
	  m_drawMoveNum(0),
	  m_drawScore(0),
	  m_resignMoveCount(0),
	  m_resignScore(0),
	  m_wait(0),
	  m_debug(false),
	  m_recover(false),
	  m_finishing(false),
	  m_pgnMode(PgnGame::Verbose),
	  m_repeatOpening(false),
	  m_variant("standard")
{
	m_startTime.start();
}

EngineMatch::~EngineMatch()
{
	qDeleteAll(m_books);
	qDeleteAll(m_builders);
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
			    const TimeControl& timeControl,
			    const QString& book,
			    int bookDepth)
{
	// We don't allow more than 2 engines at this point
	if (m_engines.size() >= 2)
	{
		qWarning() << "Only two engines can be added";
		return;
	}
	if (engineConfig.command().isEmpty())
		return;
	EngineData data = { engineConfig, timeControl, 0, book, bookDepth, 0, 0 };
	m_engines.append(data);
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

bool EngineMatch::setPgnDepth(int depth)
{
	if (depth <= 0)
		return false;

	m_pgnDepth = depth;
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

void EngineMatch::setRecoveryMode(bool recover)
{
	m_recover = recover;
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

bool EngineMatch::setVariant(const QString& variant)
{
	if (!Chess::BoardFactory::variants().contains(variant))
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

bool EngineMatch::loadOpeningBook(const QString& filename,
				  OpeningBook** book)
{
	*book = new PolyglotBook;
	if (!(*book)->read(filename))
	{
		delete *book;
		*book = 0;
		qWarning() << "Can't open book file" << filename;
		return false;
	}

	m_books << *book;
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

	QVector<EngineData>::iterator it;

	if (m_engines[0].bookFile == m_engines[1].bookFile
	&&  !m_engines[0].bookFile.isEmpty())
	{
		if (!loadOpeningBook(m_engines[0].bookFile, &m_engines[0].book))
			return false;
		m_engines[1].book = m_engines[0].book;
	}
	else
	{
		for (it = m_engines.begin(); it != m_engines.end(); ++it)
		{
			if (it->bookFile.isEmpty())
				continue;
			if (!loadOpeningBook(it->bookFile, &it->book))
				return false;
		}
	}

	for (it = m_engines.begin(); it != m_engines.end(); ++it)
	{
		if (!it->tc.isValid())
		{
			qWarning() << "Invalid or missing time control";
			return false;
		}
		if (it->config.protocol().isEmpty())
		{
			qWarning() << "Missing chess protocol";
			return false;
		}

		it->wins = 0;
		it->builder = new EngineBuilder(it->config);
		m_builders << it->builder;
	}

	connect(&m_manager, SIGNAL(ready()), this, SLOT(onManagerReady()));
	connect(&m_manager, SIGNAL(finished()), this, SIGNAL(finished()),
		Qt::QueuedConnection);

	if (m_debug)
		connect(&m_manager, SIGNAL(debugMessage(QString)),
			this, SLOT(print(QString)));

	return true;
}

void EngineMatch::onGameEnded()
{
	ChessGame* game = qobject_cast<ChessGame*>(QObject::sender());
	Q_ASSERT(game != 0);

	PgnGame* pgn(game->pgn());
	Chess::Result result(game->result());
	bool playerMissing(game->player(Chess::Side::White) == 0 ||
			   game->player(Chess::Side::Black) == 0);

	game->deleteLater();
	game = 0;

	if (playerMissing || result.type() == Chess::Result::ResultError)
	{
		delete pgn;
		m_finishedGames++;
		stop();
		return;
	}

	int gameId = pgn->round();
	int wIndex = !(gameId % 2);

	qDebug("Game %d ended: %s", gameId, qPrintable(result.toVerboseString()));
	if (result.isDraw())
		m_drawCount++;
	else if (!result.isNone())
	{
		qDebug("%s wins the game as %s",
		       qPrintable(pgn->playerName(result.winner())),
		       qPrintable(result.winner().toString()));
		m_engines[wIndex ^ result.winner()].wins++;
	}

	qDebug("Score of %s vs %s: %d - %d - %d",
	       qPrintable(pgn->playerName(Chess::Side::Type(wIndex))),
	       qPrintable(pgn->playerName(Chess::Side::Type(!wIndex))),
	       m_engines[0].wins, m_engines[1].wins, m_drawCount);

	m_games[gameId] = pgn;
	while (m_games.contains(m_finishedGames + 1))
	{
		m_finishedGames++;
		pgn = m_games.take(m_finishedGames);

		if (!m_pgnOutput.isEmpty())
			pgn->write(m_pgnOutput, m_pgnMode);

		delete pgn;
	}

	if (m_finishedGames >= m_gameCount
	||  result.type() == Chess::Result::NoResult
	||  (!m_recover &&
	     (result.type() == Chess::Result::Disconnection ||
	      result.type() == Chess::Result::StalledConnection)))
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

	Chess::Board* board = Chess::BoardFactory::create(m_variant);
	Q_ASSERT(board != 0);
	ChessGame* game = new ChessGame(board, new PgnGame);
	board->setParent(game);
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

	game->setTimeControl(white->tc, Chess::Side::White);
	game->setTimeControl(black->tc, Chess::Side::Black);

	game->setOpeningBook(white->book, Chess::Side::White, white->bookDepth);
	game->setOpeningBook(black->book, Chess::Side::Black, black->bookDepth);

	if (!m_fen.isEmpty() || !m_openingMoves.isEmpty())
	{
		game->setStartingFen(m_fen);
		m_fen.clear();
		game->setMoves(m_openingMoves);
		m_openingMoves.clear();
	}
	else if (m_pgnInputStream.isOpen())
	{
		PgnGame pgn;
		if (pgn.read(m_pgnInputStream, m_pgnDepth))
			m_pgnGamesRead++;
		// Rewind the PGN input file
		else if (m_pgnGamesRead > 0)
		{
			m_pgnInputStream.rewind();
			bool ok = pgn.read(m_pgnInputStream, m_pgnDepth);
			Q_ASSERT(ok);
			Q_UNUSED(ok);
			m_pgnGamesRead++;
		}
		game->setMoves(pgn);
	}

	game->generateOpening();

	if (m_repeatOpening && (m_currentGame % 2) != 0)
	{
		m_fen = game->startingFen();
		m_openingMoves = game->moves();
	}

	game->pgn()->setRound(m_currentGame);
	game->pgn()->setEvent(m_event);
	game->pgn()->setSite(m_site);

	game->setDrawThreshold(m_drawMoveNum, m_drawScore);
	game->setResignThreshold(m_resignMoveCount, m_resignScore);

	connect(game, SIGNAL(gameEnded()), this, SLOT(onGameEnded()));
	if (!m_manager.newGame(game, white->builder, black->builder))
		stop();
}

void EngineMatch::print(const QString& msg)
{
	qDebug("%d %s", m_startTime.elapsed(), qPrintable(msg));
}
