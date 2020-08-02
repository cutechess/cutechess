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
		/*! The policy for using a fresh opening. */
		enum OpeningPolicy
		{
			DefaultPolicy,     //!< Shift on repetition count and on new encounter
			EncounterPolicy,   //!< Shift on new encounter
			RoundPolicy        //!< Shift on new round
		};

		/*! Additional result types used for statistics. */
		enum AuxResultType
		{
			//! Regular loss (resignation, adjudication, opponent win)
			RegularLoss = Chess::Result::ResultError + 1,
			//! Regular win (mate, opponent resignation, adjudication)
			RegularWin,
			//! Win caused by timeout, illegal move, disconnect, or
			//! stalled connection of opponent.
			OtherWin,
			//! Draw Types
			Stalemate,
			InsufficientMaterial,
			MoveRepetiton,
			MovesRule,
			CountingRules,
			OtherDraw,
			//! Loss by invalid result claim
			InvalidResultClaim
		};
		constexpr static auto c_defaultFormat 
			= "Rank,Name,Elo,Error,Games,Score,DScore";

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
		 * The value of \a policy rules when to switch to the next opening.
		 *
		 * Given the default value \ref DefaultPolicy a new opening is selected
		 * when the number of opening repetitions played reaches the specified
		 * number of repetitions (\ref setOpeningRepetitions) or a new encounter
		 * is started.
		 *
		 * If \ref EncounterPolicy is used then a new opening will be selected
		 * only for any new encounter. \ref RoundPolicy shifts to a new opening
		 * only when a new round starts.
		 */
		void setOpeningPolicy(OpeningPolicy policy = DefaultPolicy);
		/*!
		 * Sets the reverse colors flag to \a enabled.
		 *
		 * If \a enabled is true then the players will play reverse
		 * sides /wrt to the normal schedule. The default is false.
		 */
		void setReverseSides(bool enabled);
		/*!
		 * Sets opening book ownership to \a enabled.
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
		 * Sets the side swap flag to \a enabled.
		 *
		 * If \a enabled is true then paired engines will
		 * swap sides for the following game.
		 */
		void setSwapSides(bool enabled);
		/*!
		 * The \a format specifies which information are listed in the
		 * table of tournament results. Available tokens are given by
		 * \p fieldMap.
		 */
		void setResultFormat(const QString &format);
		/*!
		 * Returns the format of the result table.
		 * The format uses tokens given by \p fieldMap.
		 */
		QString resultFormat() const;
		/*!
		 * Lists the result formats associated with a name.
		 */
		const QMap<QString, QString>& namedResultFormats() const;
		/*!
		 * Lists the format tokens for results.
		 */
		const QList<QString> resultFieldTokens() const;
		/*!
		 * Result field groups: Each key is a shortcut for a
		 * corresponding list of result format tokens.
		 */
		const QMap<QString, QString>& resultFieldGroups() const;
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
		/*!
		 * Returns outcome statistics per player as a string.
		 */
		virtual QString outcomes() const;
		/*!
		 * Returns true if the tournament is stopping, else false.
		 */
		bool isStopping() const;

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
		 * This signal is emitted when a game has ended irregularly.
		 * \a index is the index to the losing player's data.
		 * The type of termination is given by \a result.
		 * \note This signal is not used in case of no or unclear outcome.
		 */
		void gameTerminated(int index,
				    Chess::Result result);
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
		 * Adds \a score points to player at index \a player for
		 * playing side \a side.
		 *
		 * This member function is called when a game ends with a
		 * 1-0, 0-1 or 1/2-1/2 result. Subclasses can reimplement this
		 * to do their own score tracking.
		 */
		virtual void addScore(int player, Chess::Side side, int score);
		/*!
		 * Add \a result for a game of player at index \a iWhite vs
		 * player at index \a iBlack to the outcome statistics.
		 */
		virtual void addOutcome(int iWhite,
					int iBlack,
					Chess::Result result);
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
		/*! Output field identifier */
		enum OutputField
		{
			Rank,
			Name,
			Games,
			Wins,
			Losses,
			Draws,
			WhiteWins,
			WhiteLosses,
			WhiteDraws,
			BlackWins,
			BlackLosses,
			BlackDraws,
			Points,
			Score,
			DrawScore,
			EloDiff,
			ErrorMargin,
			LOS,
			WhiteScore,
			WhiteDrawScore,
			WhiteEloDiff,
			WhiteErrorMargin,
			WhiteLOS,
			BlackScore,
			BlackDrawScore,
			BlackEloDiff,
			BlackErrorMargin,
			BlackLOS,
			TimeForfeits,
			IllegalMoves,
			Disconnections,
			StalledConnections,
			InvalidResultClaims,
			RegularLosses,
			RegularWins,
			OtherWins,
			DrawsByStalemate,
			DrawsByMaterial,
			DrawsByRepetiton,
			DrawsByMovesRule,
			DrawsByCountingRules,
			DrawsByAdjudication,
			DrawsByAgreement,
			OtherDraws,
			TimeControlString
		};

		const QMap<QString, int> m_tokenMap = {
			{"Rank",    Rank},
			{"Name",    Name},
			{"Games",   Games},
			{"W",       Wins},
			{"L",       Losses},
			{"D",       Draws},
			{"wW",      WhiteWins},
			{"wL",      WhiteLosses},
			{"wD",      WhiteDraws},
			{"bW",      BlackWins},
			{"bL",      BlackLosses},
			{"bD",      BlackDraws},
			{"Points",  Points},
			{"Score",   Score},
			{"DScore",  DrawScore},
			{"Elo",     EloDiff},
			{"Error",   ErrorMargin},
			{"LOS",     LOS},
			{"wScore",  WhiteScore},
			{"wDScore", WhiteDrawScore},
			{"wElo",    WhiteEloDiff},
			{"wError",  WhiteErrorMargin},
			{"wLOS",    WhiteLOS},
			{"bScore",  BlackScore},
			{"bDScore", BlackDrawScore},
			{"bElo",    BlackEloDiff},
			{"bError",  BlackErrorMargin},
			{"Time",    TimeForfeits},
			{"Illegal", IllegalMoves},
			{"Discon",  Disconnections},
			{"Stall",   StalledConnections},
			{"WrClaim", InvalidResultClaims},
			{"RegulL",  RegularLosses},
			{"RegulW",  RegularWins},
			{"OtherW",  OtherWins},
			{"DStale",  DrawsByStalemate},
			{"DMater",  DrawsByMaterial},
			{"DRepet",  DrawsByRepetiton},
			{"DMoves",  DrawsByMovesRule},
			{"DCount",  DrawsByCountingRules},
			{"DAdj",    DrawsByAdjudication},
			{"DAgree",  DrawsByAgreement},
			{"OtherD",  OtherDraws},
			{"TC",      TimeControlString}
		};

		const QMap<QString, QString> m_namedGroups =
		{
			{"WLDStats",	"W,L,D,Points,Score,wW,wL,wD,wPoints,wScore,"
					"bW,bL,bD,bPoints,bScore"},
			{"DrawStats",	"DScore,DStale,DMater,DRepet,DMoves,DAdj,DAgree,OtherD"},
			{"Terminations","Time,Illegal,Discon,Stall,WrClaim"}
		};

		const QMap<QString, QString> m_namedFormats =
		{
			{"minimal",	"Rank,Name"},
			{"small",	"Rank,Name,Games,Points"},
			{"short",	"Rank,Name,Elo,Games"},
			{"concise",	"Rank,Name,Elo,Error,Games"},
			{"default",	QString(c_defaultFormat)},
			{"with-points",	"Rank,Name,Elo,Error,Games,Points,Score,DScore"},
			{"wide",	"Rank,Name,Elo,Error,Games,W,L,D,Points,Score,DScore"},
			{"wide2",	"Rank,Name,Elo,Error,Games,W,L,D,Points,"
					"Score,DScore,wScore,bScore"},
			{"per-color",	"Rank,Name,Elo,Error,Games,W,L,D,Points,"
					"wW,wL,wD,wPoints,bW,bL,bD,bPoints"},
			{"ordo",	"Rank,Name,Elo,Points,Games,Score"},
			{"term",	"Rank,Name,Elo,Error,Games,Score,"
					"Time,Illegal,Discon,Stall,WrClaim"},
			{"draws",	"Rank,Name,Elo,Error,Games,W,L,D,Points,Score,DScore,"
					"DStale,DMater,DRepet,DMoves,DAdj,DAgree,OtherD"},
			{"wide3",	"Rank,Name,TC,Elo,Error,Games,WLDStats,"
					"DrawStats,Terminations"}
		};

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
			int wins;
			int losses;
			int draws;
			int whiteWins;
			int whiteLosses;
			int whiteDraws;
			int blackWins;
			int blackLosses;
			int blackDraws;
			qreal points;
			qreal score;
			qreal drawScore;
			qreal eloDiff;
			qreal errorMargin;
			qreal LOS;
			qreal whiteScore;
			qreal whiteDrawScore;
			qreal whiteEloDiff;
			qreal whiteErrorMargin;
			qreal whiteLOS;
			qreal blackScore;
			qreal blackDrawScore;
			qreal blackEloDiff;
			qreal blackErrorMargin;
			qreal blackLOS;
			int timeForfeits;
			int illegalMoves;
			int disconnections;
			int stalledConnections;
			int invalidResultClaims;
			int regularLosses;
			int regularWins;
			int otherWins;
			int drawsByStalemate;
			int drawsByMaterial;
			int drawsByRepetiton;
			int drawsByMovesRule;
			int drawsByCountingRules;
			int drawsByAdjudication;
			int drawsByAgreement;
			int otherDraws;
			QString timeControl;
		};

		QString resultsForSides(int index) const;

		GameManager* m_gameManager;
		ChessGame* m_lastGame;
		QString m_error;
		QString m_name;
		QString m_site;
		QString m_variant;
		int m_round;
		int m_oldRound;
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
		OpeningPolicy m_openingPolicy;
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
		bool m_reverseSides;

		QString m_resultFormat;
		PgnGame::PgnMode m_pgnOutMode;
		TournamentPair* m_pair;
		QMap< QPair<int, int>, TournamentPair* > m_pairs;
		QList<TournamentPlayer> m_players;
		QMap<int, PgnGame> m_pgnGames;
		QMap<ChessGame*, GameData*> m_gameData;
		QVector<Chess::Move> m_openingMoves;
		QMap<int, QString> m_headerMap;
};

/*!
 * \brief Formatter for chess tournament results
 */
class LIB_EXPORT ResultFormatter : public QObject
{
	Q_OBJECT

	public:
		/*!
		 * The ResultFormatter uses a reference to a map of available
		 * tokens \a tokenMap to generate a tournament result list.
		 */
		ResultFormatter(const QMap<QString, int>& tokenMap,
				const QString& format = QString(),
				QObject* parent = nullptr);
		/*!
		 * Sets the output \a format of an entry in a tournament
		 * result list.
		 */
		void setEntryFormat(const QString& format);
		/*! Returns format for a line of tournament results */
		QString entryFormat() const;
		/*! Returns a line of tournament results */
		QString entry(const QMap<int, QString>& data) const;
	private:
		QString m_entryFormat;
		QMap<QString, int> m_tokenMap;
		QStringList m_tokenList;
};

#endif // TOURNAMENT_H
