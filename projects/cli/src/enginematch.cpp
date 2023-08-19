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

#include "enginematch.h"
#include <QMultiMap>
#include <chessplayer.h>
#include <playerbuilder.h>
#include <chessgame.h>
#include <polyglotbook.h>
#include <tournament.h>
#include <gamemanager.h>
#include <sprt.h>


EngineMatch::EngineMatch(Tournament* tournament, QObject* parent)
	: QObject(parent),
	  m_tournament(tournament),
	  m_debug(false),
	  m_matchState(FreeState),
	  m_ratingInterval(0),
	  m_outcomeInterval(0),
	  m_bookMode(OpeningBook::Ram),
	  m_suspendFile("_cc-suspend"),
	  m_resumeFile("_cc-resume")
{
	Q_ASSERT(tournament != nullptr);

	m_filesystemwatcher.addPath(".");
	m_startTime.start();
}

EngineMatch::~EngineMatch()
{
	qDeleteAll(m_books);
}

OpeningBook* EngineMatch::addOpeningBook(const QString& fileName)
{
	if (fileName.isEmpty())
		return nullptr;

	if (m_books.contains(fileName))
		return m_books[fileName];

	PolyglotBook* book = new PolyglotBook(m_bookMode);
	if (!book->read(fileName))
	{
		delete book;
		qWarning("Can't read opening book file %s", qUtf8Printable(fileName));
		return nullptr;
	}

	m_books[fileName] = book;
	return book;
}

void EngineMatch::start()
{
	connect(m_tournament, SIGNAL(finished()),
		this, SLOT(onTournamentFinished()));
	connect(m_tournament, SIGNAL(gameStarted(ChessGame*, int, int, int)),
		this, SLOT(onGameStarted(ChessGame*, int)));
	connect(m_tournament, SIGNAL(gameFinished(ChessGame*, int, int, int)),
		this, SLOT(onGameFinished(ChessGame*, int)));
	connect(m_tournament, SIGNAL(suspended(int)),
		this, SLOT(onTournamentSuspended(int)));

	if (m_debug)
		connect(m_tournament->gameManager(), SIGNAL(debugMessage(QString)),
			this, SLOT(print(QString)));

	QMetaObject::invokeMethod(m_tournament, "start", Qt::QueuedConnection);

	if (m_resumeFile.exists())
		m_resumeFile.remove();
	if (m_suspendFile.exists())
		m_suspendFile.remove();

	connect(&m_filesystemwatcher, SIGNAL(directoryChanged(QString)),
		SLOT(onControlDirectoryChanged()));
}

void EngineMatch::stop()
{
	disconnect(&m_filesystemwatcher);

	QMetaObject::invokeMethod(m_tournament, "stop", Qt::QueuedConnection);
}

void EngineMatch::suspend()
{
	if (m_matchState != FreeState)
		return;

	m_matchState = SuspendingState;
	print("Suspending match: no new games will be started, active games continue.");
	QMetaObject::invokeMethod(m_tournament, "suspend", Qt::QueuedConnection);
}

void EngineMatch::resume()
{
	if (m_matchState != SuspendedState)
		return;

	m_matchState = ResumingState;
	print("Resuming match.");
	QMetaObject::invokeMethod(m_tournament, "resume", Qt::QueuedConnection);
	m_matchState = FreeState;
}


void EngineMatch::setDebugMode(bool debug)
{
	m_debug = debug;
}

void EngineMatch::setRatingInterval(int interval)
{
	Q_ASSERT(interval >= 0);
	m_ratingInterval = interval;
}

void EngineMatch::setOutcomeInterval(int interval)
{
	Q_ASSERT(interval >= 0);
	m_outcomeInterval = interval;
}

void EngineMatch::setBookMode(OpeningBook::AccessMode mode)
{
	m_bookMode = mode;
}

void EngineMatch::onGameStarted(ChessGame* game, int number)
{
	Q_ASSERT(game != nullptr);

	qInfo("Started game %d of %d (%s vs %s)",
	      number,
	      m_tournament->finalGameCount(),
	      qUtf8Printable(game->player(Chess::Side::White)->name()),
	      qUtf8Printable(game->player(Chess::Side::Black)->name()));
}

void EngineMatch::onGameFinished(ChessGame* game, int number)
{
	Q_ASSERT(game != nullptr);

	Chess::Result result(game->result());
	qInfo("Finished game %d (%s vs %s): %s",
	      number,
	      qUtf8Printable(game->player(Chess::Side::White)->name()),
	      qUtf8Printable(game->player(Chess::Side::Black)->name()),
	      qUtf8Printable(result.toVerboseString()));

	if (m_tournament->playerCount() == 2)
	{
		TournamentPlayer fcp = m_tournament->playerAt(0);
		TournamentPlayer scp = m_tournament->playerAt(1);
		int totalResults = fcp.gamesFinished();
		qInfo("Score of %s vs %s: %d - %d - %d  [%.3f] %d",
		      qUtf8Printable(fcp.name()),
		      qUtf8Printable(scp.name()),
		      fcp.wins(), scp.wins(), fcp.draws(),
		      double(fcp.score()) / (totalResults * 2),
		      totalResults);
	}

	if (m_ratingInterval != 0
	&&  (m_tournament->finishedGameCount() % m_ratingInterval) == 0)
		printRanking();
	if (m_outcomeInterval != 0
	&&  (m_tournament->finishedGameCount() % m_outcomeInterval) == 0)
		printOutcomes();
}

void EngineMatch::onTournamentFinished()
{
	if (m_ratingInterval == 0
	||  m_tournament->finishedGameCount() % m_ratingInterval != 0)
		printRanking();
	if (m_outcomeInterval == 0
	||  m_tournament->finishedGameCount() % m_outcomeInterval != 0)
		printOutcomes();

	QString error = m_tournament->errorString();
	if (!error.isEmpty())
		qWarning("%s", qUtf8Printable(error));

	qInfo("Finished match");
	connect(m_tournament->gameManager(), SIGNAL(finished()),
		this, SIGNAL(finished()));
	m_tournament->gameManager()->finish();
}

void EngineMatch::onTournamentSuspended(int count)
{
	printRanking();
	print(QString("Suspended match after %0 games").arg(count));
	m_matchState = SuspendedState;
}

void EngineMatch::print(const QString& msg)
{
	qInfo("%lld %s", m_startTime.elapsed(), qUtf8Printable(msg));
}

void EngineMatch::printRanking()
{
	qInfo("%s", qUtf8Printable(m_tournament->results()));
}

void EngineMatch::printOutcomes()
{
	qInfo("%s", qUtf8Printable(m_tournament->outcomes()));
}

void EngineMatch::onControlDirectoryChanged()
{
	if (m_matchState == FreeState && m_suspendFile.exists())
	{
		suspend();
	}
	else if (m_matchState == SuspendedState && m_resumeFile.exists())
	{
		resume();
	}

	if (m_resumeFile.exists())
		m_resumeFile.remove();
	if (m_suspendFile.exists())
		m_suspendFile.remove();
}
