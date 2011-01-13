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

#include "gamedatabasedlg.h"

#include <QVBoxLayout>

#include "gamedatabasemodel.h"
#include "cutechessapp.h"
#include "gamedatabasemanager.h"
#include "chessboardview.h"
#include "chessboardmodel.h"
#include "treeviewitem.h"
#include "pgndatabase.h"
#include <pgngame.h>
#include <pgngameentry.h>
#include <chessgame.h>
#include <board/boardfactory.h>
#include <timecontrol.h>
#include <chessplayer.h>
#include <humanplayer.h>
#include <QDebug>

GameDatabaseDialog::GameDatabaseDialog()
	: QDialog(0, Qt::Window),
	  m_chessboardView(0),
	  m_chessboardModel(0),
	  m_chessboard(0)
{
	setupUi(this);

	m_gameDatabaseModel = new GameDatabaseModel(
		CuteChessApplication::instance()->gameDatabaseManager(), this);

	m_gameDatabaseView->setModel(m_gameDatabaseModel);
	m_gameDatabaseView->setAlternatingRowColors(true);
	m_gameDatabaseView->setUniformRowHeights(true);

	m_chessboardModel = new ChessboardModel(this);
	m_chessboardView = new ChessboardView(this);
	m_chessboardView->setModel(m_chessboardModel);

	QVBoxLayout* chessboardViewLayout = new QVBoxLayout();
	chessboardViewLayout->addWidget(m_chessboardView);

	m_chessboardParentWidget->setLayout(chessboardViewLayout);

	m_nextMoveButton->setIcon(QIcon(":/nextmove_small.png"));
	m_previousMoveButton->setIcon(QIcon(":/prevmove_small.png"));

	connect(m_nextMoveButton, SIGNAL(clicked(bool)), this,
		SLOT(viewNextMove()));
	connect(m_previousMoveButton, SIGNAL(clicked(bool)), this,
		SLOT(viewPreviousMove()));

	connect(m_gameDatabaseView->selectionModel(),
		SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
		this, SLOT(selectionChanged(const QModelIndex&, const QModelIndex&)));
}

GameDatabaseDialog::~GameDatabaseDialog()
{
	delete m_chessboard;
	m_chessboard = 0;
}

void GameDatabaseDialog::selectionChanged(const QModelIndex& current,
                                          const QModelIndex& previous)
{
	Q_UNUSED(previous);

	const TreeViewItem* selectedItem =
		static_cast<TreeViewItem*>(current.internalPointer());
	Q_ASSERT(selectedItem);

	// selectedItem is database?
	if (selectedItem->parent() == m_gameDatabaseModel->root())
		return;

	const PgnGameEntry* selectedGameEntry =
		static_cast<PgnGameEntry*>(selectedItem->data());
	Q_ASSERT(selectedGameEntry);

	PgnDatabase* sourceDatabase =
		static_cast<PgnDatabase*>(selectedItem->parent()->data());
	Q_ASSERT(sourceDatabase);

	m_whiteLabel->setText(selectedGameEntry->white());
	m_blackLabel->setText(selectedGameEntry->black());
	m_siteLabel->setText(selectedGameEntry->site());
	m_eventLabel->setText(selectedGameEntry->event());
	m_resultLabel->setText(selectedGameEntry->result().toShortString());

	PgnGame pgnGame;
	if (!sourceDatabase->game(selectedGameEntry, &pgnGame))
	{
		qDebug() << "PGN database read failed";
		return;
	}

	Chess::Board* board = pgnGame.createBoard();
	m_chessboardModel->setBoard(board);
	m_moveIndex = 0;
	m_moves = pgnGame.moves();

	delete m_chessboard;
	m_chessboard = board;

	m_previousMoveButton->setEnabled(false);
	m_nextMoveButton->setEnabled(!m_moves.isEmpty());
}

void GameDatabaseDialog::viewNextMove()
{
	Chess::GenericMove gmove = m_moves[m_moveIndex++].move;
	Chess::Move move = m_chessboard->moveFromGenericMove(gmove);
	m_chessboard->makeMove(move, true);

	m_previousMoveButton->setEnabled(true);
	if (m_moveIndex >= m_moves.count())
		m_nextMoveButton->setEnabled(false);
}

void GameDatabaseDialog::viewPreviousMove()
{
	m_moveIndex--;

	if (m_moveIndex == 0)
		m_previousMoveButton->setEnabled(false);

	m_chessboard->undoMove();
	m_chessboardModel->boardReset();

	m_nextMoveButton->setEnabled(true);
}
