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
#include <QString>
#include <chessboard/variant.h>
#include <engineconfiguration.h>
#include <enginesettings.h>
#include <engineprocess.h>
#include <pgnfile.h>
#include <pgngame.h>

class ChessGame;
class OpeningBook;

class EngineMatch : public QObject
{
	Q_OBJECT

	public:
		EngineMatch(QObject* parent = 0);
		~EngineMatch();

		void addEngine(const EngineConfiguration& config,
			       const EngineSettings& settings);
		void setBookDepth(int bookDepth);
		void setBookFile(const QString& filename);
		void setDebugMode(bool debug);
		void setDrawThreshold(int moveNumber, int score);
		void setEvent(const QString& event);
		void setGameCount(int gameCount);
		void setPgnInput(const QString& filename);
		void setPgnOutput(const QString& filename,
				  PgnGame::PgnMode mode);
		void setRepeatOpening(bool repeatOpening);
		void setResignThreshold(int moveCount, int score);
		void setSite(const QString& site);
		void setVariant(Chess::Variant variant);
		void setWait(int msecs);
		bool initialize();

	public slots:
		void start();
		void stop();

	signals:
		void finished();
		void stopGame();

	private slots:
		void onGameEnded();
		void print(const QString& msg);

	private:
		struct EngineData
		{
			EngineConfiguration config;
			EngineSettings settings;
			int wins;
			ChessEngine* engine;
			EngineProcess* process;
		};

		void killEngines();

		int m_bookDepth;
		int m_gameCount;
		int m_drawCount;
		int m_currentGame;
		int m_pgnGamesRead;
		int m_drawMoveNum;
		int m_drawScore;
		int m_resignMoveCount;
		int m_resignScore;
		int m_wait;
		EngineData* m_white;
		EngineData* m_black;
		OpeningBook* m_book;
		bool m_debug;
		PgnGame::PgnMode m_pgnMode;
		bool m_repeatOpening;
		ChessGame* m_game;
		Chess::Variant m_variant;
		QVector<EngineData> m_engines;
		QVector<PgnGame::MoveData> m_openingMoves;
		QString m_fen;
		QString m_event;
		QString m_site;
		PgnFile m_pgnInput;
		QString m_pgnOutput;
};

#endif // ENGINEMATCH_H
