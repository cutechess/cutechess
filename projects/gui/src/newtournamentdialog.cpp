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

#include "newtournamentdialog.h"
#include "ui_newtournamentdlg.h"

#include <QFileDialog>
#include <QSettings>
#include <functional>
#include <algorithm>

#include <board/boardfactory.h>
#include <enginemanager.h>
#include <enginebuilder.h>
#include <timecontrol.h>
#include <tournament.h>
#include <tournamentfactory.h>
#include <openingsuite.h>

#include "engineconfigurationmodel.h"
#include "engineconfigproxymodel.h"
#include "engineconfigurationdlg.h"
#include "timecontroldlg.h"
#include "engineselectiondlg.h"

#ifdef QT_DEBUG
#include <modeltest.h>
#endif

NewTournamentDialog::NewTournamentDialog(EngineManager* engineManager,
					 QWidget *parent)
	: QDialog(parent),
	  m_srcEngineManager(engineManager),
	  ui(new Ui::NewTournamentDialog)
{
	Q_ASSERT(engineManager != nullptr);
	ui->setupUi(this);

	m_srcEnginesModel = new EngineConfigurationModel(engineManager, this);
	#ifdef QT_DEBUG
	new ModelTest(m_srcEnginesModel, this);
	#endif

	m_proxyModel = new EngineConfigurationProxyModel(this);
	m_proxyModel->setSourceModel(m_srcEnginesModel);
	m_proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
	m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
	m_proxyModel->sort(0);
	m_proxyModel->setDynamicSortFilter(true);

	connect(ui->m_gameSettings, SIGNAL(variantChanged(QString)),
		this, SLOT(onVariantChanged(QString)));

	connect(ui->m_addEngineBtn, SIGNAL(clicked()),
		this, SLOT(addEngine()));
	connect(ui->m_removeEngineBtn, SIGNAL(clicked()),
		this, SLOT(removeEngine()));
	connect(ui->m_configureEngineBtn, &QToolButton::clicked, [=]()
	{
		configureEngine(ui->m_playersList->currentIndex());
	});
	connect(ui->m_moveEngineUpBtn, &QToolButton::clicked, [=]()
	{
		moveEngine(-1);
	});
	connect(ui->m_moveEngineDownBtn, &QToolButton::clicked, [=]()
	{
		moveEngine(1);
	});

	connect(ui->m_browsePgnoutBtn, &QPushButton::clicked, this, [=]()
	{
		auto dlg = new QFileDialog(this, tr("Select PGN output file"),
			QString(), tr("Portable Game Notation (*.pgn)"));
		connect(dlg, &QFileDialog::fileSelected, ui->m_pgnoutEdit, &QLineEdit::setText);
		dlg->setAttribute(Qt::WA_DeleteOnClose);
		dlg->setAcceptMode(QFileDialog::AcceptSave);
		dlg->open();
	});
	connect(ui->m_browseEpdoutBtn, &QPushButton::clicked, this, [=]()
	{
		auto dlg = new QFileDialog(this, tr("Select EPD output file"),
			QString(), tr("Extended Position Description (*.epd)"));
		connect(dlg, &QFileDialog::fileSelected, ui->m_epdoutEdit, &QLineEdit::setText);
		dlg->setAttribute(Qt::WA_DeleteOnClose);
		dlg->setAcceptMode(QFileDialog::AcceptSave);
		dlg->open();
	});

	m_addedEnginesManager = new EngineManager(this);
	m_addedEnginesModel = new EngineConfigurationModel(
		m_addedEnginesManager, this);
	ui->m_playersList->setModel(m_addedEnginesModel);

	connect(ui->m_playersList->selectionModel(),
		SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
		this, SLOT(onPlayerSelectionChanged(QItemSelection, QItemSelection)));
	connect(ui->m_playersList, SIGNAL(doubleClicked(QModelIndex)),
		this, SLOT(configureEngine(QModelIndex)));

	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	connect(ui->m_gameSettings, &GameSettingsWidget::statusChanged, [=](bool ok)
	{
		if (ok)
			ok = canStart();
		ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
	});

	ui->m_gameSettings->onHumanCountChanged(0);
	onVariantChanged(ui->m_gameSettings->chessVariant());
	readSettings();
}

