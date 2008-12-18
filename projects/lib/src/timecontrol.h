/*
    This file is part of SloppyGUI.

    SloppyGUI is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SloppyGUI is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SloppyGUI.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TIMECONTROL_H
#define TIMECONTROL_H

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
	public:
		/*! Creates a new time control with usable default settings. */
		TimeControl();

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

		/*! Sets the time per time control. */
		void setTimePerTc(int timePerTc);

		/*! Sets the number of moves per time control. */
		void setMovesPerTc(int movesPerTc);

		/*! Sets the time increment per move. */
		void setIncrement(int increment);

		/*! Sets the time per move. */
		void setTimePerMove(int timePerMove);
		
		/*! Sets the time left in the time control. */
		void setTimeLeft(int timeLeft);
		
		/*! Sets the number of full moves left in the time control. */
		void setMovesLeft(int movesLeft);
		
		/*!
		 * Updates the time control after a move was made.
		 * \param elapsedTime The time it took to make the last move.
		 */
		void update(int elapsedTime);

	private:
		int m_movesPerTc;
		int m_timePerTc;
		int m_timePerMove;
		int m_increment;
		int m_timeLeft;
		int m_movesLeft;

};

#endif // TIMECONTROL_H
