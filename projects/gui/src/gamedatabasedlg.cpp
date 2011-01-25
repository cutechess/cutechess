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
#include <QMessageBox>

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

	m_databasesListView->setModel(m_pgnDatabaseModel);
	m_databasesListView->setAlternatingRowColors(true);
	m_databasesListView->setUniformRowHeights(true);

	m_gamesListView->setModel(m_pgnGameEntryModel);
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
	m_skipToFirstMoveButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
	m_skipToLastMoveButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));

	connect(m_nextMoveButton, SIGNAL(clicked(bool)), this,
		SLOT(viewNextMove()));
	connect(m_previousMoveButton, SIGNAL(clicked(bool)), this,
		SLOT(viewPreviousMove()));
	connect(m_skipToFirstMoveButton, SIGNAL(clicked(bool)), this,
		SLOT(viewFirstMove()));
	connect(m_skipToLastMoveButton, SIGNAL(clicked(bool)), this,
		SLOT(viewLastMove()));

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

	if (!current.isValid())
	{
		QList<PgnGameEntry> emptyList;
		m_pgnGameEntryModel->setEntries(emptyList);
		return;
	}

	m_selectedDatabase =
		CuteChessApplication::instance()->gameDatabaseManager()->databases().at(current.row());

	m_pgnGameEntryModel->setEntries(m_selectedDatabase->entries());
}

void GameDatabaseDialog::gameSelectionChanged(const QModelIndex& current,
                                              const QModelIndex& previous)
{
	Q_UNUSED(previous);

	const PgnGameEntry entry = m_pgnGameEntryModel->entryAt(current.row());

	m_whiteLabel->setText(entry.white());
	m_blackLabel->setText(entry.black());
	m_siteLabel->setText(entry.site());
	m_eventLabel->setText(entry.event());
	m_resultLabel->setText(entry.result().toShortString());

	PgnGame game;
	PgnDatabase::PgnDatabaseError error;
	if ((error = m_selectedDatabase->game(entry, &game)) !=
		PgnDatabase::NoError)
	{
		if (error == PgnDatabase::DatabaseDoesNotExist)
		{
			// Ask the user if the database should be deleted from the
			// list
			QMessageBox msgBox(this);
			QPushButton* removeDbButton = msgBox.addButton(tr("Remove"),
				QMessageBox::ActionRole);
			msgBox.addButton(QMessageBox::Cancel);

			msgBox.setText("PGN database does not exist.");
			msgBox.setInformativeText(QString("Remove %1 from the list of databases?").arg(m_selectedDatabase->displayName()));
			msgBox.setDefaultButton(removeDbButton);
			msgBox.setIcon(QMessageBox::Warning);

			msgBox.exec();

			if (msgBox.clickedButton() == removeDbButton)
			{
				QItemSelectionModel* selection = m_databasesListView->selectionModel();
				if (selection->hasSelection())
					CuteChessApplication::instance()->gameDatabaseManager()->removeDatabase(selection->currentIndex().row());
			}
		}
		else
		{
			// Ask the user to re-import the database
			QMessageBox msgBox(this);
			QPushButton* importDbButton = msgBox.addButton(tr("Import"),
				QMessageBox::ActionRole);
			msgBox.addButton(QMessageBox::Cancel);

			if (error == PgnDatabase::DatabaseModified)
			{
				msgBox.setText("PGN database has been modified since the last import.");
				msgBox.setInformativeText("The database must be imported again to read it.");
			}
			else
			{
				msgBox.setText("Error occured while trying to read the PGN database.");
				msgBox.setInformativeText("Importing the database again may fix this problem.");
			}

			msgBox.setDefaultButton(importDbButton);
			msgBox.setIcon(QMessageBox::Warning);

			msgBox.exec();

			if (msgBox.clickedButton() == importDbButton)
			{
				QItemSelectionModel* selection = m_databasesListView->selectionModel();
				if (selection->hasSelection())
					CuteChessApplication::instance()->gameDatabaseManager()->importDatabaseAgain(selection->currentIndex().row());
			}
		}
	}

	m_boardScene->setBoard(game.createBoard());
	m_boardScene->populate();
	m_moveIndex = 0;
	m_moves = game.moves();

	m_previousMoveButton->setEnabled(false);
	m_nextMoveButton->setEnabled(!m_moves.isEmpty());
	m_skipToFirstMoveButton->setEnabled(false);
	m_skipToLastMoveButton->setEnabled(!m_moves.isEmpty());
}

void GameDatabaseDialog::viewNextMove()
{
	m_boardScene->makeMove(m_moves[m_moveIndex++].move);

	m_previousMoveButton->setEnabled(true);
	m_skipToFirstMoveButton->setEnabled(true);

	if (m_moveIndex >= m_moves.count())
	{
		m_nextMoveButton->setEnabled(false);
		m_skipToLastMoveButton->setEnabled(false);
	}
}

void GameDatabaseDialog::viewPreviousMove()
{
	m_moveIndex--;

	if (m_moveIndex == 0)
	{
		m_previousMoveButton->setEnabled(false);
		m_skipToFirstMoveButton->setEnabled(false);
	}

	m_boardScene->undoMove();

	m_nextMoveButton->setEnabled(true);
	m_skipToLastMoveButton->setEnabled(true);
}

void GameDatabaseDialog::viewFirstMove()
{
	while (m_moveIndex > 0)
		viewPreviousMove();
}

void GameDatabaseDialog::viewLastMove()
{
	while (m_moveIndex < m_moves.count())
		viewNextMove();
}

void GameDatabaseDialog::updateSearch(const QString& terms)
{
	m_clearBtn->setEnabled(!terms.isEmpty());
	m_searchTerms = terms;
	m_searchTimer.start(300);
}

void GameDatabaseDialog::onSearchTimeout()
{
	m_pgnGameEntryModel->setFilterWildcard(m_searchTerms);
}
