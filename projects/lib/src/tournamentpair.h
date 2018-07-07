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

#ifndef TOURNAMENTPAIR_H
#define TOURNAMENTPAIR_H

/*!
 * \brief A single encounter in a tournament
 *
 * The TournamentPair class can be used to manage a single tournament
 * encounter's progress, eg. scores for each player, number of games
 * started, games finished, games left, etc.
 *
 * TODO: players should be pointers to TournamentPlayer objects, not indexes
 */
class LIB_EXPORT TournamentPair
{
	public:
		/*!
		 * Creates a new TournamentPair object.
		 *
		 * \a firstPlayer is the first player's index and
		 * \a secondPlayer is the second player's index. An index value
		 * of -1 represents a BYE player.
		 */
		TournamentPair(int firstPlayer = -1,
			       int secondPlayer = -1);

		/*! Returns the first player's index. */
		int firstPlayer() const;
		/*! Returns the second player's index. */
		int secondPlayer() const;
		/*!
		 * Returns true if the players are in their original order,
		 * ie. not swapped; otherwise returns false.
		 */
		bool hasOriginalOrder() const;
		/*!
		 * Returns the index of the player in the lead.
		 * Returns -1 if both players have the same score.
		 *
		 * If one of the players is a BYE, the other player's index
		 * is returned.
		 */
		int leader() const;
		/*!
		 * Returns true if this pair has the same players as \a other;
		 * otherwise returns false.
		 *
		 * \note The players may be in different order in \a other.
		 */
		bool hasSamePlayers(const TournamentPair* other) const;
		/*!
		 * Returns true if both players are real (ie. not BYEs);
		 * otherwise returns false.
		 */
		bool isValid() const;
		/*!
		 * Returns the number of started games between the pair,
		 * including finished games.
		 */
		int gamesStarted() const;
		/*! Adds a new started game to the current encounter. */
		void addStartedGame();
		/*! Returns the number of finished games between the pair. */
		int gamesFinished() const;
		/*! Returns the number of ongoing between the pair. */
		int gamesInProgress() const;
		/*!
		 * Returns the sum of both player's score in their current
		 * encounter.
		 */
		int scoreSum() const;
		/*! Returns \a firstScore() - \a secondScore(). */
		int scoreDiff() const;
		/*!
		 * Returns the first player's score in the current encounter.
		 */
		int firstScore() const;
		/*!
		 * Adds \a score to the first player's score in the current
		 * encounter.
		 */
		void addFirstScore(int score);
		/*!
		 * Returns the second player's score in the current encounter.
		 */
		int secondScore() const;
		/*!
		 * Adds \a score to the second player's score in the current
		 * encounter.
		 */
		void addSecondScore(int score);
		/*!
		 * Swaps the players so that the first player becomes the
		 * second player and vice versa.
		 */
		void swapPlayers();

	private:
		struct Player
		{
			int index;
			int score;
		};

		Player m_first;
		Player m_second;
		int m_gamesStarted;
		bool m_hasOriginalOrder;
};

#endif // TOURNAMENTPAIR_H
