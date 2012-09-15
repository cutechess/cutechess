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

#include "gameviewer.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAction>
#include <QIcon>
#include <QToolBar>
#include <QSlider>
#include <pgngame.h>
#include <chessgame.h>
#include <chessplayer.h>
#include "boardview/boardscene.h"
#include "boardview/boardview.h"

GameViewer::GameViewer(Qt::Orientation orientation, QWidget* parent)
	: QWidget(parent),
	  m_toolBar(new QToolBar),
	  m_moveNumberSlider(new QSlider(Qt::Horizontal)),
	  m_moveIndex(0)
{
	m_boardScene = new BoardScene(this);
	m_boardView = new BoardView(m_boardScene, this);
	m_boardView->setEnabled(false);

	m_toolBar->setFloatable(false);
	m_toolBar->setMovable(false);
	m_toolBar->setEnabled(false);

	m_viewFirstMoveAction = m_toolBar->addAction(
		QIcon(":/icons/toolbutton/first_16x16"),
		tr("Skip to first move"),
		this, SLOT(viewFirstMove()));
	m_viewPreviousMoveAction = m_toolBar->addAction(
		QIcon(":/icons/toolbutton/previous_16x16"),
		tr("Previous move"),
		this, SLOT(viewPreviousMove()));
	m_viewNextMoveAction = m_toolBar->addAction(
		QIcon(":/icons/toolbutton/next_16x16"),
		tr("Next move"),
		this, SLOT(viewNextMove()));
	m_viewLastMoveAction = m_toolBar->addAction(
		QIcon(":/icons/toolbutton/last_16x16"),
		tr("Skip to last move"),
		this, SLOT(viewLastMove()));

	connect(m_moveNumberSlider, SIGNAL(valueChanged(int)),
		this, SLOT(viewPosition(int)));

	QVBoxLayout* layout = new QVBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(m_boardView);

	if (orientation == Qt::Horizontal)
	{
		m_toolBar->addWidget(m_moveNumberSlider);
		layout->addWidget(m_toolBar);
	}
	else
	{
		m_moveNumberSlider->setEnabled(false);
		layout->addWidget(m_moveNumberSlider);

		QHBoxLayout* toolbarLayout = new QHBoxLayout();
		toolbarLayout->setContentsMargins(0, 0, 0, 0);
		toolbarLayout->addStretch();
		toolbarLayout->addWidget(m_toolBar);
		toolbarLayout->addStretch();

		layout->addLayout(toolbarLayout);
	}

	setLayout(layout);
}

void GameViewer::setGame(ChessGame* game)
{
	Q_ASSERT(game != 0);

	setGame(game->pgn());
	m_game = game;

	connect(m_game, SIGNAL(fenChanged(QString)),
		this, SLOT(onFenChanged(QString)));
	connect(m_game, SIGNAL(moveMade(Chess::GenericMove, QString, QString)),
		this, SLOT(onMoveMade(Chess::GenericMove)));
	connect(m_game, SIGNAL(humanEnabled(bool)),
		m_boardView, SLOT(setEnabled(bool)));

	viewLastMove();

	for (int i = 0; i < 2; i++)
	{
		ChessPlayer* player(m_game->player(Chess::Side::Type(i)));
		if (player->isHuman())
			connect(m_boardScene, SIGNAL(humanMove(Chess::GenericMove, Chess::Side)),
				player, SLOT(onHumanMove(Chess::GenericMove, Chess::Side)));
	}

	m_boardView->setEnabled(!m_game->isFinished() &&
				m_game->playerToMove()->isHuman());
}

void GameViewer::setGame(const PgnGame* pgn)
{
	Q_ASSERT(pgn != 0);

	disconnectGame();

	m_boardScene->setBoard(pgn->createBoard());
	m_boardScene->populate();
	m_moveIndex = 0;

	m_moves.clear();
	foreach (const PgnGame::MoveData& md, pgn->moves())
		m_moves.append(md.move);

	m_toolBar->setEnabled(true);
	m_viewFirstMoveAction->setEnabled(false);
	m_viewPreviousMoveAction->setEnabled(false);
	m_viewNextMoveAction->setEnabled(!m_moves.isEmpty());
	m_viewLastMoveAction->setEnabled(!m_moves.isEmpty());

	m_moveNumberSlider->setEnabled(!m_moves.isEmpty());
	m_moveNumberSlider->setMaximum(m_moves.count());
}

void GameViewer::disconnectGame()
{
	m_boardView->setEnabled(false);
	if (m_game.isNull())
		return;

	disconnect(m_game, 0, m_boardScene, 0);
	disconnect(m_game, 0, m_boardView, 0);
	disconnect(m_game, 0, this, 0);

	for (int i = 0; i < 2; i++)
	{
		ChessPlayer* player(m_game->player(Chess::Side::Type(i)));
		if (player != 0)
			disconnect(m_boardScene, 0, player, 0);
	}

	m_game = 0;
}

void GameViewer::viewFirstMove()
{
	while (m_moveIndex > 0)
		viewPreviousMove();
}

void GameViewer::viewPreviousMove()
{
	m_moveIndex--;

	if (m_moveIndex == 0)
	{
		m_viewPreviousMoveAction->setEnabled(false);
		m_viewFirstMoveAction->setEnabled(false);
	}

	m_boardScene->undoMove();

	m_viewNextMoveAction->setEnabled(true);
	m_viewLastMoveAction->setEnabled(true);
	m_boardView->setEnabled(false);

	m_moveNumberSlider->setSliderPosition(m_moveIndex);
}

void GameViewer::viewNextMove()
{
	m_boardScene->makeMove(m_moves.at(m_moveIndex++));

	m_viewPreviousMoveAction->setEnabled(true);
	m_viewFirstMoveAction->setEnabled(true);

	if (m_moveIndex >= m_moves.count())
	{
		m_viewNextMoveAction->setEnabled(false);
		m_viewLastMoveAction->setEnabled(false);

		if (!m_game.isNull()
		&&  !m_game->isFinished() && m_game->playerToMove()->isHuman())
			m_boardView->setEnabled(true);
	}

	m_moveNumberSlider->setSliderPosition(m_moveIndex);
}

void GameViewer::viewLastMove()
{
	while (m_moveIndex < m_moves.count())
		viewNextMove();
}

void GameViewer::viewPosition(int index)
{
	if (m_moves.isEmpty())
		return;

	while (index < m_moveIndex)
		viewPreviousMove();
	while (index > m_moveIndex)
		viewNextMove();
}

void GameViewer::viewMove(int index)
{
	Q_ASSERT(!m_moves.isEmpty());

	if (index < m_moveIndex)
	{
		// We backtrack one move too far and then make one
		// move forward to highlight the correct move
		while (index < m_moveIndex)
			viewPreviousMove();
		viewNextMove();
	}
	else
	{
		while (index + 1 > m_moveIndex)
			viewNextMove();
	}
}

void GameViewer::onFenChanged(const QString& fen)
{
	m_moves.clear();
	m_moveIndex = 0;

	m_toolBar->setEnabled(false);
	m_moveNumberSlider->setEnabled(false);
	m_moveNumberSlider->setMaximum(0);

	m_boardScene->setFenString(fen);
}

void GameViewer::onMoveMade(const Chess::GenericMove& move)
{
	m_moves.append(move);

	m_toolBar->setEnabled(true);
	m_moveNumberSlider->setEnabled(true);
	m_moveNumberSlider->setMaximum(m_moves.count());

	if (m_moveIndex == m_moves.count() - 1)
		viewNextMove();
}
