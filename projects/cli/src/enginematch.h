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
#include <timecontrol.h>

class QProcess;
class OpeningBook;

class EngineMatch : public QObject
{
	Q_OBJECT

	public:
		EngineMatch(QObject* parent = 0);
		~EngineMatch();

		void addEngine(const EngineConfiguration& engineConfig);
		void setBookDepth(int bookDepth);
		void setBookFile(const QString& filename);
		void setDebugMode(bool debug);
		void setEvent(const QString& event);
		void setGameCount(int gameCount);
		void setPgnOutput(const QString& filename);
		void setPondering(bool pondering);
		void setSite(const QString& site);
		void setVariant(Chess::Variant variant);
		void setTimeControl(const TimeControl& timeControl);
		bool initialize();

	public slots:
		void start();

	signals:
		void finished();

	private slots:
		void onGameEnded();
		void print(const QString& msg);

	private:
		struct EngineData
		{
			EngineConfiguration engineConfig;
			int wins;
			ChessEngine* engine;
			QProcess* process;
		};

		void killEngines();

		int m_bookDepth;
		int m_gameCount;
		int m_drawCount;
		int m_currentGame;
		EngineData* m_white;
		EngineData* m_black;
		OpeningBook* m_book;
		bool m_debug;
		bool m_pondering;
		Chess::Variant m_variant;
		TimeControl m_timeControl;
		QVector<EngineData> m_engines;
		QString m_event;
		QString m_site;
		QString m_pgnOutput;
};

#endif // ENGINEMATCH_H
