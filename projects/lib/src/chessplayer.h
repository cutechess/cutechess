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

#ifndef CHESSPLAYER_H
#define CHESSPLAYER_H

#include <QObject>
#include "chessboard/chess.h"
#include "timecontrol.h"

class QString;

/*!
 * \brief A chess player, human or AI.
 *
 * \sa ChessEngine
 */
class LIB_EXPORT ChessPlayer : public QObject
{
	Q_OBJECT

	public:
		/*! Creates and initializes a new ChessPlayer object. */
		ChessPlayer(QObject* parent = 0);
		
		virtual ~ChessPlayer() { }

		/*!
		 * Starts a new chess game.
		 * \param side The side (color) the player should play as. It
		 * can be NoSide if the player is in force/observer mode.
		 * \param opponent The opposing player.
		 */
		virtual void newGame(Chess::Side side, ChessPlayer* opponent);
		
		/*! Tells the player to start thinking and make its move. */
		virtual void go();

		/*! Returns the player's time control. */
		TimeControl* timeControl();

		/*! Sets the time control for the player. */
		void setTimeControl(const TimeControl& timeControl);

		/*! Returns the side of the player. */
		Chess::Side side() const;

		/*! Sets the player to play on a specific side. */
		void setSide(Chess::Side side);

		/*!
		 * Sends the next move to the player.
		 * If the player is in force/observer mode, the move wasn't
		 * necessarily made by the opponent.
		 */
		virtual void makeMove(const Chess::Move& move) = 0;
		
		/*! Returns the player's name. */
		QString name() const;
		
		/*! Sets the player's name. */
		void setName(const QString& name);

		/*! Returns true if the player is human. */
		virtual bool isHuman() const = 0;

	signals:
		/*!
		 * Signals the time left in the player's clock when they
		 * start thinking of their next move.
		 * \param timeLeft Time left in milliseconds.
		 */
		void startedThinking(int timeLeft) const;

		/*! Signals the player's move. */
		void moveMade(const Chess::Move& move) const;
		
		/*!
		 * Signals that the player resigns the game.
		 * Invalid draw or victory claims will also be treated as
		 * resignations.
		 */
		void resign() const;

		/*! Signals a debugging message from the player. */
		void debugMessage(const QString& data) const;

	protected:
		/*! The player's name. */
		QString m_name;

		/*! Time control for the player's moves. */
		TimeControl m_timeControl;

		/*! The opposing player. */
		ChessPlayer* m_opponent;

	private:
		Chess::Side m_side;
};

#endif // CHESSPLAYER_H
