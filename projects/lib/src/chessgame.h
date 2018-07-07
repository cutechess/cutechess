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
#include <QMap>
#include <QSemaphore>
#include "pgngame.h"
#include "board/result.h"
#include "board/move.h"
#include "timecontrol.h"
#include "gameadjudicator.h"

namespace Chess { class Board; }
class ChessPlayer;
class OpeningBook;
class MoveEvaluation;


class LIB_EXPORT ChessGame : public QObject
{
	Q_OBJECT

	public:
		ChessGame(Chess::Board* board, PgnGame* pgn, QObject* parent = nullptr);
		virtual ~ChessGame();
		
		QString errorString() const;
		ChessPlayer* player(Chess::Side side) const;
		ChessPlayer* playerToMove() const;
		ChessPlayer* playerToWait() const;
		bool isFinished() const;
		bool boardShouldBeFlipped() const;
		void setBoardShouldBeFlipped(bool flip);

		PgnGame* pgn() const;
		Chess::Board* board() const;
		QString startingFen() const;
		const QVector<Chess::Move>& moves() const;
		const QMap<int,int>& scores() const;
		Chess::Result result() const;

		void setError(const QString& message);
		void setPlayer(Chess::Side side, ChessPlayer* player);
		void setStartingFen(const QString& fen);
		void setTimeControl(const TimeControl& timeControl,
				    Chess::Side side = Chess::Side());
		void setMoves(const QVector<Chess::Move>& moves);
		bool setMoves(const PgnGame& pgn);
		void setOpeningBook(const OpeningBook* book,
				    Chess::Side side = Chess::Side(),
				    int depth = 1000);
		void setAdjudicator(const GameAdjudicator& adjudicator);
		void setStartDelay(int time);
		void setBookOwnership(bool enabled);

		void generateOpening();

		void lockThread();
		void unlockThread();

	public slots:
		void start();
		void pause();
		void resume();
		void stop(bool emitMoveChanged = true);
		void kill();
		void emitStartFailed();
		void onMoveMade(const Chess::Move& move);
		void onAdjudication(const Chess::Result& result);
		void onResignation(const Chess::Result& result);

	signals:
		void humanEnabled(bool);
		void fenChanged(const QString& fenString);
		void moveMade(const Chess::GenericMove& move,
			      const QString& sanString,
			      const QString& comment);
		void moveChanged(int ply,
				 const Chess::GenericMove& move,
				 const QString& sanString,
				 const QString& comment);
		void scoreChanged(int ply, int score);
		void initialized(ChessGame* game = nullptr);
		void started(ChessGame* game = nullptr);
		void finished(ChessGame* game = nullptr,
			      Chess::Result result = Chess::Result());
		void startFailed(ChessGame* game = nullptr);
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
		Chess::Move bookMove(Chess::Side side);
		bool resetBoard();
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
		bool m_pgnInitialized;
		bool m_bookOwnership;
		bool m_boardShouldBeFlipped;
		QString m_error;
		QString m_startingFen;
		Chess::Result m_result;
		QVector<Chess::Move> m_moves;
		QMap<int,int> m_scores;
		PgnGame* m_pgn;
		QSemaphore m_pauseSem;
		QSemaphore m_resumeSem;
		GameAdjudicator m_adjudicator;
};

#endif // CHESSGAME_H
