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
 * It is used for telling the chess engines how much time they can spend
 * thinking of their moves.
 */
class TimeControl
{
public:
	TimeControl(int timePerTc = 0, int movesPerTc = 0, int increment = 0, int timePerMove = 0);

	/**
	 * Time in milliseconds per time control.
	 */
	int timePerTc() const;

	/**
	 * Number of moves per time control.
	 * If it's 0 the whole game is played in 'timePerTc' time.
	 */
	int movesPerTc() const;

	/**
	 * Time increment per move in milliseconds.
	 */
	int increment() const;

	/**
	 * Time in milliseconds per move.
	 */
	int timePerMove() const;

	void setTimePerTc(int movesPerTc);
	void setMovesPerTc(int timePerTc);
	void setIncrement(int increment);
	void setTimePerMove(int timePerMove);

private:
	int m_movesPerTc;
	int m_timePerTc;
	int m_timePerMove;
	int m_increment;
};

#endif // TIMECONTROL

