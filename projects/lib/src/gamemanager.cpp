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
		GameThread(ChessGame* game,
			   ChessPlayer* white,
			   ChessPlayer* black,
			   const PlayerBuilder* whiteBuilder,
			   const PlayerBuilder* blackBuilder,
			   QObject* parent = 0)
			: QThread(parent),
			  m_game(game),
			  m_white(white),
			  m_black(black),
			  m_whiteBuilder(whiteBuilder),
			  m_blackBuilder(blackBuilder)
		{
			Q_ASSERT(game != 0);
			Q_ASSERT(white != 0);
			Q_ASSERT(black != 0);
			Q_ASSERT(whiteBuilder != 0);
			Q_ASSERT(blackBuilder != 0);
		}

		ChessGame* game() const { return m_game; }
		ChessPlayer* white() const { return m_white; }
		ChessPlayer* black() const { return m_black; }
		const PlayerBuilder* whiteBuilder() const { return m_whiteBuilder; }
		const PlayerBuilder* blackBuilder() const { return m_blackBuilder; }

	protected:
		void run() { exec(); }

	private:
		ChessGame* m_game;
		ChessPlayer* m_white;
		ChessPlayer* m_black;
		const PlayerBuilder* m_whiteBuilder;
		const PlayerBuilder* m_blackBuilder;
};


GameManager::GameManager(QObject* parent)
	: QObject(parent),
	  m_finishing(false),
	  m_concurrency(1),
	  m_activeGameCount(0)
{
}

GameManager::~GameManager()
{
	QList<PlayerData>::iterator it;
	for (it = m_idlePlayers.begin(); it != m_idlePlayers.end(); ++it)
		it->player->deleteLater();
}

int GameManager::concurrency() const
{
	return m_concurrency;
}

void GameManager::setConcurrency(int concurrency)
{
	m_concurrency = concurrency;
}

void GameManager::cleanup()
{
	m_finishing = false;

	if (m_idlePlayers.isEmpty())
	{
		emit finished();
		return;
	}

	// Terminate players
	QList<PlayerData>::iterator it;
	for (it = m_idlePlayers.begin(); it != m_idlePlayers.end(); ++it)
	{
		connect(it->player, SIGNAL(disconnected()),
			this, SLOT(onPlayerQuit()), Qt::QueuedConnection);
		it->player->quit();
	}
}

void GameManager::finish()
{
	m_gameEntries.clear();
	if (m_activeGameCount == 0)
		cleanup();
	else
		m_finishing = true;
}

bool GameManager::newGame(ChessGame* game,
			  const PlayerBuilder* white,
			  const PlayerBuilder* black)
{
	Q_ASSERT(game != 0);
	Q_ASSERT(white != 0);
	Q_ASSERT(black != 0);

	GameEntry entry = { game, white, black };
	m_gameEntries << entry;
	return startGame();
}

void GameManager::onPlayerQuit()
{
	Q_ASSERT(!m_idlePlayers.isEmpty());
	ChessPlayer* player = qobject_cast<ChessPlayer*>(QObject::sender());
	Q_ASSERT(player != 0);

	QList<PlayerData>::iterator it;
	for (it = m_idlePlayers.begin(); it != m_idlePlayers.end(); ++it)
	{
		if (it->player == player)
		{
			m_idlePlayers.erase(it);
			break;
		}
	}
	player->deleteLater();

	if (m_idlePlayers.isEmpty())
	{
		m_finishing = false;
		emit finished();
	}
}

void GameManager::onThreadQuit()
{
	GameThread* gameThread = qobject_cast<GameThread*>(QObject::sender());
	Q_ASSERT(gameThread != 0);

	m_activeGameCount--;

	PlayerData white = { gameThread->whiteBuilder(), gameThread->white() };
	PlayerData black = { gameThread->blackBuilder(), gameThread->black() };
	m_idlePlayers << white << black;

	delete gameThread;
	startGame();

	if (m_finishing && m_activeGameCount == 0)
		cleanup();
}

ChessPlayer* GameManager::getPlayer(const PlayerBuilder* builder)
{
	ChessPlayer* player = 0;
	QList<PlayerData>::iterator it;

	for (it = m_idlePlayers.begin(); it != m_idlePlayers.end(); ++it)
	{
		if (it->builder == builder)
		{
			player = it->player;
			m_idlePlayers.erase(it);
			break;
		}
	}

	if (player == 0)
	{
		player = builder->create();
		if (player == 0)
			return 0;
		connect(player, SIGNAL(debugMessage(QString)),
			this, SIGNAL(debugMessage(QString)));
	}

	return player;
}

bool GameManager::startGame()
{
	if (m_activeGameCount >= m_concurrency)
		return true;
	if (m_gameEntries.isEmpty())
	{
		emit ready();
		return true;
	}

	GameEntry entry = m_gameEntries.takeFirst();
	ChessPlayer* white = 0;
	ChessPlayer* black = 0;
	if ((white = getPlayer(entry.white)) == 0
	||  (black = getPlayer(entry.black)) == 0)
		return false;

	ChessGame* game = entry.game;
	m_activeGameCount++;

	GameThread* gameThread = new GameThread(game, white, black, entry.white, entry.black, this);
	connect(game, SIGNAL(gameEnded()), gameThread, SLOT(quit()));
	connect(gameThread, SIGNAL(finished()), this, SLOT(onThreadQuit()));
	gameThread->start();
	
	game->setPlayer(Chess::Side::White, white);
	game->setPlayer(Chess::Side::Black, black);
	game->start(gameThread);

	return startGame();
}

#include "gamemanager.moc"
