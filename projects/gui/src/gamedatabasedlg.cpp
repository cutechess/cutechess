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
#include "ui_gamedatabasedlg.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QMessageBox>
#include <QtAlgorithms>
#include <QModelIndex>

#include <pgngame.h>
#include <pgngameentry.h>

#include "pgndatabasemodel.h"
#include "pgngameentrymodel.h"
#include "cutechessapp.h"
#include "gamedatabasemanager.h"
#include "boardview/boardview.h"
#include "boardview/boardscene.h"
#include "pgndatabase.h"
#include "gamedatabasesearchdlg.h"

GameDatabaseDialog::GameDatabaseDialog(QWidget* parent)
	: QDialog(parent, Qt::Window),
	  m_boardView(0),
	  m_boardScene(0),
	  m_pgnDatabaseModel(0),
	  m_pgnGameEntryModel(0),
	  ui(new Ui::GameDatabaseDialog)
{
	ui->setupUi(this);

	m_pgnDatabaseModel = new PgnDatabaseModel(
		CuteChessApplication::instance()->gameDatabaseManager(), this);

	// Setup a filtered model
	m_pgnGameEntryModel = new PgnGameEntryModel(this);

	ui->m_databasesListView->setModel(m_pgnDatabaseModel);
	ui->m_databasesListView->setAlternatingRowColors(true);
	ui->m_databasesListView->setUniformRowHeights(true);

	ui->m_gamesListView->setModel(m_pgnGameEntryModel);
	ui->m_gamesListView->setAlternatingRowColors(true);
	ui->m_gamesListView->setUniformRowHeights(true);

	m_boardScene = new BoardScene(this);
	m_boardView = new BoardView(m_boardScene, this);
	m_boardView->setEnabled(false);

	QVBoxLayout* chessboardViewLayout = new QVBoxLayout();
	chessboardViewLayout->addWidget(m_boardView);

	ui->m_chessboardParentWidget->setLayout(chessboardViewLayout);

	ui->m_nextMoveButton->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
	ui->m_previousMoveButton->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
	ui->m_skipToFirstMoveButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
	ui->m_skipToLastMoveButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));

	connect(ui->m_nextMoveButton, SIGNAL(clicked(bool)), this,
		SLOT(viewNextMove()));
	connect(ui->m_previousMoveButton, SIGNAL(clicked(bool)), this,
		SLOT(viewPreviousMove()));
	connect(ui->m_skipToFirstMoveButton, SIGNAL(clicked(bool)), this,
		SLOT(viewFirstMove()));
	connect(ui->m_skipToLastMoveButton, SIGNAL(clicked(bool)), this,
		SLOT(viewLastMove()));

	connect(ui->m_databasesListView->selectionModel(),
		SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		this, SLOT(databaseSelectionChanged(const QItemSelection&, const QItemSelection&)));

	connect(ui->m_gamesListView->selectionModel(),
		SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
		this, SLOT(gameSelectionChanged(const QModelIndex&, const QModelIndex&)));

	connect(ui->m_searchEdit, SIGNAL(textEdited(const QString&)),
		this, SLOT(updateSearch(const QString&)));

	connect(ui->m_clearBtn, SIGNAL(clicked()),
		this, SLOT(updateSearch()));

	connect(ui->m_advancedSearchBtn, SIGNAL(clicked()),
		this, SLOT(onAdvancedSearch()));

	m_searchTimer.setSingleShot(true);
	connect(&m_searchTimer, SIGNAL(timeout()), this, SLOT(onSearchTimeout()));
}

GameDatabaseDialog::~GameDatabaseDialog()
{
	delete ui;
}

void GameDatabaseDialog::databaseSelectionChanged(const QItemSelection& selected,
                                                  const QItemSelection& deselected)
{
	m_selectedDatabases.merge(selected, QItemSelectionModel::Select);
	m_selectedDatabases.merge(deselected, QItemSelectionModel::Deselect);

	if (m_selectedDatabases.isEmpty())
	{
		m_pgnGameEntryModel->setEntries(QList<const PgnGameEntry*>());
		return;
	}

	QList<const PgnGameEntry*> entries;
	foreach (const QModelIndex& index, m_selectedDatabases.indexes())
	{
		entries.append(
		    CuteChessApplication::instance()->gameDatabaseManager()->databases().at(index.row())->entries());
	}

	m_pgnGameEntryModel->setEntries(entries);
	ui->m_advancedSearchBtn->setEnabled(true);
}

