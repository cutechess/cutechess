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

#ifndef CHESSPLAYER_H
#define CHESSPLAYER_H

#include <QObject>
#include <QString>
#include <QVector>
#include "board/result.h"
#include "board/move.h"
#include "timecontrol.h"
#include "moveevaluation.h"
class QTimer;
namespace Chess { class Board; }


/*!
 * \brief A chess player, human or AI.
 *
 * \sa ChessEngine
 */
class LIB_EXPORT ChessPlayer : public QObject
{
	Q_OBJECT

	public:
		/*! The different states of ChessPlayer. */
		enum State
		{
			NotStarted,	//!< Not started or uninitialized
			Starting,	//!< Starting or initializing
			Idle,		//!< Idle and ready to start a game
			Observing,	//!< Observing a game, or waiting for turn
			Thinking,	//!< Thinking of the next move
			FinishingGame,	//!< Finishing or cleaning up after a game
			Disconnected	//!< Disconnected or terminated
		};

		/*! Creates and initializes a new ChessPlayer object. */
		ChessPlayer(QObject* parent = nullptr);
		virtual ~ChessPlayer();
		
		/*!
		 * Returns true if the player is ready for input.
		 *
		 * \note When the player's state is \a Disconnected, this
		 * function still returns true if all the cleanup following
		 * the disconnection is done.
		 */
		virtual bool isReady() const;

		/*! Returns the player's state. */
		State state() const;

		/*! Returns true if a parsing error occured. */
		bool hasError() const;
		/*! Returns a detailed description of the error. */
		QString errorString() const;

		/*!
		 * Prepares the player for a new chess game, and then calls
		 * startGame() to start the game.
		 *
		 * \param side The side (color) the player should play as. It
		 * can be NoSide if the player is in force/observer mode.
		 * \param opponent The opposing player.
		 * \param board The chessboard on which the game is played.
		 *
		 * \sa startGame()
		 */
		void newGame(Chess::Side side,
			     ChessPlayer* opponent,
			     Chess::Board* board);
		
		/*!
		 * Tells the player that the game ended by \a result.
		 *
		 * \note Subclasses that reimplement this function must call
		 * the base implementation.
		 */
		virtual void endGame(const Chess::Result& result);
		
		/*! Returns the player's evaluation of the current position. */
		const MoveEvaluation& evaluation() const;

		/*! Returns the player's time control. */
		const TimeControl* timeControl() const;

		/*! Sets the time control for the player. */
		void setTimeControl(const TimeControl& timeControl);

		/*! Returns the side of the player. */
		Chess::Side side() const;

		/*!
		 * Sends the next move of an ongoing game to the player.
		 * If the player is in force/observer mode, the move wasn't
		 * necessarily made by the opponent.
		 */
		virtual void makeMove(const Chess::Move& move) = 0;
		
		/*! Forces the player to play \a move as its next move. */
		virtual void makeBookMove(const Chess::Move& move);
		
		/*! Returns the player's name. */
		QString name() const;
		
		/*! Sets the player's name. */
		void setName(const QString& name);

		/*! Returns true if the player can play \a variant. */
		virtual bool supportsVariant(const QString& variant) const = 0;

		/*!
		 * Tells the player to start pondering (thinking on the
		 * opponent's move). The default implementation does nothing.
		 */
		virtual void startPondering();

		/*! Clears the player's pondering state. */
		virtual void clearPonderState();

		/*! Returns true if the player is human. */
		virtual bool isHuman() const = 0;

		/*!
		 * Returns true if result claims from the engine are validated;
		 * otherwise returns false.
		 *
		 * With validation on (the default) the engine will forfeit the
		 * game if it makes an incorrect result claim.
		 */
		bool areClaimsValidated() const;
		/*! Sets result claim validation mode to \a validate. */
		void setClaimsValidated(bool validate);

		/*!
		 * If \a enable is true the player will be allowed to continue
		 * the game after time is out, else the player will forfeit.
		 */
		void setCanPlayAfterTimeout(bool enable);


