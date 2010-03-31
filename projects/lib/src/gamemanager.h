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

#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include <QList>
class ChessGame;
class ChessPlayer;
class PlayerBuilder;


/*!
 * \brief A class for managing chess games and players
 *
 * GameManager can start games in a new thread, run
 * multiple games concurrently, and queue games to be
 * run when a game slot/thread is free.
 *
 * \sa ChessGame, PlayerBuilder
 */
class LIB_EXPORT GameManager : public QObject
{
	Q_OBJECT

	public:
		/*! Creates a new game manager. */
		GameManager(QObject* parent = 0);
		/*! Destructs the manager and deletes all idle players. */
		~GameManager();

		/*! Returns the maximum allowed number of concurrent games. */
		int concurrency() const;
		/*! Sets concurrency to \a concurrency. */
		void setConcurrency(int concurrency);

		/*!
		 * Removes all future games from the queue, waits for
		 * ongoing games to end, and deletes all idle players.
		 * Emits the finished() signal when done.
		 */
		void finish();

		/*!
		 * Adds game \a game to the game queue.
		 *
		 * The game is started as soon as free game slots are available.
		 * Construction of the players is delayed to the moment when the
		 * game starts. Idle players from a previous game are reused
		 * instead of constructing new players if the same builders are
		 * used again.
		 *
		 * Returns true if successfull.
		 * \note If there are still free game slots after starting this
		 * game, the ready() signal is emitted immediately.
		 * \note \a game must not have a parent.
		 */
		bool newGame(ChessGame* game,
			     const PlayerBuilder* white,
			     const PlayerBuilder* black);

	signals:
		/*!
		 * The manager is ready to start a new game.
		 * This signal is emitted after a game has started
		 * or after a game has ended, if there are free
		 * game slots.
		 */
		void ready();
		/*!
		 * This signal is emitted when all games have ended and all
		 * idle players have been deleted. Then the manager can be
		 * safely deleted.
		 */
		void finished();
		/*! This signal redirects the ChessPlayer::debugMessage() signal. */
		void debugMessage(const QString& data);

	private slots:
		void onPlayerQuit();
		void onThreadQuit();

	private:
		struct GameEntry
		{
			ChessGame* game;
			const PlayerBuilder* white;
			const PlayerBuilder* black;
		};

		struct PlayerData
		{
			const PlayerBuilder* builder;
			ChessPlayer* player;
		};

		ChessPlayer* getPlayer(const PlayerBuilder* builder);
		bool startGame();
		void cleanup();

		bool m_finishing;
		int m_concurrency;
		int m_activeGameCount;
		QList<GameEntry> m_gameEntries;
		QList<PlayerData> m_idlePlayers;
};

#endif // GAMEMANAGER_H
