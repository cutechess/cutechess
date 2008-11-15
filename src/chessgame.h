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

#ifndef CHESSGAME_H
#define CHESSGAME_H

#include <QObject>
#include <QTime>
#include "chessboard/chess.h"

class ChessPlayer;

class ChessGame : public QObject
{
	Q_OBJECT

	public:
		ChessGame(QObject *parent = 0);
		~ChessGame();
		Chess::Board* chessboard() const;
		void newGame(ChessPlayer* whitePlayer, ChessPlayer* blackPlayer);
		ChessPlayer* whitePlayer() const;
		ChessPlayer* blackPlayer() const;

	public slots:
		void moveMade(const Chess::Move& move);

	signals:
		void moveHappened(const Chess::Move& move);

	private:
		Chess::Board* m_chessboard;
		ChessPlayer* m_whitePlayer;
		ChessPlayer* m_blackPlayer;
		ChessPlayer* m_playerToMove;
		bool m_gameInProgress;
		int m_moveCount;
		QTime m_timer;
};

#endif // CHESSGAME_H

