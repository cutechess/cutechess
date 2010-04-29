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
#include <QStringList>
#include "pgngame.h"
#include "board/result.h"
#include "board/move.h"
#include "timecontrol.h"

namespace Chess { class Board; }
class ChessPlayer;
class OpeningBook;
class MoveEvaluation;
class QThread;


class LIB_EXPORT ChessGame : public QObject
{
	Q_OBJECT

	public:
		explicit ChessGame(Chess::Board* board, QObject* parent = 0);
		
		ChessPlayer* player(Chess::Side side) const;

		PgnGame& pgn();
		QString startingFen() const;
		const QVector<Chess::Move>& moves() const;
		Chess::Result result() const;

		void setPlayer(Chess::Side side, ChessPlayer* player);
		void setStartingFen(const QString& fen);
		void setTimeControl(const TimeControl& timeControl,
				    Chess::Side side = Chess::Side());
		void setMoves(const QVector<Chess::Move>& moves);
		void setMoves(const PgnGame& pgn);
		void setOpeningBook(const OpeningBook* book,
				    Chess::Side side = Chess::Side(),
				    int depth = 1000);
		void setDrawThreshold(int moveNumber, int score);
		void setResignThreshold(int moveCount, int score);

		void generateOpening();

	public slots:
		void start(QThread* thread = 0);
		void stop();
		void kill();
		void onMoveMade(const Chess::Move& move);

	signals:
		void humanEnabled(bool);
		void moveMade(const Chess::Move& move);
		void gameEnded();
		void playersReady();

	private slots:
		void startGame();
		void sendGameEnded();
		void finish();
		void onForfeit(const Chess::Result& result);
		void syncPlayers(bool ignoreSender = false);

	private:
		void adjudication(const MoveEvaluation& eval);
		bool arePlayersReady() const;
		Chess::Move bookMove(Chess::Side side);
		void startTurn();
		ChessPlayer* playerToMove();
		ChessPlayer* playerToWait();
		void resetBoard();
		void initializePgn();
		void addPgnMove(const Chess::Move& move, const QString& comment);
		
		Chess::Board* m_board;
		QThread* m_origThread;
		ChessPlayer* m_player[2];
		TimeControl m_timeControl[2];
                const OpeningBook* m_book[2];
		int m_bookDepth[2];
		bool m_gameEnded;
		bool m_gameInProgress;
		int m_drawMoveNum;
		int m_drawScore;
		int m_drawScoreCount;
		int m_resignMoveCount;
		int m_resignScore;
		int m_resignScoreCount[2];
		QString m_startingFen;
		Chess::Result m_result;
		QVector<Chess::Move> m_moves;
		PgnGame m_pgn;
};

#endif // CHESSGAME_H
