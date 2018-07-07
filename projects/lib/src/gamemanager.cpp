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

#include "gamemanager.h"
#include <QThread>
#include <algorithm>
#include "playerbuilder.h"
#include "chessgame.h"
#include "chessplayer.h"

class GameInitializer : public QObject
{
	Q_OBJECT

	public:
		GameInitializer(const PlayerBuilder* white,
				const PlayerBuilder* black);
		virtual ~GameInitializer();

		const PlayerBuilder* whiteBuilder() const;
		const PlayerBuilder* blackBuilder() const;
		void swapPlayers();
		void setGame(ChessGame* game);

	public slots:
		void initializeGame();
		void finish();

	signals:
		void gameInitialized(bool success);
		void finished();

	private slots:
		void onPlayerQuit();

	private:
		void deletePlayer(int index);

		int m_playerCount;
		bool m_finishing;
		const PlayerBuilder* m_builder[2];
		ChessPlayer* m_player[2];
		ChessGame* m_game;
};

GameInitializer::GameInitializer(const PlayerBuilder* white,
				 const PlayerBuilder* black)
	: m_playerCount(0),
	  m_finishing(false),
	  m_game(nullptr)
{
	Q_ASSERT(white != nullptr);
	Q_ASSERT(black != nullptr);

	m_builder[Chess::Side::White] = white;
	m_builder[Chess::Side::Black] = black;
	m_player[0] = nullptr;
	m_player[1] = nullptr;
}

GameInitializer::~GameInitializer()
{
	for (int i = 0; i < 2; i++)
	{
		if (m_player[i] == nullptr)
			continue;

		m_player[i]->disconnect();
		m_player[i]->kill();
	}
}

const PlayerBuilder* GameInitializer::whiteBuilder() const
{
	return m_builder[Chess::Side::White];
}

const PlayerBuilder* GameInitializer::blackBuilder() const
{
	return m_builder[Chess::Side::Black];
}

void GameInitializer::swapPlayers()
{
	std::swap(m_builder[0], m_builder[1]);
	std::swap(m_player[0], m_player[1]);
}

void GameInitializer::setGame(ChessGame* game)
{
	m_game = game;
}

void GameInitializer::deletePlayer(int index)
{
	ChessPlayer* player = m_player[index];
	if (player == nullptr)
		return;

	m_player[index] = nullptr;
	if (player->state() == ChessPlayer::Disconnected)
		player->deleteLater();
	else
	{
		connect(player, SIGNAL(disconnected()),
			player, SLOT(deleteLater()));
		player->kill();
	}
}

void GameInitializer::initializeGame()
{
	for (int i = 0; i < 2; i++)
	{
		// Delete a disconnected player (crashed engine) so that
		// it will be restarted.
		if (m_player[i] != nullptr
		&&  m_player[i]->state() == ChessPlayer::Disconnected)
		{
			deletePlayer(i);
		}

		if (m_player[i] == nullptr)
		{
			QString error;
			m_player[i] = m_builder[i]->create(thread()->parent(),
							   SIGNAL(debugMessage(QString)),
							   this, &error);
			m_game->setError(error);

			if (m_player[i] == nullptr)
			{
				m_playerCount = 0;
				deletePlayer(!i);

				emit gameInitialized(false);
				return;
			}
		}
		m_game->setPlayer(Chess::Side::Type(i), m_player[i]);
	}
	m_playerCount = 2;

	emit gameInitialized(true);
}

void GameInitializer::finish()
{
	if (m_finishing)
		return;
	m_finishing = true;

	if (m_playerCount <= 0)
	{
		emit finished();
		return;
	}

	for (int i = 0; i < 2; i++)
	{
		if (m_player[i] == nullptr)
			continue;

		connect(m_player[i], SIGNAL(disconnected()),
			this, SLOT(onPlayerQuit()),
			Qt::QueuedConnection);
		m_player[i]->quit();
	}
}

void GameInitializer::onPlayerQuit()
{
	if (--m_playerCount <= 0)
		emit finished();
}


class GameThread : public QThread
{
	Q_OBJECT

	public:
		GameThread(const PlayerBuilder* white,
			   const PlayerBuilder* black,
			   QObject* parent);
		virtual ~GameThread();

		bool isReady() const;
		void newGame(ChessGame* game);
		void finish();
		void finishAndDelete();

		GameInitializer* initializer() const;
		ChessGame* game() const;
		GameManager::StartMode startMode() const;
		GameManager::CleanupMode cleanupMode() const;

