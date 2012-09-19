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


#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include <QObject>
#include <QPair>
#include <QList>
#include <QVector>
#include <QMap>
#include "board/move.h"
#include "timecontrol.h"
#include "pgngame.h"
class GameManager;
class PlayerBuilder;
class ChessGame;
class OpeningBook;
class OpeningSuite;

/*!
 * \brief Base class for chess tournaments
 */
class LIB_EXPORT Tournament : public QObject
{
	Q_OBJECT

	public:
		/*! \brief A structure for storing player details. */
		struct PlayerData
		{
			//! The player's builder object
			PlayerBuilder* builder;
			//! The player's time control
			TimeControl timeControl;
			//! The player's opening book
			const OpeningBook* book;
			//! The maximum book depth in plies
			int bookDepth;
			//! The number of games won by the player
			int wins;
			//! The number of games drawn by the player
			int draws;
			//! The number of games lost by the player
			int losses;
		};

		/*!
		 * Creates a new tournament that uses \a gameManager
		 * to manage the games.
		 */
		Tournament(GameManager* gameManager, QObject *parent);
		/*!
		 * Destroys the tournament.
		 *
		 * The destructor doesn't end any active games or cleanup
		 * their resources. The tournament can be safely destroyed
		 * only after the finished() signal is emitted.
		 */
		virtual ~Tournament();

		/*! Returns the tournament type (eg. "round-robin" or "gauntlet"). */
		virtual QString type() const = 0;
		/*! Returns the GameManager that manages the tournament's games. */
		GameManager* gameManager() const;
		/*! Returns true if the tournament is finished; otherwise returns false. */
		bool isFinished() const;
		/*! Returns a detailed description of the error. */
		QString errorString() const;
		/*!
		 * Returns the name of the tournament.
		 *
		 * The tournament name will be used as the "Event" tag's value
		 * in the resulting PGN data.
		 */
		QString name() const;
		/*! Returns the tournament site, ie. the "Site" tag's value. */
		QString site() const;
		/*! Returns the games' chess variant. */
		QString variant() const;
		/*! Returns the currently executing round of the tournament. */
		int currentRound() const;
		/*!
		 * Returns the number of games to play per encounter.
		 *
		 * If the number of games is even then each player is
		 * guaranteed to play an equal number of games with white
		 * and black pieces.
		 *
		 * The default value is 1.
		 */
		int gamesPerEncounter() const;
		/*!
		 * Returns the multiplier for the number of rounds to play.
		 *
		 * The default value is 1.
		 */
		int roundMultiplier() const;
		/*! Returns the number of games finished so far. */
		int finishedGameCount() const;
		/*! Returns the total number of games that will be played. */
		int finalGameCount() const;
		/*! Returns player data for the player at \a index. */
		PlayerData playerAt(int index) const;
		/*! Returns the number of participants in the tournament. */
		int playerCount() const;

		/*! Sets the tournament's name to \a name. */
		void setName(const QString& name);
		/*! Sets the tournament's site to \a site. */
		void setSite(const QString& site);
		/*! Sets the games' chess variant to \a variant. */
		void setVariant(const QString& variant);
		/*!
		 * Sets the game count per encounter to \a counter.
		 *
		 * \a counter must be at least 1.
		 */
		void setGamesPerEncounter(int count);
		/*!
		 * Sets the multiplier for the number of rounds to \a factor.
		 *
		 * \a factor must be at least 1.
		 */
		void setRoundMultiplier(int factor);
		/*! Sets the starting delay for each game to \a delay msec. */
		void setStartDelay(int delay);
		/*!
		 * Sets the recovery mode to \a recover.
		 *
		 * If \a recover is true then crashed players will
		 * will be restarted for the next game; otherwise the
		 * whole tournament stops when a player crashes.
		 */
		void setRecoveryMode(bool recover);
		/*!
		 * Sets the draw adjudication threshold for each game.
		 *
		 * A game will be adjudicated as a draw if both players report
		 * a score that's within \a score centipawns from zero after
		 * at least \a moveNumber full moves have been played.
		 */
		void setDrawThreshold(int moveNumber, int score);
		/*!
		 * Sets the resign adjudication threshold for each game.
		 *
		 * A game will be adjudicated as a loss for the player that
		 * made the last move if it reports a score that's at least
		 * \a score centipawns below zero for at least \a moveCount
		 * consecutive moves.
		 */
		void setResignThreshold(int moveCount, int score);
		/*!
		 * Uses \a suite as the opening suite (a collection of openings)
		 * for the games.
		 *
		 * The tournament takes ownership of \a suite.
		 */
		void setOpeningSuite(OpeningSuite* suite);
		/*!
		 * Sets the maximum depth of an opening from the opening suite
		 * to \a plies (halfmoves).
		 */
		void setOpeningDepth(int plies);
		/*!
		 * Sets the PGN output file for the games to \a fileName.
		 *
		 * The games are saved to the file in mode \a mode.
		 * If no PGN output file is set (default) then the games
		 * won't be saved.
		 */
		void setPgnOutput(const QString& fileName,
				  PgnGame::PgnMode mode = PgnGame::Verbose);

