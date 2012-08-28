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


#include "tournament.h"
#include <QFile>
#include "gamemanager.h"
#include "playerbuilder.h"
#include "board/boardfactory.h"
#include "chessplayer.h"
#include "chessgame.h"
#include "pgnstream.h"
#include "openingsuite.h"

Tournament::Tournament(GameManager* gameManager, QObject *parent)
	: QObject(parent),
	  m_gameManager(gameManager),
	  m_lastGame(0),
	  m_variant("standard"),
	  m_round(0),
	  m_nextGameNumber(0),
	  m_finishedGameCount(0),
	  m_savedGameCount(0),
	  m_finalGameCount(0),
	  m_gamesPerEncounter(1),
	  m_roundMultiplier(1),
	  m_startDelay(0),
	  m_drawMoveNumber(0),
	  m_drawScore(0),
	  m_resignMoveCount(0),
	  m_resignScore(0),
	  m_openingDepth(1024),
	  m_stopping(false),
	  m_repeatOpening(false),
	  m_recover(false),
	  m_pgnCleanup(true),
	  m_finished(false),
	  m_openingSuite(0),
	  m_pgnOutMode(PgnGame::Verbose),
	  m_pair(QPair<int, int>(-1, -1))
{
	Q_ASSERT(gameManager != 0);
}

Tournament::~Tournament()
{
	if (!m_gameData.isEmpty())
		qWarning("Tournament: Destroyed while games are still running.");

	qDeleteAll(m_gameData);
	foreach (const PlayerData& data, m_players)
		delete data.builder;

	delete m_openingSuite;
}

GameManager* Tournament::gameManager() const
{
	return m_gameManager;
}

bool Tournament::isFinished() const
{
	return m_finished;
}

QString Tournament::name() const
{
	return m_name;
}

QString Tournament::site() const
{
	return m_site;
}

QString Tournament::variant() const
{
	return m_variant;
}

int Tournament::currentRound() const
{
	return m_round;
}

int Tournament::gamesPerEncounter() const
{
	return m_gamesPerEncounter;
}

int Tournament::roundMultiplier() const
{
	return m_roundMultiplier;
}

int Tournament::finishedGameCount() const
{
	return m_finishedGameCount;
}

int Tournament::finalGameCount() const
{
	return m_finalGameCount;
}

Tournament::PlayerData Tournament::playerAt(int index) const
{
	return m_players.at(index);
}

int Tournament::playerCount() const
{
	return m_players.size();
}

void Tournament::setName(const QString& name)
{
	m_name = name;
}

void Tournament::setSite(const QString& site)
{
	m_site = site;
}

void Tournament::setVariant(const QString& variant)
{
	Q_ASSERT(Chess::BoardFactory::variants().contains(variant));
	m_variant = variant;
}

void Tournament::setCurrentRound(int round)
{
	Q_ASSERT(round >= 1);
	m_round = round;
}

void Tournament::setGamesPerEncounter(int count)
{
	Q_ASSERT(count > 0);
	m_gamesPerEncounter = count;
}

void Tournament::setRoundMultiplier(int factor)
{
	Q_ASSERT(factor > 0);
	m_roundMultiplier = factor;
}

void Tournament::setStartDelay(int delay)
{
	Q_ASSERT(delay >= 0);
	m_startDelay = delay;
}

void Tournament::setRecoveryMode(bool recover)
{
	m_recover = recover;
}

void Tournament::setDrawThreshold(int moveNumber, int score)
{
	Q_ASSERT(moveNumber >= 0);
	Q_ASSERT(score >= 0);

	m_drawMoveNumber = moveNumber;
	m_drawScore = score;
}

void Tournament::setResignThreshold(int moveCount, int score)
{
	Q_ASSERT(moveCount >= 0);

	m_resignMoveCount = moveCount;
	m_resignScore = score;
}

void Tournament::setOpeningSuite(OpeningSuite *suite)
{
	delete m_openingSuite;
	m_openingSuite = suite;
}

void Tournament::setOpeningDepth(int plies)
{
	m_openingDepth = plies;
}

void Tournament::setPgnOutput(const QString& fileName, PgnGame::PgnMode mode)
{
	m_pgnout = fileName;
	m_pgnOutMode = mode;
}

void Tournament::setPgnCleanupEnabled(bool enabled)
{
	m_pgnCleanup = enabled;
}

void Tournament::setOpeningRepetition(bool repeat)
{
	m_repeatOpening = repeat;
}

void Tournament::addPlayer(PlayerBuilder* builder,
			   const TimeControl& timeControl,
			   const OpeningBook* book,
			   int bookDepth)
{
	Q_ASSERT(builder != 0);

	PlayerData data = { builder, timeControl, book, bookDepth, 0, 0, 0 };
	m_players.append(data);
}

