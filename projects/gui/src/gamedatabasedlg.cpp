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
#include <QDebug>

#include <pgngame.h>
#include <pgngameentry.h>
#include <board/board.h>

#include "pgndatabasemodel.h"
#include "pgngameentrymodel.h"
#include "cutechessapp.h"
#include "gamedatabasemanager.h"
#include "chessboardview.h"
#include "chessboardmodel.h"
#include "pgndatabase.h"

GameDatabaseDialog::GameDatabaseDialog()
	: QDialog(0, Qt::Window),
	  m_chessboardView(0),
	  m_chessboardModel(0),
	  m_chessboard(0),
	  m_pgnDatabaseModel(0),
	  m_pgnGameEntryModel(0),
	  m_selectedDatabase(0)
{
	setupUi(this);

	m_pgnDatabaseModel = new PgnDatabaseModel(
		CuteChessApplication::instance()->gameDatabaseManager(), this);

	m_pgnGameEntryModel = new PgnGameEntryModel(this);

	m_databasesListView->setModel(m_pgnDatabaseModel);
	m_databasesListView->setAlternatingRowColors(true);
	m_databasesListView->setUniformRowHeights(true);

	m_gamesListView->setModel(m_pgnGameEntryModel);
	m_gamesListView->setAlternatingRowColors(true);
	m_gamesListView->setUniformRowHeights(true);

	m_chessboardModel = new ChessboardModel(this);
	m_chessboardView = new ChessboardView(this);
	m_chessboardView->setModel(m_chessboardModel);

	QVBoxLayout* chessboardViewLayout = new QVBoxLayout();
	chessboardViewLayout->addWidget(m_chessboardView);

	m_chessboardParentWidget->setLayout(chessboardViewLayout);

	m_nextMoveButton->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
	m_previousMoveButton->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));

	connect(m_nextMoveButton, SIGNAL(clicked(bool)), this,
		SLOT(viewNextMove()));
	connect(m_previousMoveButton, SIGNAL(clicked(bool)), this,
		SLOT(viewPreviousMove()));

	connect(m_databasesListView->selectionModel(),
		SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
		this, SLOT(databaseSelectionChanged(const QModelIndex&, const QModelIndex&)));

	connect(m_gamesListView->selectionModel(),
		SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
		this, SLOT(gameSelectionChanged(const QModelIndex&, const QModelIndex&)));

}

GameDatabaseDialog::~GameDatabaseDialog()
{
	delete m_chessboard;
	m_chessboard = 0;
}

void GameDatabaseDialog::databaseSelectionChanged(const QModelIndex& current,
                                                  const QModelIndex& previous)
{
	Q_UNUSED(previous);

	m_selectedDatabase =
		CuteChessApplication::instance()->gameDatabaseManager()->databases().at(current.row());

	m_pgnGameEntryModel->setEntries(m_selectedDatabase->entries());
}

void GameDatabaseDialog::gameSelectionChanged(const QModelIndex& current,
                                              const QModelIndex& previous)
{
	Q_UNUSED(previous);

	const PgnGameEntry entry = m_selectedDatabase->entries().at(current.row());

	m_whiteLabel->setText(entry.white());
	m_blackLabel->setText(entry.black());
	m_siteLabel->setText(entry.site());
	m_eventLabel->setText(entry.event());
	m_resultLabel->setText(entry.result().toShortString());

	PgnGame game;
	if (!m_selectedDatabase->game(entry, &game))
	{
		qDebug() << "PGN database read failed";
		return;
	}

	Chess::Board* board = game.createBoard();
	m_chessboardModel->setBoard(board);
	m_moveIndex = 0;
	m_moves = game.moves();

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
