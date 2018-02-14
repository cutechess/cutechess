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

#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include "chessplayer.h"
#include "board/genericmove.h"


/*!
 * \brief A chess player controlled by a human user.
 *
 * A HumanPlayer object works between a graphical chessboard
 * and a ChessGame object by forwarding the user's moves to the
 * game.
 *
 * Typically human players are created by using a HumanBuilder
 * object.
 */
class LIB_EXPORT HumanPlayer : public ChessPlayer
{
	Q_OBJECT

	public:
		/*! Creates a new human player. */
		HumanPlayer(QObject* parent = nullptr);

		// Inherted from ChessPlayer
		virtual void endGame(const Chess::Result& result);
		virtual void makeMove(const Chess::Move& move);
		virtual bool supportsVariant(const QString& variant) const;
		virtual bool isHuman() const;

	public slots:
		/*!
		 * Plays \a move as the human player's next move if
		 * \a side is the player's side and the move is legal;
		 * otherwise does nothing.
		 *
		 * If the player is in \a Thinking state, it plays
		 * the move immediately. If its in \a Observing state,
		 * it saves the move for later, emits the wokeUp() signal,
		 * and plays the move when it gets its turn.
		 */
		void onHumanMove(const Chess::GenericMove& move,
				 const Chess::Side& side);

	signals:
		/*!
		 * This signal is emitted when the player receives a
		 * user-made move in \a Observing state.
		 *
		 * Normally this signal is connected to ChessGame::resume()
		 * to resume a paused game when the user makes a move.
		 */
		void wokeUp();
		
	protected:
		// Inherited from ChessPlayer
		virtual void startGame();
		virtual void startThinking();

	private:
		Chess::GenericMove m_bufferMove;
};

#endif // HUMANPLAYER_H
