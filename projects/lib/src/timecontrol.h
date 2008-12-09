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

/**
 * The TimeControl class represents the time controls of a chess game.
 *
 * It is used for telling the chess engines how much time they can spend
 * thinking of their moves.
 *
 * All time handling is done in milliseconds.
 */
class LIB_EXPORT TimeControl
{
	public:
		/**
		 * Creates a new TimeControl object.
		 */
		TimeControl();

		/**
		 * Returns the time per time control.
		 * @return Time per time control or 0 if there's no time specified.
		 * @see timePerMove()
		 */
		int timePerTc() const;

		/**
		 * Returns the number of moves per time control.
		 * @return Number of moves per time control. Returns 0 if the whole
		 * game is played in timePerTc() time.
		 */
		int movesPerTc() const;

		/**
		 * Returns the time increment per move.
		 * @return Time increment per move.
		 */
		int timeIncrement() const;

		/**
		 * Returns the time per move.
		 * The player will think of each move this long at most.
		 * @return The time per move or 0 if there's no time specified.
		 * @see timePerTc()
		 */
		int timePerMove() const;

		/**
		 * Returns the time left in the time control.
		 * @return The time left in the time control.
		 */
		int timeLeft() const;

		/**
		 * Returns the number of full moves left in the time control.
		 * @return The number of full moves left in the time control or 0
		 * if the number of moves is not specified.
		 */
		int movesLeft() const;

		/**
		 * Sets the time per time control.
		 * @param timePerTc The time per time control.
		 * @see timePerTc()
		 */
		void setTimePerTc(int timePerTc);

		/**
		 * Sets the number of moves per time control.
		 * @param movesPerTc The number of moves per time control.
		 * @see movesPerTc()
		 */
		void setMovesPerTc(int movesPerTc);

		/**
		 * Sets the time increment per move.
		 * @param increment The time increment per move.
		 * @see timeIncrement()
		 */
		void setIncrement(int increment);

		/**
		 * Sets the time per move.
		 * @param timePerMove The time per move.
		 * @see timePerMove()
		 */
		void setTimePerMove(int timePerMove);
		
		/**
		 * Sets the time left in the time control.
		 * @param timeLeft The time left in the time control.
		 * @see timeLeft()
		 */
		void setTimeLeft(int timeLeft);
		
		/**
		 * Sets the number of full moves left in the time control.
		 * @param movesLeft The number of full moves left in time control.
		 * @see movesLeft()
		 */
		void setMovesLeft(int movesLeft);
		
		/**
		 * Update the time control after a move was made.
		 * @param elapsedTime The time in milliseconds it took to make the last move.
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