		/*!
		 * Sets PgnGame cleanup mode to \a enabled.
		 *
		 * If \a enabled is true (the default) then the generated PgnGame
		 * objects are destroyed automatically once the games are finished.
		 */
		void setPgnCleanupEnabled(bool enabled);

		/*!
		 * Sets the opening repetition mode to \a repeat.
		 *
		 * If \a repeat is true, each opening is repeated for two
		 * rounds; otherwise each game gets its own opening.
		 */
		void setOpeningRepetition(bool repeat);
		/*!
		 * Adds player \a builder to the tournament.
		 *
		 * The player's time control will be \a timeControl, which
		 * may differ from the other players' time controls.
		 *
		 * The Tournament object takes ownership of \a builder and will
		 * take care of deleting it.
		 */
		void addPlayer(PlayerBuilder* builder,
			       const TimeControl& timeControl,
			       const OpeningBook* book = 0,
			       int bookDepth = 256);

	public slots:
		/*! Starts the tournament. */
		void start();
		/*!
		 * Stops the tournament.
		 *
		 * Any running games will have an unterminated result.
		 * The finished() signal is emitted when the tournament
		 * is fully stopped.
		 */
		void stop();

	signals:
		/*!
		 * This signal is emitted when game \a game with ordering
		 * number \a number is started.
		 *
		 * \a whiteIndex is the index to the white player's data
		 * \a blackIndex is the index to the black player's data
		 * \note The game numbers start at 1.
		 */
		void gameStarted(ChessGame* game,
				 int number,
				 int whiteIndex,
				 int blackIndex);
		/*!
		 * This signal is emitted when game \a game with ordering
		 * number \a number is finished.
		 *
		 * The Tournament object deletes the game right after
		 * emitting this signal.
		 *
		 * \a whiteIndex is the index to the white player's data
		 * \a blackIndex is the index to the black player's data
		 * \note The game numbers start at 1.
		 * \note The games may not finish in the same order they
		 * are started.
		 */
		void gameFinished(ChessGame* game,
				  int number,
				  int whiteIndex,
				  int blackIndex);
		/*!
		 * This signal is emitted when all of the tournament's games
		 * have been played or after the tournament was stopped.
		 *
		 * The tournament can be safely destroyed after this signal
		 * is sent.
		 */
		void finished();

	protected:
		/*! Sets the currently executing tournament round to \a round. */
		void setCurrentRound(int round);
		/*!
		 * Initializes the pairings for the tournament.
		 *
		 * Subclasses that implement this member function should prepare
		 * their data so that the nextPair() function can be called
		 * immediately afterwards. The total number of games to play
		 * should also be set here via the \a setFinalGameCount()
		 * member function.
		 */
		virtual void initializePairing() = 0;
		/*!
		 * Returns the number of games in one tournament cycle.
		 *
		 * The gamesPerEncounter() and roundMultiplier() values
		 * must not affect the number of games per cycle.
		 */
		virtual int gamesPerCycle() const = 0;
		/*!
		 * Returns the pair of players for the next game.
		 *
		 * The pair's first element is the white player's index number
		 * and the second element is the black player's index number.
		 *
		 * Subclasses that implement this member function should call
		 * setCurrentRound() to increase the round when needed.
		 * Subclasses should also alternate the colors when needed,
		 * to make the tournament as fair as possible.
		 */
		virtual QPair<int, int> nextPair() = 0;

	private slots:
		void startNextGame();
		void onGameStarted(ChessGame* game);
		void onGameFinished(ChessGame* game);
		void onGameDestroyed(ChessGame* game);
		void onGameStartFailed(ChessGame* game);

	private:
		struct GameData
		{
			int number;
			int whiteIndex;
			int blackIndex;
		};

		GameManager* m_gameManager;
		ChessGame* m_lastGame;
		QString m_error;
		QString m_name;
		QString m_site;
		QString m_variant;
		int m_round;
		int m_nextGameNumber;
		int m_finishedGameCount;
		int m_savedGameCount;
		int m_finalGameCount;
		int m_gamesPerEncounter;
		int m_roundMultiplier;
		int m_startDelay;
		int m_drawMoveNumber;
		int m_drawScore;
		int m_resignMoveCount;
		int m_resignScore;
		int m_openingDepth;
		bool m_stopping;
		bool m_repeatOpening;
		bool m_recover;
		bool m_pgnCleanup;
		bool m_finished;
		OpeningSuite* m_openingSuite;
		QString m_pgnout;
		QString m_startFen;
		PgnGame::PgnMode m_pgnOutMode;
		QPair<int, int> m_pair;
		QList<PlayerData> m_players;
		QMap<int, PgnGame> m_pgnGames;
		QMap<ChessGame*, GameData*> m_gameData;
		QVector<Chess::Move> m_openingMoves;
};

#endif // TOURNAMENT_H
