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

#ifndef TIMECONTROL_H
#define TIMECONTROL_H

#include <QElapsedTimer>
#include <QString>
#include <QCoreApplication>
class QSettings;

/*!
 * \brief Time controls of a chess game.
 *
 * TimeControl is used for telling the chess players how much time
 * they can spend thinking of their moves.
 *
 * \note All time handling is done in milliseconds.
 */
class LIB_EXPORT TimeControl
{
	Q_DECLARE_TR_FUNCTIONS(TimeControl)

	public:
		/*! Creates a new time control with invalid default settings. */
		TimeControl();

		/*!
		 * Creates a new time control from a string.
		 *
		 * \a str must either be "inf" for infinite time, or it can use
		 * the format: movesPerTc/timePerTc+timeIncrement
		 * - timePerTc is time in seconds if it's a single value.
		 *   It can also use the form minutes:seconds.
		 * - if movesPerTc is 0, it should be left out, and the slash
		 *   character isn't needed.
		 * - timeIncrement is the time increment per move in seconds.
		 *   If it's 0, it should be left out along with the plus sign.
		 *
		 * Example 1 (40 moves in 120 seconds):
		 *   TimeControl("40/120");
		 *
		 * Example 2 (same as example 1, 40 moves in 2 minutes):
		 *   TimeControl("40/2:0");
		 *
		 * Example 3 (whole game in 2.5 minutes plus 5 sec increment):
		 *   TimeControl("2:30+5");
		 *
		 * Example 4 (infinite thinking time):
		 *   TimeControl("inf");
		 */
		TimeControl(const QString& str);

		/*!
		 * Returns true if \a other is the same as this time control.
		 *
		 * The state of a game (eg. time left, used time, the expiry flag)
		 * and the expiry margin are ignored.
		 */
		bool operator==(const TimeControl& other) const;

		/*! Returns true if the time control is valid. */
		bool isValid() const;

		/*! Returns the time control string in PGN format. */
		QString toString() const;

		/*! Returns a verbose description of the time control. */
		QString toVerboseString() const;

		/*! Initializes the time control ready for a new game. */
		void initialize();

		/*! Returns true if the time control is infinite. */
		bool isInfinite() const;

		/*!
		 * Returns the time per time control,
		 * or 0 if there's no specified total time.
		 */
		int timePerTc() const;

		/*!
		 * Returns the number of moves per time control,
		 * or 0 if the whole game is played in timePerTc() time.
		 */
		int movesPerTc() const;

		/*! Returns the time increment per move. */
		int timeIncrement() const;

		/*!
		 * Returns the time per move.
		 *
		 * The player will think of each move this long at most.
		 * Returns 0 if there's no specified total time.
		 */
		int timePerMove() const;

		/*! Returns the time left in the time control. */
		int timeLeft() const;

		/*!
		 * Returns the number of full moves left in the time control,
		 * or 0 if the number of moves is not specified.
		 */
		int movesLeft() const;

		/*! Returns the maximum search depth in plies. */
		int plyLimit() const;

		/*! Returns the node limit for each move. */
		int nodeLimit() const;

		/*!
		 * Returns the expiry margin.
		 *
		 * Expiry margin is the amount of time a player can go over
		 * the time limit without losing on time.
		 * The default value is 0.
		 */
		int expiryMargin() const;


		/*!
		 * If \a enabled is true, infinite time control is enabled;
		 * otherwise it is disabled.
		 */
		void setInfinity(bool enabled = true);

		/*! Sets the time per time control. */
		void setTimePerTc(int timePerTc);

		/*! Sets the number of moves per time control. */
		void setMovesPerTc(int movesPerTc);

		/*! Sets the time increment per move. */
		void setTimeIncrement(int increment);

		/*! Sets the time per move. */
		void setTimePerMove(int timePerMove);
		
		/*! Sets the time left in the time control. */
		void setTimeLeft(int timeLeft);
		
		/*! Sets the number of full moves left in the time control. */
		void setMovesLeft(int movesLeft);

		/*! Sets the maximum search depth in plies. */
		void setPlyLimit(int plies);

		/*! Sets the node limit. */
		void setNodeLimit(int nodes);

		/*! Sets the expiry margin. */
		void setExpiryMargin(int expiryMargin);

		
		/*! Start the timer. */
		void startTimer();
		
		/*!
		 * Update the time control with the elapsed time.
		 * A move time increment will only be applied if
		 * \a applyIncrement is true. This is the default.
		 * Set this value to false if no increment is necessary for
		 * the current move, e.g. for a book move.
		 */
		void update(bool applyIncrement = true);

		/*! Returns the last elapsed move time. */
		int lastMoveTime() const;

		/*! Returns true if the allotted time has expired. */
		bool expired() const;

		/*!
		 * Returns the time left in an active clock.
		 *
		 * The TimeControl object doesn't know whether the clock is
		 * active or not. It's recommended to check the player's
		 * state first to verify that it's in the thinking state.
		 */
		int activeTimeLeft() const;

		/*! Reads time control settings from \a settings. */
		void readSettings(QSettings* settings);

		/*! Writes this time control to \a settings. */
		void writeSettings(QSettings* settings);

	private:
		int m_movesPerTc;
		int m_timePerTc;
		int m_timePerMove;
		int m_increment;
		int m_timeLeft;
		int m_movesLeft;
		int m_plyLimit;
		int m_nodeLimit;
		int m_lastMoveTime;
		int m_expiryMargin;
		bool m_expired;
		bool m_infinite;
		QElapsedTimer m_time;
};

#endif // TIMECONTROL_H