	public slots:
		/*!
		 * Waits (without blocking) until the player is ready,
		 * starts the chess clock, and tells the player to start thinking
		 * of the next move.
		 *
		 * \note Subclasses that reimplement this function must call
		 * the base implementation.
		 */
		virtual void go();

		/*! Terminates the player non-violently. */
		virtual void quit();

		/*!
		 * Kills the player process or connection, causing it to
		 * exit immediately.
		 *
		 * The player's state is set to \a Disconnected.
		 *
		 * \note Subclasses that reimplement this function must call
		 * the base implementation.
		 */
		virtual void kill();

	signals:
		/*! This signal is emitted when the player disconnects. */
		void disconnected();

		/*! Signals that the player is ready for input. */
		void ready();
		
		/*!
		 * Signals the time left in the player's clock when they
		 * start thinking of their next move.
		 * \param timeLeft Time left in milliseconds.
		 */
		void startedThinking(int timeLeft);

		/*!
		 * This signal is emitted when the player stops thinking of
		 * a move. Note that it doesn't necessarily mean that the
		 * player has made a move - they could've lost the game on
		 * time, disconnected, etc.
		 */
		void stoppedThinking();

		/*!
		 * This signal is emitted when the player's evaluation of the
		 * current position changes.
		 */
		void thinking(const MoveEvaluation& eval);

		/*! Signals the player's move. */
		void moveMade(const Chess::Move& move);
		
		/*!
		 * Emitted when the player claims the game to end
		 * with result \a result.
		 */
		void resultClaim(const Chess::Result& result);

		/*! Signals a debugging message from the player. */
		void debugMessage(const QString& data);

		/*! Emitted when player's name is changed. */
		void nameChanged(const QString& name);

	protected slots:
		/*!
		 * Called when the player's process or connection
		 * crashes unexpectedly.
		 *
		 * Forfeits the game.
		 */
		virtual void onCrashed();

		/*!
		 * Called when the player's flag falls.
		 * Forfeits the game.
		 */
		virtual void onTimeout();

	protected:
		/*! Returns the chessboard on which the player is playing. */
		Chess::Board* board();

		/*! Starts the chess game set up by newGame(). */
		virtual void startGame() = 0;

		/*!
		 * Tells the player to start thinking of the next move.
		 *
		 * The player is guaranteed to be ready to move when this
		 * function is called by go().
		 */
		virtual void startThinking() = 0;

		/*!
		 * Returns true if this player can keep playing after running
		 * out of time; otherwise returns false.
		 */
		virtual bool canPlayAfterTimeout() const;

		/*! Emits the resultClaim() signal with result \a result. */
		void claimResult(const Chess::Result& result);
		/*!
		 * Emits the resultClaim() signal with a result of type
		 * \a type, description \a description, and this player
		 * as the loser.
		 */
		void forfeit(Chess::Result::Type type,
			     const QString& description = QString());

		/*!
		 * Emits the player's move, and a timeout signal if the
		 * move came too late.
		 */
		void emitMove(const Chess::Move& move);
		
		/*! Returns the opposing player. */
		const ChessPlayer* opponent() const;

		/*! Sets the player's state to \a state. */
		void setState(State state);

		/*! Sets the current error to \a error. */
		void setError(const QString& error);
		
		/*!
		 * Move evaluation for the current move.
		 *
		 * This move evaluation may have eval info from multiple search
		 * depths if the highest depth doesn't contain a complete
		 * evaluation.
		 */
		MoveEvaluation m_eval;

	private:
		void startClock();

		QString m_name;
		QString m_error;
		State m_state;
		TimeControl m_timeControl;
		QTimer* m_timer;
		bool m_claimedResult;
		bool m_validateClaims;
		bool m_canPlayAfterTimeout;
		Chess::Side m_side;
		Chess::Board* m_board;
		ChessPlayer* m_opponent;
};

#endif // CHESSPLAYER_H
