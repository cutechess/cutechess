/*
    This file is part of Cute Chess.
    Copyright (C) 2008-2018 Cute Chess authors

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
#include <QMap>
#include <QString>
#include <QTimer>
#include <QElapsedTimer>
#include <QFileSystemWatcher>
#include <QFile>
#include <openingbook.h>

class ChessGame;
class OpeningBook;
class Tournament;


class EngineMatch : public QObject
{
	Q_OBJECT

	public:
		enum MatchSuspensionState
		{
			FreeState,
			SuspendingState,
			SuspendedState,
			ResumingState
		};

		EngineMatch(Tournament* tournament, QObject* parent = nullptr);
		virtual ~EngineMatch();

		OpeningBook* addOpeningBook(const QString& fileName);
		void setDebugMode(bool debug);
		void setRatingInterval(int interval);
		void setOutcomeInterval(int interval);
		void setBookMode(OpeningBook::AccessMode mode);

		void start();
		void stop();
		void suspend();
		void resume();

	signals:
		void finished();

	private slots:
		void onGameStarted(ChessGame* game, int number);
		void onGameFinished(ChessGame* game, int number);
		void onTournamentFinished();
		void onTournamentSuspended(int count);
		void print(const QString& msg);
		void onControlDirectoryChanged();

	private:
		void printRanking();
		void printOutcomes();

		Tournament* m_tournament;
		bool m_debug;
		MatchSuspensionState m_matchState;
		int m_ratingInterval;
		int m_outcomeInterval;
		OpeningBook::AccessMode m_bookMode;
		QMap<QString, OpeningBook*> m_books;
		QElapsedTimer m_startTime;
		QFileSystemWatcher m_filesystemwatcher;
		QFile m_suspendFile;
		QFile m_resumeFile;
};

#endif // ENGINEMATCH_H
