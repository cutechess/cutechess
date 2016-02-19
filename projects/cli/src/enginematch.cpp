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
	  m_ratingInterval(0)
{
	Q_ASSERT(tournament != 0);

	m_startTime.start();
}

EngineMatch::~EngineMatch()
{
	qDeleteAll(m_books);
}

OpeningBook* EngineMatch::addOpeningBook(const QString& fileName)
{
	if (fileName.isEmpty())
		return 0;

	if (m_books.contains(fileName))
		return m_books[fileName];

	PolyglotBook* book = new PolyglotBook;
	if (!book->read(fileName))
	{
		delete book;
		qWarning("Can't read opening book file %s", qPrintable(fileName));
		return 0;
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

	if (m_debug)
		connect(m_tournament->gameManager(), SIGNAL(debugMessage(QString)),
			this, SLOT(print(QString)));

	QMetaObject::invokeMethod(m_tournament, "start", Qt::QueuedConnection);
}

void EngineMatch::stop()
{
	QMetaObject::invokeMethod(m_tournament, "stop", Qt::QueuedConnection);
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

void EngineMatch::onGameStarted(ChessGame* game, int number)
{
	Q_ASSERT(game != 0);

	qDebug("Started game %d of %d (%s vs %s)",
	       number,
	       m_tournament->finalGameCount(),
	       qPrintable(game->player(Chess::Side::White)->name()),
	       qPrintable(game->player(Chess::Side::Black)->name()));
}

void EngineMatch::onGameFinished(ChessGame* game, int number)
{
	Q_ASSERT(game != 0);

	Chess::Result result(game->result());
	qDebug("Finished game %d (%s vs %s): %s",
	       number,
	       qPrintable(game->player(Chess::Side::White)->name()),
	       qPrintable(game->player(Chess::Side::Black)->name()),
	       qPrintable(result.toVerboseString()));

	if (m_tournament->playerCount() == 2)
	{
		Tournament::PlayerData fcp = m_tournament->playerAt(0);
		Tournament::PlayerData scp = m_tournament->playerAt(1);
		int totalResults = fcp.wins + fcp.losses + fcp.draws;
		qDebug("Score of %s vs %s: %d - %d - %d  [%.3f] %d",
		       qPrintable(fcp.builder->name()),
		       qPrintable(scp.builder->name()),
		       fcp.wins, scp.wins, fcp.draws,
		       double(fcp.wins * 2 + fcp.draws) / (totalResults * 2),
		       totalResults);
	}

	if (m_ratingInterval != 0
	&&  (m_tournament->finishedGameCount() % m_ratingInterval) == 0)
		printRanking();
}

void EngineMatch::onTournamentFinished()
{
	if (m_ratingInterval == 0
	||  m_tournament->finishedGameCount() % m_ratingInterval != 0)
		printRanking();

	QString error = m_tournament->errorString();
	if (!error.isEmpty())
		qWarning("%s", qPrintable(error));

	qDebug("Finished match");
	connect(m_tournament->gameManager(), SIGNAL(finished()),
		this, SIGNAL(finished()));
	m_tournament->gameManager()->finish();
}

void EngineMatch::print(const QString& msg)
{
	qDebug("%lld %s", m_startTime.elapsed(), qPrintable(msg));
}

void EngineMatch::printRanking()
{
	qDebug("%s", qPrintable(m_tournament->results()));
}
