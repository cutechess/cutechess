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
#include <QString>
#include <QTimer>
#include <QVector>
#include "chessboard/chess.h"
#include "chessboard/result.h"
#include "chessboard/variant.h"
#include "timecontrol.h"
#include "moveevaluation.h"


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
		
		/*! Returns true if the player is connected. */
		bool isConnected() const;

		/*! Returns true if the player is ready to play. */
		bool isReady() const;

		/*!
		 * Starts a new chess game.
		 * \param side The side (color) the player should play as. It
		 * can be NoSide if the player is in force/observer mode.
		 * \param opponent The opposing player.
		 */
		virtual void newGame(Chess::Side side, ChessPlayer* opponent);
		
		/*! Tells the player that the game ended by \a result. */
		virtual void endGame(Chess::Result result);
		
		/*! Tells the player to start thinking and make its move. */
		virtual void go() = 0;

		/*! Returns the player's evaluation of the current position. */
		const MoveEvaluation& evaluation() const;

		/*! Returns the player's time control. */
		const TimeControl* timeControl() const;

		/*! Sets the time control for the player. */
		void setTimeControl(const TimeControl& timeControl);

		/*! Returns the side of the player. */
		Chess::Side side() const;

		/*! Sets the player to play on a specific side. */
		void setSide(Chess::Side side);

		/*!
		 * Returns true if the player is in observer mode.
		 * In this mode, the player doesn't start thinking and
		 * doesn't make any moves unless excplicitly told so.
		 *
		 * \sa go()
		 */
		virtual bool inObserverMode() const;

		/*!
		 * Sets the observer mode to \a enabled.
		 * \note Some player types are always in observer mode, in
		 * which case this method does nothing.
		 */
		virtual void setObserverMode(bool enabled);

		/*!
		 * Sends the next move to the player.
		 * If the player is in force/observer mode, the move wasn't
		 * necessarily made by the opponent.
		 */
		virtual void makeMove(const Chess::Move& move) = 0;
		
		/*! Forces the player to play \a move as its next move. */
		void makeBookMove(const Chess::Move& move);
		
		/*! Returns the player's name. */
		QString name() const;
		
		/*! Sets the player's name. */
		void setName(const QString& name);

		/*! Sets the board object. */
		void setBoard(Chess::Board* board);
		
		/*! Returns true if the player can play \a variant. */
		virtual bool supportsVariant(Chess::Variant variant) const = 0;

		/*! Returns true if the player is human. */
		virtual bool isHuman() const = 0;

	signals:
		/*!
		 * Emitted when the player gets the turn.
		 * \a isHumanTurn is true if the player is human.
		 */
		void humanTurn(bool isHumanTurn);

		/*! Signals the player's evaluation of the position. */
		void sendEvaluation(const MoveEvaluation& eval) const;

		/*! Signals that the player is ready to play. */
		void ready() const;
		
		/*!
		 * Signals the time left in the player's clock when they
		 * start thinking of their next move.
		 * \param timeLeft Time left in milliseconds.
		 */
		void startedThinking(int timeLeft) const;

		/*! Signals the player's move. */
		void moveMade(const Chess::Move& move) const;
		
		/*! Signals that the player forfeits the game. */
		void forfeit(Chess::Result result) const;

		/*! Signals a debugging message from the player. */
		void debugMessage(const QString& data) const;

	protected slots:
		/*! Called when the player disconnects. */
		virtual void onDisconnect();

		/*! Called when the player's flag falls. */
		virtual void onTimeout();

	protected:
		/*! Closes the player's connection to the operator. */
		virtual void closeConnection();

		/*! Emits the forfeit() signal. */
		void emitForfeit(Chess::Result::Code code, const QString& arg = "");

		/*!
		 * Emits the player's move, and a timeout signal if the
		 * move came too late.
		 */
		void emitMove(const Chess::Move& move);
		
		/*! Starts the player's move timer (chess clock). */
		void startClock();

		/*! Returns the opponent's side. */
		Chess::Side otherSide() const;
		
		/*! The current evaluation. */
		MoveEvaluation m_eval;

		/*! Is the player ready to play? */
		bool m_isReady;
		
		/*! The board object. */
		Chess::Board* m_chessboard;

		/*! The opposing player. */
		ChessPlayer* m_opponent;
		
		/*! Timer for detecting when the player's time is up. */
		QTimer m_timer;
		
		/*! Is a game in progress? If not, all moves are rejected. */
		bool m_gameInProgress;

	private:
		QString m_name;
		TimeControl m_timeControl;
		bool m_connected;
		bool m_forfeited;
		Chess::Side m_side;
};

#endif // CHESSPLAYER_H
