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

#include <QObject>


/**
 * The TimeControl class represents the time controls of a chess game.
 * It is used for telling the chess engines how much time they can spend
 * thinking of their moves.
 */
class TimeControl : public QObject
{
	Q_OBJECT

	public:
		/**
		 * Creates a new TimeControl object.
		 * @param timePerTc Time in milliseconds per time control.
		 * @param movesPerTc Number of full moves per time control.
		 * @param increment Time increment in milliseconds per move.
		 * @param timePerMove Time in milliseconds per move.
		 * @param parent The parent object.
		 */
		TimeControl(int timePerTc = 0, int movesPerTc = 0, int increment = 0,
			int timePerMove = 0, QObject* parent = 0);

		/**
		 * Time in milliseconds per time control.
		 * Returns 0 if there's no specified time per time control.
		 * @see timePerMove()
		 */
		int timePerTc() const;

		/**
		 * Number of moves per time control.
		 * Returns 0 if the whole game is played in timePerTc() time.
		 */
		int movesPerTc() const;

		/**
		 * Time increment in milliseconds per move.
		 */
		int timeIncrement() const;

		/**
		 * Time in milliseconds per move.
		 * The player will think of each move this long at most.
		 * Returns 0 if there's no specified time per move.
		 * @see timePerTc()
		 */
		int timePerMove() const;

		/**
		 * Time in milliseconds left in the time control.
		 */
		int timeLeft() const;

		/**
		 * Number of full moves left in the time control.
		 * Returns 0 if there's no specific number of moves per time control.
		 */
		int movesLeft() const;


		/**
		 * Sets the time in milliseconds per time control.
		 * @see timePerTc()
		 */
		void setTimePerTc(int movesPerTc);

		/**
		 * Sets the number of moves per time control.
		 * @see movesPerTc()
		 */
		void setMovesPerTc(int movesPerTc);

		/**
		 * Sets the time increment in milliseconds per move.
		 * @see timeIncrement()
		 */
		void setIncrement(int increment);

		/**
		 * Sets the time in milliseconds per move.
		 * @see timePerMove()
		 */
		void setTimePerMove(int timePerMove);
		
		/**
		 * Sets the time in milliseconds left in the time control.
		 * @see timeLeft()
		 */
		void setTimeLeft(int timeLeft);
		
		/**
		 * Sets the number of full moves left in the time control.
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

