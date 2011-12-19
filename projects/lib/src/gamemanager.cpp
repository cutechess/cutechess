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

#include "gamemanager.h"
#include <QThread>
#include <QtDebug>
#include "playerbuilder.h"
#include "chessgame.h"
#include "chessplayer.h"


class GameThread : public QThread
{
	Q_OBJECT

	public:
		GameThread(const PlayerBuilder* whiteBuilder,
			   const PlayerBuilder* blackBuilder,
			   QObject* parent);
		virtual ~GameThread();

		bool isReady() const;
		bool newGame(ChessGame* game);
		void swapSides();
		void quitPlayers();

		ChessGame* game() const;
		const PlayerBuilder* whiteBuilder() const;
		const PlayerBuilder* blackBuilder() const;
		GameManager::StartMode startMode() const;
		GameManager::CleanupMode cleanupMode() const;

		void setStartMode(GameManager::StartMode mode);
		void setCleanupMode(GameManager::CleanupMode mode);

	signals:
		void ready();

	private slots:
		void onGameDestroyed();
		void onPlayerQuit();

	private:
		bool m_ready;
		bool m_quitting;
		GameManager::StartMode m_startMode;
		GameManager::CleanupMode m_cleanupMode;
		int m_playerCount;
		ChessGame* m_game;
		ChessPlayer* m_player[2];
		const PlayerBuilder* m_builder[2];
};

GameThread::GameThread(const PlayerBuilder* whiteBuilder,
		       const PlayerBuilder* blackBuilder,
		       QObject* parent)
	: QThread(parent),
	  m_ready(true),
	  m_quitting(false),
	  m_startMode(GameManager::StartImmediately),
	  m_cleanupMode(GameManager::DeletePlayers),
	  m_playerCount(0),
	  m_game(0)
{
	Q_ASSERT(parent != 0);
	Q_ASSERT(whiteBuilder != 0);
	Q_ASSERT(blackBuilder != 0);

	m_player[Chess::Side::White] = 0;
	m_player[Chess::Side::Black] = 0;
	m_builder[Chess::Side::White] = whiteBuilder;
	m_builder[Chess::Side::Black] = blackBuilder;
}

GameThread::~GameThread()
{
	for (int i = 0; i < 2; i++)
	{
		if (m_player[i] == 0)
			continue;

		m_player[i]->disconnect();
		m_player[i]->kill();
		delete m_player[i];
	}
}

bool GameThread::isReady() const
{
	return m_ready;
}

bool GameThread::newGame(ChessGame* game)
{
	m_ready = false;
	m_game = game;
	m_game->moveToThread(this);
	connect(game, SIGNAL(destroyed()), this, SLOT(onGameDestroyed()));

	for (int i = 0; i < 2; i++)
	{
		// Delete a disconnected player (crashed engine) so that
		// it will be restarted.
		if (m_player[i] != 0
		&&  m_player[i]->state() == ChessPlayer::Disconnected)
		{
			m_player[i]->deleteLater();
			m_player[i] = 0;
		}

		if (m_player[i] == 0)
		{
			m_player[i] = m_builder[i]->create(parent(), SIGNAL(debugMessage(QString)));
			if (m_player[i] == 0)
			{
				m_ready = true;
				m_playerCount = 0;

				int j = !i;
				if (m_player[j] != 0)
				{
					m_player[j]->kill();
					delete m_player[j];
					m_player[j] = 0;
				}

				return false;
			}

			m_player[i]->moveToThread(this);
		}
		m_game->setPlayer(Chess::Side::Type(i), m_player[i]);
	}
	m_playerCount = 2;

	return true;
}

void GameThread::swapSides()
{
	qSwap(m_player[0], m_player[1]);
	qSwap(m_builder[0], m_builder[1]);
}

void GameThread::quitPlayers()
{
	if (m_quitting)
		return;
	m_quitting = true;

	if (m_playerCount <= 0)
	{
		quit();
		return;
	}

	for (int i = 0; i < 2; i++)
	{
		if (m_player[i] == 0)
			continue;

		connect(m_player[i], SIGNAL(disconnected()),
			this, SLOT(onPlayerQuit()), Qt::QueuedConnection);
		QMetaObject::invokeMethod(m_player[i], "quit", Qt::QueuedConnection);
	}
}

