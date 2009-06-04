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
#include <QVector>
#include "chessboard/chess.h"
#include "chessboard/result.h"
#include "chessboard/variant.h"
#include "chessboard/chessmove.h"
#include "pgngame.h"

class ChessPlayer;
class OpeningBook;
class MoveEvaluation;

class LIB_EXPORT ChessGame : public QObject, public PgnGame
{
	Q_OBJECT

	public:
		explicit ChessGame(Chess::Variant variant, QObject* parent = 0);
		
		Chess::Board* board() const;
		ChessPlayer* player(Chess::Side) const;
		void setPlayer(Chess::Side, ChessPlayer* player);
		bool setFenString(const QString& fen);
		void setOpeningBook(const OpeningBook* book, int maxMoves = 1000);
		void setOpeningMoves(const QVector<PgnGame::MoveData>& moves);
		void setDrawThreshold(int moveNumber, int score);
		void setResignThreshold(int moveCount, int score);

	public slots:
		void start();
		void stop();
		void kill();
		void onMoveMade(const Chess::Move& move);

	signals:
		void moveMade(const Chess::Move& move);
		void gameEnded();
		void playersReady();

	private slots:
		void onForfeit(Chess::Result result);
		void syncPlayers(bool ignoreSender = false);

	private:
		void adjudication(const MoveEvaluation& eval);
		bool arePlayersReady() const;
		Chess::Move bookMove(const OpeningBook* book);
		ChessPlayer* playerToMove();
		ChessPlayer* playerToWait();
		void setBoard();
		
		Chess::Board* m_board;
		ChessPlayer* m_player[2];
		bool m_gameEnded;
		bool m_gameInProgress;
		int m_drawMoveNum;
		int m_drawScore;
		int m_drawScoreCount;
		int m_resignMoveCount;
		int m_resignScore;
		int m_resignScoreCount[2];
};

#endif // CHESSGAME_H
