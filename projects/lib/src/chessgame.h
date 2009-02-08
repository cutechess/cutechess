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

#ifndef CHESSGAME_H
#define CHESSGAME_H

#include <QObject>
#include "chessboard/chess.h"

class ChessPlayer;
class OpeningBook;

class LIB_EXPORT ChessGame : public QObject
{
	Q_OBJECT

	public:
		ChessGame(QObject *parent = 0,
		          Chess::Variant variant = Chess::StandardChess);
		~ChessGame();
		Chess::Board* chessboard() const;
		void newGame(ChessPlayer* whitePlayer,
		             ChessPlayer* blackPlayer,
		             const QString& fen = "",
		             OpeningBook* book = 0);
		ChessPlayer* whitePlayer() const;
		ChessPlayer* blackPlayer() const;
		Chess::Result result() const;

	public slots:
		void moveMade(const Chess::Move& move);
		void resign();
		void onTimeout();

	signals:
		void moveHappened(const Chess::Move& move);
		void gameEnded();

	private:
		Chess::Move bookMove();
		void endGame();
		
		Chess::Board* m_chessboard;
		ChessPlayer* m_whitePlayer;
		ChessPlayer* m_blackPlayer;
		ChessPlayer* m_playerToMove;
		OpeningBook* m_book;
		bool m_gameInProgress;
		int m_moveCount;
		Chess::Result m_result;
};

#endif // CHESSGAME_H
