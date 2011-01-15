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
#include <QSortFilterProxyModel>

#include <pgngame.h>
#include <pgngameentry.h>

#include "pgndatabasemodel.h"
#include "pgngameentrymodel.h"
#include "cutechessapp.h"
#include "gamedatabasemanager.h"
#include "boardview/boardview.h"
#include "boardview/boardscene.h"
#include "pgndatabase.h"

GameDatabaseDialog::GameDatabaseDialog()
	: QDialog(0, Qt::Window),
	  m_boardView(0),
	  m_boardScene(0),
	  m_pgnDatabaseModel(0),
	  m_pgnGameEntryModel(0),
	  m_selectedDatabase(0)
{
	setupUi(this);

	m_pgnDatabaseModel = new PgnDatabaseModel(
		CuteChessApplication::instance()->gameDatabaseManager(), this);

	// Setup a filtered model
	m_pgnGameEntryModel = new PgnGameEntryModel(this);
	m_filteredModel = new QSortFilterProxyModel(this);
	m_filteredModel->setSourceModel(m_pgnGameEntryModel);
	m_filteredModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

	m_databasesListView->setModel(m_pgnDatabaseModel);
	m_databasesListView->setAlternatingRowColors(true);
	m_databasesListView->setUniformRowHeights(true);

	m_gamesListView->setModel(m_filteredModel);
	m_gamesListView->setAlternatingRowColors(true);
	m_gamesListView->setUniformRowHeights(true);

	m_boardScene = new BoardScene(this);
	m_boardView = new BoardView(m_boardScene, this);
	m_boardView->setEnabled(false);

	QVBoxLayout* chessboardViewLayout = new QVBoxLayout();
	chessboardViewLayout->addWidget(m_boardView);

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

	connect(m_searchEdit, SIGNAL(textChanged(const QString&)),
		this, SLOT(updateSearch(const QString&)));

	m_searchTimer.setSingleShot(true);
	connect(&m_searchTimer, SIGNAL(timeout()), this, SLOT(onSearchTimeout()));
}

GameDatabaseDialog::~GameDatabaseDialog()
{
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

	const QModelIndex selected = m_filteredModel->mapToSource(current);

	if (!selected.isValid())
		return;

	const PgnGameEntry entry = m_selectedDatabase->entries().at(selected.row());

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

	m_boardScene->setBoard(game.createBoard());
	m_boardScene->populate();
	m_moveIndex = 0;
	m_moves = game.moves();

	m_previousMoveButton->setEnabled(false);
	m_nextMoveButton->setEnabled(!m_moves.isEmpty());
}

void GameDatabaseDialog::viewNextMove()
{
	m_boardScene->makeMove(m_moves[m_moveIndex++].move);

	m_previousMoveButton->setEnabled(true);
	if (m_moveIndex >= m_moves.count())
		m_nextMoveButton->setEnabled(false);
}

void GameDatabaseDialog::viewPreviousMove()
{
	m_moveIndex--;

	if (m_moveIndex == 0)
		m_previousMoveButton->setEnabled(false);

	m_boardScene->undoMove();

	m_nextMoveButton->setEnabled(true);
}

void GameDatabaseDialog::updateSearch(const QString& terms)
{
	m_clearBtn->setEnabled(!terms.isEmpty());
	m_searchTerms = terms;

	if (m_searchTerms.isEmpty())
		m_filteredModel->setFilterWildcard(terms);
	else
		m_searchTimer.start(300);
}

void GameDatabaseDialog::onSearchTimeout()
{
	m_filteredModel->setFilterWildcard(m_searchTerms);
}
