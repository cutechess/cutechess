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

#ifndef ENGINEMATCH_H
#define ENGINEMATCH_H

#include <QObject>
#include <QVector>
#include <QMap>
#include <QString>
#include <QTime>
#include <QFile>
#include <board/move.h>
#include <engineconfiguration.h>
#include <timecontrol.h>
#include <pgnstream.h>
#include <pgngame.h>
#include <gamemanager.h>

class ChessGame;
class OpeningBook;
class EngineBuilder;


class EngineMatch : public QObject
{
	Q_OBJECT

	public:
		EngineMatch(QObject* parent = 0);
		virtual ~EngineMatch();

		void addEngine(const EngineConfiguration& config,
			       const TimeControl& timeControl,
			       const QString& book,
			       int bookDepth);
		bool setConcurrency(int concurrency);
		void setDebugMode(bool debug);
		void setDrawThreshold(int moveNumber, int score);
		void setEvent(const QString& event);
		bool setGameCount(int gameCount);
		bool setPgnDepth(int depth);
		bool setPgnInput(const QString& filename);
		void setPgnOutput(const QString& filename,
				  PgnGame::PgnMode mode);
		void setRecoveryMode(bool recover);
		void setRepeatOpening(bool repeatOpening);
		void setResignThreshold(int moveCount, int score);
		void setSite(const QString& site);
		bool setVariant(const QString& variant);
		bool setWait(int msecs);
		bool initialize();

	public slots:
		void start();
		void stop();

	signals:
		void finished();
		void stopGame();

	private slots:
		void onGameEnded();
		void onManagerReady();
		void print(const QString& msg);

	private:
		bool loadOpeningBook(const QString& filename, OpeningBook** book);

		struct EngineData
		{
			EngineConfiguration config;
			TimeControl tc;
			OpeningBook* book;
			QString bookFile;
			int bookDepth;
			int wins;
			EngineBuilder* builder;
		};

		int m_gameCount;
		int m_drawCount;
		int m_currentGame;
		int m_finishedGames;
		int m_pgnDepth;
		int m_pgnGamesRead;
		int m_drawMoveNum;
		int m_drawScore;
		int m_resignMoveCount;
		int m_resignScore;
		int m_wait;
		bool m_debug;
		bool m_recover;
		bool m_finishing;
		PgnGame::PgnMode m_pgnMode;
		bool m_repeatOpening;
		QString m_variant;
		QVector<EngineData> m_engines;
		EngineData* m_fcp;
		EngineData* m_scp;
		QVector<Chess::Move> m_openingMoves;
		QList<OpeningBook*> m_books;
		QList<EngineBuilder*> m_builders;
		QString m_fen;
		QString m_event;
		QString m_site;
		QFile m_pgnInputFile;
		PgnStream m_pgnInputStream;
		QString m_pgnOutput;
		QTime m_startTime;
		GameManager m_manager;
		QMap<int, PgnGame*> m_games;
};

#endif // ENGINEMATCH_H
