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
#include <functional>
#include <algorithm>

#include <board/boardfactory.h>
#include <enginemanager.h>
#include <enginebuilder.h>
#include <timecontrol.h>
#include <roundrobintournament.h>
#include <gauntlettournament.h>
#include <knockouttournament.h>
#include <openingsuite.h>

#include "engineconfigurationmodel.h"
#include "engineconfigproxymodel.h"
#include "engineconfigurationdlg.h"
#include "timecontroldlg.h"
#include "engineselectiondlg.h"

#include <modeltest.h>

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
	onVariantChanged(ui->m_gameSettings->chessVariant());

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

	connect(ui->m_browseOpeningSuiteBtn, &QPushButton::clicked, this, [=]()
	{
		auto dlg = new QFileDialog(this, tr("Select opening suite"), QString(),
			tr("PGN/EPD files (*.pgn *.epd)"));
		connect(dlg, &QFileDialog::fileSelected, ui->m_suiteFileEdit, &QLineEdit::setText);
		dlg->setAttribute(Qt::WA_DeleteOnClose);
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

	connect(ui->m_knockoutRadio, &QRadioButton::toggled, [=](bool checked)
	{
		ui->m_roundsSpin->setDisabled(checked);
	});

	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

NewTournamentDialog::~NewTournamentDialog()
{
	delete ui;
}

void NewTournamentDialog::addEngine()
{
	EngineSelectionDialog dlg(m_proxyModel);
	if (dlg.exec() != QDialog::Accepted)
		return;

	QModelIndexList list(dlg.selection().indexes());
	foreach (const QModelIndex& index, list)
		m_addedEnginesManager->addEngine(m_srcEngineManager->engineAt(index.row()));

	QPushButton* button = ui->buttonBox->button(QDialogButtonBox::Ok);
	button->setEnabled(m_addedEnginesManager->engineCount() > 1);
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

	foreach (const QModelIndex& index, selected)
		m_addedEnginesManager->removeEngineAt(index.row());

	QPushButton* button = ui->buttonBox->button(QDialogButtonBox::Ok);
	button->setEnabled(m_addedEnginesManager->engineCount() > 1);
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

void NewTournamentDialog::onVariantChanged(const QString& variant)
{
	m_proxyModel->setFilterVariant(variant);
	ui->m_addEngineBtn->setEnabled(m_proxyModel->rowCount() > 0);
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

	Tournament* t = nullptr;
	if (ui->m_roundRobinRadio->isChecked())
		t = new RoundRobinTournament(gameManager, parent());
	else if (ui->m_gauntletRadio->isChecked())
		t = new GauntletTournament(gameManager, parent());
	else if (ui->m_knockoutRadio->isChecked())
		t = new KnockoutTournament(gameManager, parent());
	else
		return nullptr;

	t->setPgnCleanupEnabled(false);
	t->setName(ui->m_nameEdit->text());
	t->setSite(ui->m_siteEdit->text());
	t->setVariant(ui->m_gameSettings->chessVariant());
	t->setPgnOutput(ui->m_pgnoutEdit->text());
	t->setGamesPerEncounter(ui->m_gamesPerEncounterSpin->value());
	if (t->canSetRoundMultiplier())
		t->setRoundMultiplier(ui->m_roundsSpin->value());

	const QString fileName = ui->m_suiteFileEdit->text();
	if (!fileName.isEmpty())
	{
		OpeningSuite::Format format = OpeningSuite::PgnFormat;
		if (fileName.endsWith(".epd"))
			format = OpeningSuite::EpdFormat;

		OpeningSuite::Order order = OpeningSuite::SequentialOrder;
		if (ui->m_seqOrderRadio->isChecked())
			order = OpeningSuite::SequentialOrder;
		else if (ui->m_randomOrderRadio->isChecked())
			order = OpeningSuite::RandomOrder;

		OpeningSuite* suite = new OpeningSuite(ui->m_suiteFileEdit->text(),
		                         format, order, 0);
		if (suite->initialize())
			t->setOpeningSuite(suite);
		else
		{
			delete suite;
			delete t;
			return nullptr;
		}
	}

	t->setOpeningRepetition(ui->m_repeatCheckBox->isChecked());

	foreach (EngineConfiguration config, m_addedEnginesManager->engines())
	{
		ui->m_gameSettings->applyEngineConfiguration(&config);
		t->addPlayer(new EngineBuilder(config),
			     ui->m_gameSettings->timeControl(),
			     nullptr,
			     256);
	}

	return t;
}
