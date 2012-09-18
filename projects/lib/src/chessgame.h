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
#include <QSemaphore>
#include "pgngame.h"
#include "board/result.h"
#include "board/move.h"
#include "timecontrol.h"

namespace Chess { class Board; }
class ChessPlayer;
class OpeningBook;
class MoveEvaluation;


class LIB_EXPORT ChessGame : public QObject
{
	Q_OBJECT

	public:
		ChessGame(Chess::Board* board, PgnGame* pgn, QObject* parent = 0);
		virtual ~ChessGame();
		
		ChessPlayer* player(Chess::Side side) const;
		ChessPlayer* playerToMove() const;
		ChessPlayer* playerToWait() const;
		bool isFinished() const;

		PgnGame* pgn() const;
		Chess::Board* board() const;
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
		void setStartDelay(int time);

		void generateOpening();

		void lockThread();
		void unlockThread();

	public slots:
		void start();
		void pause();
		void resume();
		void stop();
		void kill();
		void emitStartFailed();
		void onMoveMade(const Chess::Move& move);

	signals:
		void humanEnabled(bool);
		void fenChanged(const QString& fenString);
		void moveMade(const Chess::GenericMove& move,
			      const QString& sanString,
			      const QString& comment);
		void started(ChessGame* game = 0);
		void finished(ChessGame* game = 0);
		void startFailed(ChessGame* game = 0);
		void playersReady();

	private slots:
		void startGame();
		void startTurn();
		void finish();
		void onResultClaim(const Chess::Result& result);
		void onPlayerReady();
		void syncPlayers();
		void pauseThread();

	private:
		void adjudication(const MoveEvaluation& eval);
		Chess::Move bookMove(Chess::Side side);
		void resetBoard();
		void initializePgn();
		void addPgnMove(const Chess::Move& move, const QString& comment);
		void emitLastMove();
		
		Chess::Board* m_board;
		ChessPlayer* m_player[2];
		TimeControl m_timeControl[2];
                const OpeningBook* m_book[2];
		int m_bookDepth[2];
		int m_startDelay;
		bool m_finished;
		bool m_gameInProgress;
		bool m_paused;
		int m_drawMoveNum;
		int m_drawScore;
		int m_drawScoreCount;
		int m_resignMoveCount;
		int m_resignScore;
		int m_resignScoreCount[2];
		QString m_startingFen;
		Chess::Result m_result;
		QVector<Chess::Move> m_moves;
		PgnGame* m_pgn;
		QSemaphore m_pauseSem;
		QSemaphore m_resumeSem;
};

#endif // CHESSGAME_H
