/*
    This file is part of Cute Chess.
    Copyright (C) 2008-2018 Cute Chess authors

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
#include <QMessageBox>
#include <QtAlgorithms>
#include <QModelIndex>
#include <QFileDialog>
#include <QInputDialog>

#include <pgnstream.h>
#include <pgngame.h>
#include <pgngameentry.h>
#include <polyglotbook.h>

#include "pgndatabasemodel.h"
#include "pgngameentrymodel.h"
#include "gamedatabasemanager.h"
#include "boardview/boardview.h"
#include "boardview/boardscene.h"
#include "gameviewer.h"
#include "pgndatabase.h"
#include "gamedatabasesearchdlg.h"
#include "threadedtask.h"

#ifdef QT_DEBUG
#include <modeltest.h>
#endif

class PgnGameIterator
{
	public:
		PgnGameIterator(const GameDatabaseDialog* dlg);

		int count() const;
		bool hasNext() const;
		PgnGame next(bool* ok, int depth = INT_MAX - 1);

	private:
		const GameDatabaseDialog* m_dlg;
		int m_dbIndex;
		int m_gameIndex;
		int m_gameCount;
		QFile m_file;
		PgnStream m_in;
};

PgnGameIterator::PgnGameIterator(const GameDatabaseDialog* dlg)
	: m_dlg(dlg),
	  m_dbIndex(-1),
	  m_gameIndex(0),
	  m_gameCount(dlg->m_pgnGameEntryModel->entryCount())
{
}

int PgnGameIterator::count() const
{
	return m_gameCount;
}

bool PgnGameIterator::hasNext() const
{
	return m_gameIndex < m_gameCount;
}

PgnGame PgnGameIterator::next(bool* ok, int depth)
{
	Q_ASSERT(hasNext());

	int newDbIndex = m_dlg->databaseIndexFromGame(m_gameIndex);
	Q_ASSERT(newDbIndex != -1);

	if (newDbIndex != m_dbIndex)
	{
		m_dbIndex = newDbIndex;
		m_file.close();

		const PgnDatabase* db = m_dlg->m_dbManager->databases().at(m_dbIndex);
		if (db->status() == PgnDatabase::Ok)
		{
			m_file.setFileName(db->fileName());
			if (m_file.open(QIODevice::ReadOnly | QIODevice::Text))
				m_in.setDevice(&m_file);
			else
				m_in.setDevice(nullptr);
		}
	}

	PgnGame game;
	if (!m_file.isOpen())
	{
		*ok = false;
		m_gameIndex++;
		return game;
	}

	const PgnGameEntry* entry = m_dlg->m_pgnGameEntryModel->entryAt(m_gameIndex++);
	*ok = m_in.seek(entry->pos(), entry->lineNumber()) && game.read(m_in, depth);

	return game;
}


class BookExportTask : public ThreadedTask
{
	Q_OBJECT

	public:
		BookExportTask(PgnGameIterator* it,
			       QFile* file,
			       int maxDepth,
			       QWidget* parent);

	protected:
		virtual void run();

	private:
		PgnGameIterator* m_it;
		QFile* m_file;
		int m_depth;
};

BookExportTask::BookExportTask(PgnGameIterator* it,
			       QFile* file,
			       int maxDepth,
			       QWidget* parent)
	: ThreadedTask(tr("Export Opening Book"),
		       tr("Parsing %1 PGN games").arg(it->count()),
		       0, it->count(),
		       parent),
	  m_it(it),
	  m_file(file),
	  m_depth(maxDepth)
{
	m_file->moveToThread(this);
}

void BookExportTask::run()
{
	QDataStream out(m_file);
	PolyglotBook openingBook;

	int i = 0;
	while (m_it->hasNext())
	{
		bool ok;
		PgnGame game(m_it->next(&ok, m_depth));

		if (ok)
		{
			openingBook.import(game, m_depth);
			if (++i % 512 == 0)
			{
				if (cancelRequested())
					break;
				emit progressValueChanged(i);
			}
		}
	}

	// Write the already imported games to the book
	// even if cancel was requested.
	emit statusMessageChanged(tr("Writing opening book to disk"));
	out << &openingBook;

	delete m_it;
	delete m_file;

	emit progressValueChanged(i);
}


class PgnExportTask : public ThreadedTask
{
	Q_OBJECT

	public:
		PgnExportTask(PgnGameIterator* it,
			      QFile* file,
			      QWidget* parent);

	protected:
		virtual void run();

	private:
		PgnGameIterator* m_it;
		QFile* m_file;
};

PgnExportTask::PgnExportTask(PgnGameIterator* it,
			     QFile* file,
			     QWidget* parent)
	: ThreadedTask(tr("Export Games"),
		       tr("Writing %1 games to file").arg(it->count()),
		       0, it->count(),
		       parent),
	  m_it(it),
	  m_file(file)
{
	m_file->moveToThread(this);
}

void PgnExportTask::run()
{
	QTextStream out(m_file);

	int i = 0;
	while (m_it->hasNext())
	{
		bool ok;
		PgnGame game(m_it->next(&ok));

		if (ok)
		{
			out << game;
			if (++i % 512 == 0)
			{
				if (cancelRequested())
					break;
				emit progressValueChanged(i);
			}
		}
	}

	delete m_it;
	delete m_file;

	emit progressValueChanged(i);
}


GameDatabaseDialog::GameDatabaseDialog(GameDatabaseManager* dbManager, QWidget* parent)
	: QDialog(parent, Qt::Window),
	  m_gameViewer(nullptr),
	  m_dbManager(dbManager),
	  m_pgnDatabaseModel(nullptr),
	  m_pgnGameEntryModel(nullptr),
	  ui(new Ui::GameDatabaseDialog)
{
	Q_ASSERT(dbManager != nullptr);
	ui->setupUi(this);

	m_pgnDatabaseModel = new PgnDatabaseModel(m_dbManager, this);
	#ifdef QT_DEBUG
	new ModelTest(m_pgnDatabaseModel, this);
	#endif

	// Setup a filtered model
	m_pgnGameEntryModel = new PgnGameEntryModel(this);
	#ifdef QT_DEBUG
	new ModelTest(m_pgnGameEntryModel, this);
	#endif

	ui->m_databasesListView->setModel(m_pgnDatabaseModel);
	ui->m_databasesListView->setAlternatingRowColors(true);
	ui->m_databasesListView->setUniformRowHeights(true);

	ui->m_gamesListView->setModel(m_pgnGameEntryModel);
	ui->m_gamesListView->setAlternatingRowColors(true);
	ui->m_gamesListView->setUniformRowHeights(true);

	m_gameViewer = new GameViewer(Qt::Horizontal);
	ui->m_viewerLayout->insertWidget(0, m_gameViewer);

	ui->m_splitter->setSizes(QList<int>() << 100 << 500 << 300);

	connect(ui->m_importBtn, &QPushButton::clicked, this, [=]()
	{
		auto dlg = new QFileDialog(this, tr("Import Game"), QString(),
			tr("Portable Game Notation (*.pgn);;All Files (*.*)"));
		connect(dlg, &QFileDialog::fileSelected, m_dbManager, &GameDatabaseManager::importPgnFile);
		dlg->setAttribute(Qt::WA_DeleteOnClose);
		dlg->open();
	});
	connect(ui->m_exportBtn, &QPushButton::clicked, this, [=]()
	{
		auto dlg = new QFileDialog(this, tr("Export game collection"), QString(),
			tr("Portable Game Notation (*.pgn)"));
		connect(dlg, &QFileDialog::fileSelected, this, &GameDatabaseDialog::exportPgn);
		dlg->setAttribute(Qt::WA_DeleteOnClose);
		dlg->setAcceptMode(QFileDialog::AcceptSave);
		dlg->open();
	});
	connect(ui->m_createOpeningBookBtn, SIGNAL(clicked(bool)), this,
		SLOT(createOpeningBook()));

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

	connect(m_pgnGameEntryModel, SIGNAL(modelReset()), this,
		SLOT(updateUi()));
	connect(m_pgnGameEntryModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
		this, SLOT(updateUi()));

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
	const auto deselectedIndexes = deselected.indexes();
	for (const QModelIndex& index : deselectedIndexes)
		m_selectedDatabases.remove(index.row());

	const auto selectedIndexes = selected.indexes();
	for (const QModelIndex& index : selectedIndexes)
		m_selectedDatabases[index.row()] = m_dbManager->databases().at(index.row());

	if (m_selectedDatabases.isEmpty())
	{
		m_pgnGameEntryModel->setEntries(QList<const PgnGameEntry*>());
		return;
	}

	QList<const PgnGameEntry*> entries;
	QMap<int, PgnDatabase*>::const_iterator it;
	for (it = m_selectedDatabases.constBegin(); it != m_selectedDatabases.constEnd(); ++it)
		entries.append(it.value()->entries());

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

	PgnDatabase* selectedDatabase = m_dbManager->databases().at(databaseIndex);

	PgnGame game;
	PgnDatabase::Status status;
	const PgnGameEntry* entry = m_pgnGameEntryModel->entryAt(current.row());

	if ((status = selectedDatabase->game(entry, &game)) != PgnDatabase::Ok)
	{
		if (status == PgnDatabase::DoesNotExist)
		{
			// Ask the user if the database should be deleted from the
			// list
			QMessageBox msgBox(this);
			QPushButton* removeDbButton = msgBox.addButton(tr("Remove"),
				QMessageBox::ActionRole);
			msgBox.addButton(QMessageBox::Cancel);

			msgBox.setText(tr("PGN database does not exist."));
			msgBox.setInformativeText(tr("Remove %1 from the list of databases?").arg(selectedDatabase->displayName()));
			msgBox.setDefaultButton(removeDbButton);
			msgBox.setIcon(QMessageBox::Warning);

			msgBox.exec();

			if (msgBox.clickedButton() == removeDbButton)
				m_dbManager->removeDatabase(databaseIndex);
		}
		else
		{
			// Ask the user to re-import the database
			QMessageBox msgBox(this);
			QPushButton* importDbButton = msgBox.addButton(tr("Import"),
				QMessageBox::ActionRole);
			msgBox.addButton(QMessageBox::Cancel);

			if (status == PgnDatabase::Modified)
			{
				msgBox.setText(tr("PGN database has been modified since the last import."));
				msgBox.setInformativeText(tr("The database must be imported again to read it."));
			}
			else
			{
				msgBox.setText(tr("Error occured while trying to read the PGN database."));
				msgBox.setInformativeText(tr("Importing the database again may fix this problem."));
			}

			msgBox.setDefaultButton(importDbButton);
			msgBox.setIcon(QMessageBox::Warning);

			msgBox.exec();

			if (msgBox.clickedButton() == importDbButton)
				m_dbManager->importDatabaseAgain(databaseIndex);
		}

		return;
	}

	ui->m_whiteLabel->setText(game.tagValue("White"));
	ui->m_blackLabel->setText(game.tagValue("Black"));
	ui->m_siteLabel->setText(game.tagValue("Site"));
	ui->m_eventLabel->setText(game.tagValue("Event"));
	ui->m_resultLabel->setText(game.tagValue("Result"));

	m_gameViewer->setGame(&game);
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

	ui->m_searchEdit->setText(tr("[Advanced search]"));
	ui->m_searchEdit->setEnabled(false);
	m_pgnGameEntryModel->setFilter(dlg.filter());
	ui->m_clearBtn->setEnabled(true);
}

int GameDatabaseDialog::databaseIndexFromGame(int game) const
{
	if (m_selectedDatabases.isEmpty())
		return -1;

	game = m_pgnGameEntryModel->sourceIndex(game);

	QMap<int, PgnDatabase*>::const_iterator it;
	for (it = m_selectedDatabases.constBegin(); it != m_selectedDatabases.constEnd(); ++it)
	{
		game -= it.value()->entries().count();
		if (game < 0)
			return it.key();
	}

	return -1;
}

void GameDatabaseDialog::exportPgn(const QString& fileName)
{
	QFile* file = new QFile(fileName);
	if (!file->open(QIODevice::WriteOnly | QIODevice::Append))
	{
		QMessageBox::critical(this, tr("File Error"),
				      tr("Error while saving file %1\n%2")
				      .arg(fileName, file->errorString()));
		delete file;
		return;
	}

	PgnExportTask* task = new PgnExportTask(new PgnGameIterator(this),
						file, this);
	task->start();
}

void GameDatabaseDialog::createOpeningBook()
{
	bool ok;
	int depth = QInputDialog::getInt(this, tr("Opening depth"),
					 tr("Maximum opening depth (plies):"),
					 20, 1, 1024, 1, &ok);
	if (!ok)
		return;

	const QString fileName = QFileDialog::getSaveFileName(this, tr("Create Opening Book"),
		QString(), tr("Polyglot Book File (*.bin)"));

	if (fileName.isEmpty())
		return;

	QFile* file = new QFile(fileName);
	if (!file->open(QIODevice::WriteOnly))
	{
		QMessageBox::critical(this, tr("File Error"),
				      tr("Error while saving file %1\n%2")
				      .arg(fileName, file->errorString()));
		delete file;
		return;
	}

	BookExportTask* task = new BookExportTask(new PgnGameIterator(this),
						  file, depth, this);
	task->start();
}

void GameDatabaseDialog::updateUi()
{
	bool enable = m_pgnGameEntryModel->rowCount() > 0;
	ui->m_createOpeningBookBtn->setEnabled(enable);
	ui->m_exportBtn->setEnabled(enable);
}

#include "gamedatabasedlg.moc"