ChessGame* GameThread::game() const
{
	return m_game;
}

const PlayerBuilder* GameThread::whiteBuilder() const
{
	return m_builder[Chess::Side::White];
}

const PlayerBuilder* GameThread::blackBuilder() const
{
	return m_builder[Chess::Side::Black];
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

void GameThread::onPlayerQuit()
{
	if (--m_playerCount <= 0)
		quit();
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
		Q_ASSERT(thread != 0);

		if (thread->isReady())
		{
			it = m_activeThreads.erase(it);

			connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
			thread->quitPlayers();
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
		if (*it == 0 || !(*it)->isRunning())
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
	foreach (GameThread* thread, m_threads)
	{
		connect(thread, SIGNAL(finished()), this, SLOT(onThreadQuit()),
			Qt::QueuedConnection);
		thread->quitPlayers();
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

bool GameManager::newGame(ChessGame* game,
			  const PlayerBuilder* white,
			  const PlayerBuilder* black,
			  StartMode startMode,
			  CleanupMode cleanupMode)
{
	Q_ASSERT(game != 0);
	Q_ASSERT(white != 0);
	Q_ASSERT(black != 0);
	Q_ASSERT(game->parent() == 0);

	GameEntry entry = { game, white, black, startMode, cleanupMode };

	if (startMode == StartImmediately)
		return startGame(entry);

	m_gameEntries << entry;
	return startQueuedGame();
}

void GameManager::onThreadQuit()
{
	GameThread* thread = qobject_cast<GameThread*>(QObject::sender());
	Q_ASSERT(thread != 0);

	m_threads.removeOne(thread);
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
	Q_ASSERT(thread != 0);
	ChessGame* game = thread->game();

	m_activeGames.removeOne(game);
	m_threads.removeAll(0);

	if (thread->cleanupMode() == DeletePlayers)
	{
		m_activeThreads.removeOne(thread);

		delete thread->whiteBuilder();
		delete thread->blackBuilder();

		connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
		thread->quitPlayers();
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

GameThread* GameManager::getThread(const PlayerBuilder* white,
				   const PlayerBuilder* black)
{
	Q_ASSERT(white != 0);
	Q_ASSERT(black != 0);

	foreach (GameThread* thread, m_activeThreads)
	{
		if (!thread->isReady())
			continue;

		if (thread->whiteBuilder() == black
		&&  thread->blackBuilder() == white)
			thread->swapSides();
		if (thread->whiteBuilder() == white && thread->blackBuilder() == black)
			return thread;
	}

	GameThread* gameThread = new GameThread(white, black, this);
	m_threads << gameThread;
	m_activeThreads << gameThread;
	connect(gameThread, SIGNAL(ready()),
		this, SLOT(onThreadReady()));

	return gameThread;
}

bool GameManager::startGame(const GameEntry& entry)
{
	GameThread* gameThread = getThread(entry.white, entry.black);
	Q_ASSERT(gameThread != 0);

	gameThread->setStartMode(entry.startMode);
	gameThread->setCleanupMode(entry.cleanupMode);

	if (!gameThread->newGame(entry.game))
	{
		m_threads.removeOne(gameThread);
		m_activeThreads.removeOne(gameThread);
		gameThread->deleteLater();
		return false;
	}
	m_activeGames << entry.game;
	if (entry.startMode == Enqueue)
	{
		m_activeQueuedGameCount++;
		cleanupIdleThreads();
	}

	connect(entry.game, SIGNAL(started(ChessGame*)),
		this, SIGNAL(gameStarted(ChessGame*)),
		Qt::QueuedConnection);
	gameThread->start();
	entry.game->start();

	return true;
}

bool GameManager::startQueuedGame()
{
	if (m_activeQueuedGameCount >= m_concurrency)
		return true;
	if (m_gameEntries.isEmpty())
	{
		emit ready();
		return true;
	}

	if (!startGame(m_gameEntries.takeFirst()))
		return false;

	return startQueuedGame();
}

#include "gamemanager.moc"