void GameDatabaseDialog::gameSelectionChanged(const QModelIndex& current,
                                              const QModelIndex& previous)
{
	Q_UNUSED(previous);

	if (!current.isValid())
		return;

	int databaseIndex;
	if ((databaseIndex = databaseIndexFromGame(current.row())) == -1)
		return;

	PgnDatabase* selectedDatabase =
	    CuteChessApplication::instance()->gameDatabaseManager()->databases().at(databaseIndex);

	PgnGame game;
	PgnDatabase::PgnDatabaseError error;
	const PgnGameEntry* entry = m_pgnGameEntryModel->entryAt(current.row());

	if ((error = selectedDatabase->game(entry, &game)) !=
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
			msgBox.setInformativeText(QString("Remove %1 from the list of databases?").arg(selectedDatabase->displayName()));
			msgBox.setDefaultButton(removeDbButton);
			msgBox.setIcon(QMessageBox::Warning);

			msgBox.exec();

			if (msgBox.clickedButton() == removeDbButton)
				CuteChessApplication::instance()->gameDatabaseManager()->removeDatabase(databaseIndex);
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
				CuteChessApplication::instance()->gameDatabaseManager()->importDatabaseAgain(databaseIndex);
		}
	}

	ui->m_whiteLabel->setText(game.tagValue("White"));
	ui->m_blackLabel->setText(game.tagValue("Black"));
	ui->m_siteLabel->setText(game.tagValue("Site"));
	ui->m_eventLabel->setText(game.tagValue("Event"));
	ui->m_resultLabel->setText(game.tagValue("Result"));

	m_boardScene->setBoard(game.createBoard());
	m_boardScene->populate();
	m_moveIndex = 0;
	m_moves = game.moves();

	ui->m_previousMoveButton->setEnabled(false);
	ui->m_nextMoveButton->setEnabled(!m_moves.isEmpty());
	ui->m_skipToFirstMoveButton->setEnabled(false);
	ui->m_skipToLastMoveButton->setEnabled(!m_moves.isEmpty());
}

void GameDatabaseDialog::viewNextMove()
{
	m_boardScene->makeMove(m_moves.at(m_moveIndex++).move);

	ui->m_previousMoveButton->setEnabled(true);
	ui->m_skipToFirstMoveButton->setEnabled(true);

	if (m_moveIndex >= m_moves.count())
	{
		ui->m_nextMoveButton->setEnabled(false);
		ui->m_skipToLastMoveButton->setEnabled(false);
	}
}

void GameDatabaseDialog::viewPreviousMove()
{
	m_moveIndex--;

	if (m_moveIndex == 0)
	{
		ui->m_previousMoveButton->setEnabled(false);
		ui->m_skipToFirstMoveButton->setEnabled(false);
	}

	m_boardScene->undoMove();

	ui->m_nextMoveButton->setEnabled(true);
	ui->m_skipToLastMoveButton->setEnabled(true);
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
	ui->m_clearBtn->setEnabled(!terms.isEmpty());
	m_searchTerms = terms;
	m_searchTimer.start(500);
}

void GameDatabaseDialog::onSearchTimeout()
{
	m_pgnGameEntryModel->setFilter(m_searchTerms);
}

void GameDatabaseDialog::onAdvancedSearch()
{
	GameDatabaseSearchDialog dlg;
	if (dlg.exec() != QDialog::Accepted)
		return;

	ui->m_searchEdit->clear();
	m_pgnGameEntryModel->setFilter(dlg.filter());
	ui->m_clearBtn->setEnabled(true);
}

int GameDatabaseDialog::databaseIndexFromGame(int game)
{
	QList<int> sorted;
	int databaseIndex;

	foreach (const QModelIndex& index, m_selectedDatabases.indexes())
		sorted << index.row();

	if (sorted.count() == 0)
		return -1;

	qSort(sorted);

	forever
	{
		databaseIndex = sorted.takeFirst();
		game -=
		    CuteChessApplication::instance()->gameDatabaseManager()->databases().at(databaseIndex)->entries().count();

		if (game < 0)
			break;
	}

	return databaseIndex;
}
