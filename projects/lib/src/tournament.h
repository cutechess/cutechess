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
#include <QList>
#include <QVector>
#include <QMap>
#include <QFile>
#include <QTextStream>
#include "board/move.h"
#include "timecontrol.h"
#include "pgngame.h"
#include "gameadjudicator.h"
#include "tournamentplayer.h"
#include "tournamentpair.h"
class GameManager;
class PlayerBuilder;
class ChessGame;
class OpeningBook;
class OpeningSuite;
class Sprt;

/*!
 * \brief Base class for chess tournaments
 */
class LIB_EXPORT Tournament : public QObject
{
	Q_OBJECT

	public:
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
		const TournamentPlayer& playerAt(int index) const;
		/*! Returns the number of participants in the tournament. */
		int playerCount() const;
		/*!
		 * Returns the maximum number of players that can be seeded
		 * in the tournament.
		 */
		int seedCount() const;
		/*!
		 * Returns the SPRT object of this tournament.
		 *
		 * Initializing \a sprt makes this tournament to use it as a
		 * stopping criterion.
		 */
		Sprt* sprt() const;

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
		 * Returns true (default) if the tournament supports
		 * user-defined round multiplier; otherwise returns false.
		 */
		virtual bool canSetRoundMultiplier() const;
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
		 * Sets the game adjudicator to \a adjudicator.
		 *
		 * The default adjudicator does nothing.
		 */
		void setAdjudicator(const GameAdjudicator& adjudicator);
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
		 * Sets the PgnGame mode to write unfinished games to \a enabled.
		 *
		 * If \a enabled is true (the default) then the generated PGN games
		 * are saved even if they have no result.
		 */
		void setPgnWriteUnfinishedGames(bool enabled);

		/*!
		 * Sets PgnGame cleanup mode to \a enabled.
		 *
		 * If \a enabled is true (the default) then the generated PgnGame
		 * objects are destroyed automatically once the games are finished.
		 */
		void setPgnCleanupEnabled(bool enabled);

		/*!
		 * Sets the EPD output file for the end positions to \a fileName.
		 *
		 * If no EPD output file is set (default) then the positions
		 * will not be saved.
		 */
		void setEpdOutput(const QString& fileName);

		/*!
		 * Sets the number of opening repetitions to \a count.
		 *
		 * Each opening is played \a count times before
		 * going to the next opening. The default is 1.
		 */
		void setOpeningRepetitions(int count);
		/*!
		 * Sets the side swap flag to \a enabled.
		 *
		 * If \a enabled is true then paired engines will
		 * swap sides for the following game.
		 */
		void setSwapSides(bool enabled);
		/*!
		 * Sets opening book ownerhip to \a enabled.
		 *
		 * By default the \a Tournament object doesn't take ownership of
		 * its opening books.
		 */
		void setOpeningBookOwnership(bool enabled);
		/*!
		 * Sets the maximum number of players that should be seeded in
		 * the tournament.
		 */
		void setSeedCount(int seedCount);
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
			       const OpeningBook* book = nullptr,
			       int bookDepth = 256);
		/*!
		 * Returns tournament results as a string.
		 * The default implementation works for most tournament types.
		 */
		virtual QString results() const;

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
		/*! Returns the number of games in progress. */
		int gamesInProgress() const;
		/*! Returns the pair that started the last game. */
		TournamentPair* currentPair() const;
		/*!
		 * Returns a tournament pair of \a player1 and \a player2.
		 *
		 * If a pair with the given players doesn't already exist,
		 * one is created.
		 */
		TournamentPair* pair(int player1, int player2);
		/*!
		 * This member function is called by \a startNextGame() to
		 * start a new tournament game between \a pair.
		 *
		 * Reimplementations should call the base implementation.
		 */
		void startGame(TournamentPair* pair);
		/*!
		 * This member function is called by \a startGame() right
		 * before the game is actually started.
		 *
		 * The default implementation does nothing.
		 */
		virtual void onGameAboutToStart(ChessGame* game,
						const PlayerBuilder* white,
						const PlayerBuilder* black);
		/*!
		 * Returns the index of player \a side in \a game.
		 *
		 * \note \a game must belong to this tournament.
		 */
		int playerIndex(ChessGame* game, Chess::Side side) const;
		/*!
		 * Initializes the pairings for the tournament.
		 *
		 * Subclasses that implement this member function should prepare
		 * their data so that the nextPair() function can be called
		 * immediately afterwards.
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
		 *
		 * Sublasses can return \a (-1, -1) if the next game
		 * should not be started yet.
		 */
		virtual TournamentPair* nextPair(int gameNumber) = 0;
		/*!
		 * Emits the \a finished() signal.
		 *
		 * Subclasses can reimplement this method to add their own
		 * cleanup and finalization routines when the tournament ends.
		 */
		virtual void onFinished();
		/*!
		 * Adds \a score points to player at index \a player.
		 *
		 * This member function is called when a game ends with a
		 * 1-0, 0-1 or 1/2-1/2 result. Subclasses can reimplement this
		 * to do their own score tracking.
		 */
		virtual void addScore(int player, int score);
		/*!
		 * Returns true if all games in the tournament have finished;
		 * otherwise returns false.
		 */
		virtual bool areAllGamesFinished() const;
		/*!
		 * Returns true if Gauntlet ordering is used for the ratings
		 * table (ie. first engine always at the top and the rest
		 * ordered by Elo); otherwise returns false.
		 *
		 * The default implementation always returns false.
		 */
		virtual bool hasGauntletRatingsOrder() const;

	private slots:
		void startNextGame();
		bool writePgn(PgnGame* pgn, int gameNumber);
		bool writeEpd(ChessGame* game);
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
		struct RankingData
		{
			QString name;
			int games;
			qreal score;
			qreal draws;
			qreal errorMargin;
			qreal eloDiff;
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
		int m_openingDepth;
		int m_seedCount;
		bool m_stopping;
		int m_openingRepetitions;
		bool m_recover;
		bool m_pgnCleanup;
		bool m_pgnWriteUnfinishedGames;
		bool m_finished;
		bool m_bookOwnership;
		GameAdjudicator m_adjudicator;
		OpeningSuite* m_openingSuite;
		Sprt* m_sprt;
		QFile m_pgnFile;
		QTextStream m_pgnOut;
		QFile m_epdFile;
		QTextStream m_epdOut;
		QString m_startFen;
		int m_repetitionCounter;
		int m_swapSides;
		PgnGame::PgnMode m_pgnOutMode;
		TournamentPair* m_pair;
		QMap< QPair<int, int>, TournamentPair* > m_pairs;
		QList<TournamentPlayer> m_players;
		QMap<int, PgnGame> m_pgnGames;
		QMap<ChessGame*, GameData*> m_gameData;
		QVector<Chess::Move> m_openingMoves;
};

#endif // TOURNAMENT_H
