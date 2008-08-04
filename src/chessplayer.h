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

#ifndef CHESSPLAYER_H
#define CHESSPLAYER_H

#include <QObject>
#include "chessboard/chessboard.h"
#include "timecontrol.h"

class QString;

/**
 * The ChessPlayer class represents any chess player, human or AI.
 */
class ChessPlayer : public QObject
{
	Q_OBJECT

	public:
		/**
		 * Creates a new ChessPlayer object.
		 * @param timeControl Time control for the player.
		 * @param parent The parent object.
		 */
		ChessPlayer(const TimeControl& timeControl, QObject* parent = 0);
		
		virtual ~ChessPlayer() { }

		/**
		 * Starts a new chess game.
		 * @param side The side (color) the player should play as.
		 */
		virtual void newGame(Chessboard::ChessSide side);
		
		/**
		 * Tells the player to start thinking and make its next move.
		 */
		virtual void go() = 0;

		/**
		 * Gets the player's time control.
		 * @return The time control.
		 */
		TimeControl timeControl() const;

		/**
		 * Sets the time control for the player.
		 * @param timeControl Time control for the player.
		 */
		void setTimeControl(const TimeControl& timeControl);

		/**
		 * Gets the side of the player.
		 * @return The side of the player.
		 * @see setSide()
		 */
		Chessboard::ChessSide side() const;

		/**
		 * Sets the player to play on a specific side (white or black)
		 * @param side The side of the player.
		 */
		void setSide(Chessboard::ChessSide side);

		/**
		 * Sets the opponent of the player.
		 * @param opponent The opponent.
		 */
		void setOpponent(ChessPlayer* opponent);

		/**
		 * Tells the opponent's move to the player.
		 * @param move A chess move which the opponent made.
		 */
		virtual void sendOpponentsMove(const ChessMove& move) = 0;
		
		/**
		 * Gets the name of the player.
		 * @return The name of the player.
		 */
		QString name() const;
		
		/**
		 * Gives the player a name.
		 * @param name The player's name.
		 */
		void setName(const QString& name);

		/**
		 * Tells whether or not the player is human.
		 * @return True if the player is human.
		 */
		virtual bool isHuman() const = 0;

	signals:
		/**
		 * Signals the engine's move.
		 * @param move A chess move which the engine made.
		 */
		void moveMade(const ChessMove& move) const;

		/**
		 * Signals a debugging message from the player.
		 * @param data The debugging message.
		 */
		void debugMessage(const QString& data) const;

	protected:
		/** The player's name. */
		QString m_name;

		/** Time control for the player's moves. */
		TimeControl m_timeControl;

		/** The opposing player. */
		ChessPlayer* m_opponent;

	private:
		Chessboard::ChessSide m_side;
};

#endif // CHESSPLAYER_H