		void setStartMode(GameManager::StartMode mode);
		void setCleanupMode(GameManager::CleanupMode mode);

	signals:
		void gameInitialized(bool success);
		void ready();

	private slots:
		void onGameDestroyed();

	private:
		bool m_ready;
		GameManager::StartMode m_startMode;
		GameManager::CleanupMode m_cleanupMode;
		ChessGame* m_game;
		GameInitializer* m_initializer;
};

GameThread::GameThread(const PlayerBuilder* white,
		       const PlayerBuilder* black,
		       QObject* parent)
	: QThread(parent),
	  m_ready(true),
	  m_startMode(GameManager::StartImmediately),
	  m_cleanupMode(GameManager::DeletePlayers),
	  m_game(nullptr),
	  m_initializer(new GameInitializer(white, black))
{
	connect(m_initializer, SIGNAL(gameInitialized(bool)),
		this, SIGNAL(gameInitialized(bool)));
	connect(m_initializer, SIGNAL(finished()),
		m_initializer, SLOT(deleteLater()),
		Qt::QueuedConnection);
	connect(m_initializer, SIGNAL(destroyed()),
		this, SLOT(quit()),
		Qt::QueuedConnection);
	m_initializer->moveToThread(this);
}

GameThread::~GameThread()
{
}

bool GameThread::isReady() const
{
	return m_ready;
}

void GameThread::newGame(ChessGame* game)
{
	m_ready = false;
	m_game = game;
	connect(game, SIGNAL(destroyed()),
		this, SLOT(onGameDestroyed()),
		Qt::QueuedConnection);

	m_initializer->setGame(m_game);
	QMetaObject::invokeMethod(m_initializer, "initializeGame",
				  Qt::QueuedConnection);
}

void GameThread::finish()
{
	if (m_initializer == nullptr)
		return;

	if (m_cleanupMode == GameManager::DeletePlayers)
	{
		delete m_initializer->whiteBuilder();
		delete m_initializer->blackBuilder();
	}

	QMetaObject::invokeMethod(m_initializer, "finish",
				  Qt::QueuedConnection);
	m_initializer = nullptr;
}

void GameThread::finishAndDelete()
{
	connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
	finish();
}

GameInitializer* GameThread::initializer() const
{
	return m_initializer;
}

ChessGame* GameThread::game() const
{
	return m_game;
}

GameManager::StartMode GameThread::startMode() const
{
	return m_startMode;
}

GameManager::CleanupMode GameThread::cleanupMode() const
{
	return m_cleanupMode;
}

void GameThread::setStartMode(GameManager::StartMode mode)
{
	m_startMode = mode;
}

void GameThread::setCleanupMode(GameManager::CleanupMode mode)
{
	m_cleanupMode = mode;
}

void GameThread::onGameDestroyed()
{
	m_ready = true;
	emit ready();
}


GameManager::GameManager(QObject* parent)
	: QObject(parent),
	  m_finishing(false),
	  m_concurrency(1),
	  m_activeQueuedGameCount(0)
{
}

QList<ChessGame*> GameManager::activeGames() const
{
	return m_activeGames;
}

int GameManager::concurrency() const
{
	return m_concurrency;
}

void GameManager::setConcurrency(int concurrency)
{
	m_concurrency = concurrency;
}

void GameManager::cleanupIdleThreads()
{
	QList<GameThread*>::iterator it = m_activeThreads.begin();
	while (it != m_activeThreads.end())
	{
		GameThread* thread = *it;
		Q_ASSERT(thread != nullptr);

		if (thread->isReady())
		{
			it = m_activeThreads.erase(it);
			thread->finishAndDelete();
		}
		else
			++it;
	}
}

void GameManager::cleanup()
{
	m_finishing = false;

	// Remove terminated threads from the list
	QList< QPointer<GameThread> >::iterator it = m_threads.begin();
	while (it != m_threads.end())
	{
		if (*it == nullptr || !(*it)->isRunning())
			it = m_threads.erase(it);
		else
			++it;
	}

	if (m_threads.isEmpty())
	{
		emit finished();
		return;
	}

	// Terminate running threads
	for (GameThread* thread : qAsConst(m_threads))
	{
		connect(thread, SIGNAL(finished()), this, SLOT(onThreadQuit()),
			Qt::QueuedConnection);
		thread->finish();
	}
}

void GameManager::finish()
{
	m_gameEntries.clear();
	if (m_activeGames.isEmpty())
		cleanup();
	else
		m_finishing = true;
}

