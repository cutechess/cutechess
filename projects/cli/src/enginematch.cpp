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
#include <cmath>
#include <chessplayer.h>
#include <playerbuilder.h>
#include <chessgame.h>
#include <polyglotbook.h>
#include <tournament.h>
#include <gamemanager.h>


EngineMatch::EngineMatch(Tournament* tournament, QObject* parent)
	: QObject(parent),
	  m_tournament(tournament),
	  m_debug(false)
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

void EngineMatch::onGameStarted(ChessGame* game, int number)
{
	Q_ASSERT(game != 0);

	qDebug("Started game %d of %d",
	       number,
	       m_tournament->finalGameCount());
}

void EngineMatch::onGameFinished(ChessGame* game, int number)
{
	Q_ASSERT(game != 0);

	Chess::Result result(game->result());
	qDebug("Game %d ended: %s", number, qPrintable(result.toVerboseString()));
	if (!result.winner().isNull())
		qDebug("%s wins the game as %s",
		       qPrintable(game->player(result.winner())->name()),
		       qPrintable(result.winner().toString()));

	Tournament::PlayerData fcp = m_tournament->playerAt(0);
	Tournament::PlayerData scp = m_tournament->playerAt(1);
	int totalResults = fcp.wins + fcp.losses + fcp.draws;
	qDebug("Score of %s vs %s: %d - %d - %d  [%.2f] %d",
	       qPrintable(fcp.builder->name()),
	       qPrintable(scp.builder->name()),
	       fcp.wins, scp.wins, fcp.draws,
	       double(fcp.wins * 2 + fcp.draws) / (totalResults * 2),
	       totalResults);
}

void EngineMatch::onTournamentFinished()
{
	Tournament::PlayerData fcp = m_tournament->playerAt(0);

	int score = fcp.wins * 2 + fcp.draws;
	int total = (fcp.wins + fcp.losses + fcp.draws) * 2;
	if (total > 0)
	{
		double ratio = double(score) / double(total);
		double eloDiff = -400.0 * std::log(1.0 / ratio - 1.0) / std::log(10.0);
		qDebug("ELO difference: %.0f", eloDiff);
	}

	qDebug("Finished match");
	connect(m_tournament->gameManager(), SIGNAL(finished()),
		this, SIGNAL(finished()));
	m_tournament->gameManager()->finish();
}

void EngineMatch::print(const QString& msg)
{
	qDebug("%d %s", m_startTime.elapsed(), qPrintable(msg));
}
