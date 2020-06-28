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

#ifndef TOURNAMENTPLAYER_H
#define TOURNAMENTPLAYER_H

#include "playerbuilder.h"
#include "timecontrol.h"
#include "board/side.h"

class OpeningBook;

/*! \brief A class for storing a player's tournament-specific details. */
class LIB_EXPORT TournamentPlayer
{
	public:
		/*! Creates a new TournamentPlayer object. */
		TournamentPlayer(PlayerBuilder* builder,
				 const TimeControl& timeControl,
				 const OpeningBook* book,
				 int bookDepth);

		/*! Returns the player's builder object. */
		const PlayerBuilder* builder() const;
		/*! Returns the player's name. */
		QString name() const;
		/*! Sets the player's name to \a name. */
		void setName(const QString& name);
		/*! Returns the player's time control. */
		const TimeControl& timeControl() const;
		/*! Returns the player's opening book. */
		const OpeningBook* book() const;
		/*! Returns the player's maximum book depth. */
		int bookDepth() const;
		/*!
		 * Returns the total number of wins the player has in the
		 * tournament.
		 */
		int wins() const;
		/*!
		 * Returns the total number of draws the player has in the
		 * tournament.
		 */
		int draws() const;
		/*!
		 * Returns the total number of losses the player has in the
		 * tournament.
		 */
		int losses() const;
		/*!
		 * Returns the total number of wins of the player when
		 * playing with White.
		 */
		int whiteWins() const;
		/*!
		 * Returns the total number of draws of the player when
		 * playing with White.
		 */
		int whiteDraws() const;
		/*!
		 * Returns the total number of losses of the player when
		 * playing with White.
		 */
		int whiteLosses() const;
		/*!
		 * Returns the total number of wins of the player when
		 * playing with Black.
		 */
		int blackWins() const;
		/*!
		 * Returns the total number of draws of the player when
		 * playing with Black.
		 */
		int blackDraws() const;
		/*!
		 * Returns the total number of losses of the player when
		 * playing with Black.
		 */
		int blackLosses() const;
		/*! Returns the player's total score in the tournament. */
		int score() const;
		/*! Adds \a score to the player's score in the tournament. */
		void addScore(Chess::Side side, int score);
		/*!
		 * Returns the total number of games the player has finished
		 * in the tournament.
		 */
		int gamesFinished() const;

	private:
		PlayerBuilder* m_builder;
		TimeControl m_timeControl;
		const OpeningBook* m_book;
		int m_bookDepth;
		int m_wins;
		int m_draws;
		int m_losses;
		int m_whiteWins;
		int m_whiteDraws;
		int m_whiteLosses;
};

#endif // TOURNAMENTPLAYER_H