void GameManager::newGame(ChessGame* game,
			  const PlayerBuilder* white,
			  const PlayerBuilder* black,
			  StartMode startMode,
			  CleanupMode cleanupMode)
{
	Q_ASSERT(game != nullptr);
	Q_ASSERT(white != nullptr);
	Q_ASSERT(black != nullptr);
	Q_ASSERT(game->parent() == nullptr);

	GameEntry entry = { game, white, black, startMode, cleanupMode };
	if (!white->isHuman() && black->isHuman())
		game->setBoardShouldBeFlipped(true);

	if (startMode == StartImmediately)
	{
		startGame(entry);
		return;
	}

	m_gameEntries << entry;
	startQueuedGame();
}

void GameManager::onThreadQuit()
{
	GameThread* thread = qobject_cast<GameThread*>(QObject::sender());
	m_threads.removeOne(thread);

	if (thread != nullptr)
		thread->deleteLater();

	if (m_threads.isEmpty())
	{
		m_finishing = false;
		emit finished();
	}
}

void GameManager::onThreadReady()
{
	GameThread* thread = qobject_cast<GameThread*>(QObject::sender());
	Q_ASSERT(thread != nullptr);
	ChessGame* game = thread->game();

	m_activeGames.removeOne(game);
	m_threads.removeAll(nullptr);

	if (thread->cleanupMode() == DeletePlayers)
	{
		m_activeThreads.removeOne(thread);
		thread->finishAndDelete();
	}

	if (thread->startMode() == Enqueue)
	{
		m_activeQueuedGameCount--;
		startQueuedGame();
	}

	emit gameDestroyed(game);
	if (m_finishing && m_activeGames.isEmpty())
		cleanup();
}

void GameManager::onGameInitialized(bool success)
{
	GameThread* gameThread = qobject_cast<GameThread*>(sender());
	Q_ASSERT(gameThread != nullptr);
	ChessGame* game = gameThread->game();

	if (!success)
	{
		if (gameThread->startMode() == Enqueue)
			m_activeQueuedGameCount--;

		m_threads.removeOne(gameThread);
		m_activeThreads.removeOne(gameThread);

		connect(gameThread, SIGNAL(destroyed()),
			game, SLOT(emitStartFailed()));
		gameThread->finishAndDelete();

		return;
	}

	m_activeGames << game;
	if (gameThread->startMode() == Enqueue)
		cleanupIdleThreads();

	game->moveToThread(gameThread);
	connect(game, SIGNAL(started(ChessGame*)),
		this, SIGNAL(gameStarted(ChessGame*)),
		Qt::QueuedConnection);
	QMetaObject::invokeMethod(game, "start", Qt::QueuedConnection);

	startQueuedGame();
}

GameThread* GameManager::getThread(const PlayerBuilder* white,
				   const PlayerBuilder* black)
{
	Q_ASSERT(white != nullptr);
	Q_ASSERT(black != nullptr);

	for (GameThread* thread : qAsConst(m_activeThreads))
	{
		if (!thread->isReady())
			continue;

		GameInitializer* tmp = thread->initializer();
		if (tmp->whiteBuilder() == black
		&&  tmp->blackBuilder() == white)
			tmp->swapPlayers();
		if (tmp->whiteBuilder() == white && tmp->blackBuilder() == black)
			return thread;
	}

	GameThread* gameThread = new GameThread(white, black, this);
	m_threads << gameThread;
	m_activeThreads << gameThread;
	connect(gameThread, SIGNAL(ready()),
		this, SLOT(onThreadReady()));
	connect(gameThread, SIGNAL(gameInitialized(bool)),
		this, SLOT(onGameInitialized(bool)),
		Qt::QueuedConnection);

	gameThread->start();
	return gameThread;
}

void GameManager::startGame(const GameEntry& entry)
{
	GameThread* gameThread = getThread(entry.white, entry.black);
	Q_ASSERT(gameThread != nullptr);

	gameThread->setStartMode(entry.startMode);
	gameThread->setCleanupMode(entry.cleanupMode);
	gameThread->newGame(entry.game);
}

void GameManager::startQueuedGame()
{
	if (m_activeQueuedGameCount >= m_concurrency)
		return;
	if (m_gameEntries.isEmpty())
	{
		emit ready();
		return;
	}

	m_activeQueuedGameCount++;
	startGame(m_gameEntries.takeFirst());
}

#include "gamemanager.moc"