NewTournamentDialog::~NewTournamentDialog()
{
	delete ui;
}

void NewTournamentDialog::addEngineOnDblClick(const QModelIndex& index)
{
	const QListView* listView = ((QListView*)sender());
	const QModelIndex& idx = m_proxyModel->mapToSource(index);

	m_addedEnginesManager->addEngine(m_srcEngineManager->engineAt(idx.row()));
	listView->selectionModel()->select(index, QItemSelectionModel::Deselect);

	QPushButton* button = ui->buttonBox->button(QDialogButtonBox::Ok);
	button->setEnabled(canStart());
}


void NewTournamentDialog::addEngine()
{
	EngineSelectionDialog dlg(m_proxyModel);
	connect(dlg.enginesList(), SIGNAL(doubleClicked(QModelIndex)), this,
		SLOT(addEngineOnDblClick(QModelIndex)));
	int value = dlg.exec();
	disconnect(dlg.enginesList(), SIGNAL(doubleClicked(QModelIndex)), this,
		   SLOT(addEngineOnDblClick(QModelIndex)));

	if (value != QDialog::Accepted)
		return;

	const QModelIndexList list(dlg.selection().indexes());
	for (const QModelIndex& index : list)
		m_addedEnginesManager->addEngine(m_srcEngineManager->engineAt(index.row()));

	QPushButton* button = ui->buttonBox->button(QDialogButtonBox::Ok);
	button->setEnabled(canStart());
}

void NewTournamentDialog::removeEngine()
{
	QList<QModelIndex> selected = ui->m_playersList->selectionModel()->selectedRows();

	// Can't use std::greater because operator> isn't implemented
	// for QModelIndex.
	std::sort(selected.begin(), selected.end(),
	[](const QModelIndex &a, const QModelIndex &b)
	{
		return b < a;
	});

	for (const QModelIndex& index : qAsConst(selected))
		m_addedEnginesManager->removeEngineAt(index.row());

	QPushButton* button = ui->buttonBox->button(QDialogButtonBox::Ok);
	button->setEnabled(canStart());
}

void NewTournamentDialog::configureEngine(const QModelIndex& index)
{
	EngineConfigurationDialog dlg(EngineConfigurationDialog::ConfigureEngine);

	int row = index.row();
	const EngineConfiguration& config = m_addedEnginesManager->engineAt(row);
	dlg.applyEngineInformation(config);

	QSet<QString> names = m_addedEnginesManager->engineNames();
	names.remove(config.name());
	dlg.setReservedNames(names);

	if (dlg.exec() == QDialog::Accepted)
		m_addedEnginesManager->updateEngineAt(row, dlg.engineConfiguration());

	QPushButton* button = ui->buttonBox->button(QDialogButtonBox::Ok);
	button->setEnabled(canStart());
}

void NewTournamentDialog::moveEngine(int offset)
{
	QModelIndex index(ui->m_playersList->currentIndex());
	int row1 = index.row();
	int row2 = row1 + offset;
	EngineConfiguration tmp(m_addedEnginesManager->engineAt(row1));

	m_addedEnginesManager->updateEngineAt(row1, m_addedEnginesManager->engineAt(row2));
	m_addedEnginesManager->updateEngineAt(row2, tmp);

	ui->m_playersList->setCurrentIndex(index.sibling(row2, 0));
}

bool NewTournamentDialog::canStart() const
{
	if (!ui->m_gameSettings->isValid())
		return false;

	if (m_addedEnginesManager->engineCount() < 2)
		return false;

	QPushButton* button = ui->buttonBox->button(QDialogButtonBox::Ok);

	// check for duplicate configuration names
	if (m_addedEnginesManager->engineNames().count()
	!=  m_addedEnginesManager->engineCount())
	{
		button->setText(tr("Resolve Duplicates!"));
		return false;
	}
	button->setText("&OK");

	QString variant = ui->m_gameSettings->chessVariant();
	return m_addedEnginesManager->supportsVariant(variant);
}