void Tournament::startNextGame()
{
	if (m_stopping || m_nextGameNumber >= m_finalGameCount)
		return;

	if (m_nextGameNumber % m_gamesPerEncounter == 0)
	{
		m_pair = nextPair();

		if (m_players.size() > 2)
		{
			m_startFen.clear();
			m_openingMoves.clear();
		}
	}
	else
		m_pair = qMakePair(m_pair.second, m_pair.first);

	PlayerData& white = m_players[m_pair.first];
	PlayerData& black = m_players[m_pair.second];

	Chess::Board* board = Chess::BoardFactory::create(m_variant);
	Q_ASSERT(board != 0);
	ChessGame* game = new ChessGame(board, new PgnGame());

	connect(game, SIGNAL(started(ChessGame*)),
		this, SLOT(onGameStarted(ChessGame*)));
	connect(game, SIGNAL(finished(ChessGame*)),
		this, SLOT(onGameFinished(ChessGame*)));

	game->setTimeControl(white.timeControl, Chess::Side::White);
	game->setTimeControl(black.timeControl, Chess::Side::Black);

	game->setOpeningBook(white.book, Chess::Side::White, white.bookDepth);
	game->setOpeningBook(black.book, Chess::Side::Black, black.bookDepth);

	bool isRepeat = false;
	if (!m_startFen.isEmpty() || !m_openingMoves.isEmpty())
	{
		game->setStartingFen(m_startFen);
		game->setMoves(m_openingMoves);
		m_startFen.clear();
		m_openingMoves.clear();
		isRepeat = true;
	}
	else if (m_openingSuite != 0)
		game->setMoves(m_openingSuite->nextGame(m_openingDepth));

	game->generateOpening();
	if (m_repeatOpening && !isRepeat)
	{
		m_startFen = game->startingFen();
		m_openingMoves = game->moves();
	}

	game->pgn()->setEvent(m_name);
	game->pgn()->setSite(m_site);
	game->pgn()->setRound(m_round);

	game->setStartDelay(m_startDelay);

	game->setDrawThreshold(m_drawMoveNumber, m_drawScore);
	game->setResignThreshold(m_resignMoveCount, m_resignScore);

	GameData* data = new GameData;
	data->number = ++m_nextGameNumber;
	data->whiteIndex = m_pair.first;
	data->blackIndex = m_pair.second;
	m_gameData[game] = data;

	if (!m_gameManager->newGame(game,
				    white.builder,
				    black.builder,
				    GameManager::Enqueue,
				    GameManager::ReusePlayers))
	{
		delete game->pgn();
		game->deleteLater();
		m_gameData.remove(game);

		stop();
	}
}

void Tournament::onGameStarted(ChessGame* game)
{
	Q_ASSERT(game != 0);
	Q_ASSERT(m_gameData.contains(game));

	GameData* data = m_gameData[game];
	m_players[data->whiteIndex].builder->setName(game->player(Chess::Side::White)->name());
	m_players[data->blackIndex].builder->setName(game->player(Chess::Side::Black)->name());

	emit gameStarted(game, data->number, data->whiteIndex, data->blackIndex);
}

void Tournament::onGameFinished(ChessGame* game)
{
	Q_ASSERT(game != 0);

	PgnGame* pgn(game->pgn());
	Chess::Result result(game->result());

	m_finishedGameCount++;

	Q_ASSERT(m_gameData.contains(game));
	GameData* data = m_gameData.take(game);
	int gameNumber = data->number;

	switch (game->result().winner())
	{
	case Chess::Side::White:
		m_players[data->whiteIndex].wins++;
		m_players[data->blackIndex].losses++;
		break;
	case Chess::Side::Black:
		m_players[data->blackIndex].wins++;
		m_players[data->whiteIndex].losses++;
		break;
	default:
		if (game->result().isDraw())
		{
			m_players[data->whiteIndex].draws++;
			m_players[data->blackIndex].draws++;
		}
		break;
	}

	if (!m_pgnout.isEmpty())
	{
		m_pgnGames[gameNumber] = *pgn;
		while (m_pgnGames.contains(m_savedGameCount + 1))
		{
			PgnGame tmp = m_pgnGames.take(++m_savedGameCount);
			if (!tmp.write(m_pgnout, m_pgnOutMode))
				qWarning("Can't write to PGN file %s", qPrintable(m_pgnout));
		}
	}
	if (m_pgnCleanup)
		delete pgn;

	Chess::Result::Type resultType(game->result().type());
	bool crashed = (resultType == Chess::Result::Disconnection ||
			resultType == Chess::Result::StalledConnection);
	if (!m_recover && crashed)
		stop();

	emit gameFinished(game, gameNumber, data->whiteIndex, data->blackIndex);

	if (m_finishedGameCount == m_finalGameCount
	||  (m_stopping && m_gameData.isEmpty()))
	{
		m_stopping = false;
		m_lastGame = game;
		connect(m_gameManager, SIGNAL(gameDestroyed(ChessGame*)),
			this, SLOT(onGameDestroyed(ChessGame*)));
	}

	delete data;
	game->deleteLater();
}

void Tournament::onGameDestroyed(ChessGame* game)
{
	if (game != m_lastGame)
		return;

	m_lastGame = 0;
	m_gameManager->cleanupIdleThreads();
	m_finished = true;
	emit finished();
}

void Tournament::start()
{
	Q_ASSERT(m_players.size() > 1);

	m_round = 1;
	m_nextGameNumber = 0;
	m_finishedGameCount = 0;
	m_savedGameCount = 0;
	m_finalGameCount = 0;
	m_stopping = false;

	m_gameData.clear();
	m_pgnGames.clear();
	m_startFen.clear();
	m_openingMoves.clear();

	connect(m_gameManager, SIGNAL(ready()),
		this, SLOT(startNextGame()));

	initializePairing();
	m_finalGameCount = gamesPerCycle() * gamesPerEncounter() * roundMultiplier();

	startNextGame();
}

void Tournament::stop()
{
	if (m_stopping)
		return;

	disconnect(m_gameManager, SIGNAL(ready()),
		   this, SLOT(startNextGame()));

	if (m_gameData.isEmpty())
	{
		m_gameManager->cleanupIdleThreads();
		m_finished = true;
		emit finished();
		return;
	}

	m_stopping = true;
	foreach (ChessGame* game, m_gameData.keys())
		QMetaObject::invokeMethod(game, "stop", Qt::QueuedConnection);
}
