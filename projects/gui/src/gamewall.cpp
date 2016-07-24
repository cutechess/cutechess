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

#include "gamewall.h"

#include <QTimer>
#include <QSettings>

#include <chessplayer.h>
#include <chessgame.h>
#include <gamemanager.h>

#include "tilelayout.h"
#include "boardview/boardscene.h"
#include "boardview/boardview.h"
#include "chessclock.h"
#include "cutechessapp.h"

GameWall::GameWall(GameManager* manager, QWidget *parent)
	: QDialog(parent),
	  m_timer(new QTimer(this))
{
	setWindowTitle(tr("Game Wall"));
	setLayout(new TileLayout());

	foreach (ChessGame* game, manager->activeGames())
		addGame(game);

	connect(manager, SIGNAL(gameStarted(ChessGame*)),
		this, SLOT(addGame(ChessGame*)));
	connect(manager, SIGNAL(gameDestroyed(ChessGame*)),
		this, SLOT(removeGame(ChessGame*)));

	m_timer->setSingleShot(true);
	m_timer->setInterval(2000);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(cleanupWidgets()));
}

void GameWall::addGame(ChessGame* game)
{
	Q_ASSERT(game != nullptr);

	QWidget* widget = new QWidget(this);

	ChessClock* clock[2] = { new ChessClock(), new ChessClock() };
	QHBoxLayout* clockLayout = new QHBoxLayout();
	for (int i = 0; i < 2; i++)
	{
		clock[i] = new ChessClock();
		clockLayout->addWidget(clock[i]);

		Chess::Side side = Chess::Side::Type(i);
		clock[i]->setPlayerName(side.toString());
	}
	clockLayout->insertSpacing(1, 20);

	BoardScene* scene = new BoardScene();
	BoardView* view = new BoardView(scene);

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addLayout(clockLayout);
	mainLayout->addWidget(view);
	mainLayout->setContentsMargins(0, 0, 0, 0);

	widget->setLayout(mainLayout);
	layout()->addWidget(widget);

	game->lockThread();
	connect(game, SIGNAL(fenChanged(QString)),
		scene, SLOT(setFenString(QString)));
	connect(game, SIGNAL(moveMade(Chess::GenericMove, QString, QString)),
		scene, SLOT(makeMove(Chess::GenericMove)));
	connect(game, SIGNAL(humanEnabled(bool)),
		view, SLOT(setEnabled(bool)));

	for (int i = 0; i < 2; i++)
	{
		ChessPlayer* player(game->player(Chess::Side::Type(i)));

		if (player->isHuman())
			connect(scene, SIGNAL(humanMove(Chess::GenericMove, Chess::Side)),
				player, SLOT(onHumanMove(Chess::GenericMove, Chess::Side)));

		clock[i]->setPlayerName(player->name());
		connect(player, SIGNAL(nameChanged(QString)),
			clock[i], SLOT(setPlayerName(QString)));

		clock[i]->setInfiniteTime(player->timeControl()->isInfinite());

		if (player->state() == ChessPlayer::Thinking)
			clock[i]->start(player->timeControl()->activeTimeLeft());
		else
			clock[i]->setTime(player->timeControl()->timeLeft());

		connect(player, SIGNAL(startedThinking(int)),
			clock[i], SLOT(start(int)));
		connect(player, SIGNAL(stoppedThinking()),
			clock[i], SLOT(stop()));
	}

	scene->setBoard(game->pgn()->createBoard());
	scene->populate();

	foreach (const Chess::Move& move, game->moves())
		scene->makeMove(move);

	game->unlockThread();

	view->setEnabled(!game->isFinished() &&
			 game->playerToMove()->isHuman());
	m_games[game] = widget;

	cleanupWidgets();
}

void GameWall::removeGame(ChessGame* game)
{
	Q_ASSERT(m_games.contains(game));
	m_gamesToRemove.append(m_games.take(game));

	if (!m_timer->isActive())
		m_timer->start();
}

void GameWall::cleanupWidgets()
{
	m_timer->stop();
	if (m_gamesToRemove.isEmpty())
		return;

	qDeleteAll(m_gamesToRemove);
	m_gamesToRemove.clear();

	if (m_games.isEmpty())
		close();
}