void NewTournamentDialog::onVariantChanged(const QString& variant)
{
	m_proxyModel->setFilterVariant(variant);
	ui->m_addEngineBtn->setEnabled(m_proxyModel->rowCount() > 0);

	m_addedEnginesModel->setChessVariant(variant);
	QPushButton* button = ui->buttonBox->button(QDialogButtonBox::Ok);
	button->setEnabled(canStart());

	onPlayerSelectionChanged(QItemSelection(), QItemSelection());
}

void NewTournamentDialog::onPlayerSelectionChanged(const QItemSelection& selected,
						   const QItemSelection& deselected)
{
	Q_UNUSED(selected);
	Q_UNUSED(deselected);

	bool enable = ui->m_playersList->selectionModel()->hasSelection();
	ui->m_configureEngineBtn->setEnabled(enable);
	ui->m_removeEngineBtn->setEnabled(enable);

	int i = ui->m_playersList->currentIndex().row();
	ui->m_moveEngineUpBtn->setEnabled(enable && i > 0);
	ui->m_moveEngineDownBtn->setEnabled(enable && i < m_addedEnginesManager->engineCount() - 1);
}

Tournament* NewTournamentDialog::createTournament(GameManager* gameManager) const
{
	Q_ASSERT(gameManager != nullptr);
	auto ts = ui->m_tournamentSettings;

	auto t = TournamentFactory::create(
		ts->tournamentType(), gameManager, parent());

	t->setPgnCleanupEnabled(false);
	t->setName(ui->m_nameEdit->text());
	t->setSite(ui->m_siteEdit->text());
	t->setVariant(ui->m_gameSettings->chessVariant());
	t->setPgnOutput(ui->m_pgnoutEdit->text());
	t->setEpdOutput(ui->m_epdoutEdit->text());

	t->setSeedCount(ts->seedCount());
	t->setGamesPerEncounter(ts->gamesPerEncounter());
	if (t->canSetRoundMultiplier())
		t->setRoundMultiplier(ts->rounds());
	t->setStartDelay(ts->delayBetweenGames());

	t->setAdjudicator(ui->m_gameSettings->adjudicator());

	t->setOpeningSuite(ui->m_gameSettings->openingSuite());
	t->setOpeningDepth(ui->m_gameSettings->openingSuiteDepth());

	t->setOpeningBookOwnership(true);
	auto book = ui->m_gameSettings->openingBook();
	int bookDepth = ui->m_gameSettings->bookDepth();

	t->setOpeningRepetitions(ts->openingRepetition()? 2: 1);
	t->setRecoveryMode(ts->engineRecovery());
	t->setPgnWriteUnfinishedGames(ts->savingOfUnfinishedGames());

	const auto engines = m_addedEnginesManager->engines();
	for (EngineConfiguration config : engines)
	{
		ui->m_gameSettings->applyEngineConfiguration(&config);
		t->addPlayer(new EngineBuilder(config),
			     ui->m_gameSettings->timeControl(),
			     book,
			     bookDepth);
	}

	return t;
}

void NewTournamentDialog::readSettings()
{
	ui->m_siteEdit->setText(QSettings().value("pgn/site").toString());

	QString pgnName = ui->m_pgnoutEdit->text();
	if (pgnName.isEmpty())
	{
		pgnName = QSettings().value("tournament/default_pgn_output_file",
					    QString()).toString();
		ui->m_pgnoutEdit->setText(pgnName);
	}

	QString epdName = ui->m_epdoutEdit->text();
	if (epdName.isEmpty())
	{
		epdName = QSettings().value("tournament/default_epd_output_file",
					    QString()).toString();
		ui->m_epdoutEdit->setText(epdName);
	}
}
