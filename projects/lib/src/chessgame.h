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
#include "chessboard/result.h"

class ChessPlayer;
class OpeningBook;

class LIB_EXPORT ChessGame : public QObject
{
	Q_OBJECT

	public:
		explicit ChessGame(Chess::Variant variant, QObject* parent = 0);
		~ChessGame();
		
		Chess::Board* board() const;
		ChessPlayer* player(Chess::Side) const;
		void setPlayer(Chess::Side, ChessPlayer* player);
		void setFenString(const QString& fen);
		void setOpeningBook(OpeningBook* book);
		Chess::Result result() const;

		void start();

	public slots:
		void onMoveMade(const Chess::Move& move);
		void onTimeout();
		void resign();

	signals:
		void moveMade(const Chess::Move& move);
		void gameEnded();

	private:
		Chess::Move bookMove();
		void endGame();
		ChessPlayer* playerToMove();
		ChessPlayer* playerToWait();
		
		Chess::Board* m_board;
		QString m_fen;
		ChessPlayer* m_player[2];
		OpeningBook* m_book;
		bool m_gameInProgress;
		int m_moveCount;
		Chess::Result m_result;
};

#endif // CHESSGAME_H
