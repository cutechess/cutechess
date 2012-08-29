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

#include <board/boardfactory.h>
#include <enginemanager.h>
#include <enginebuilder.h>
#include <timecontrol.h>
#include <roundrobintournament.h>
#include <gauntlettournament.h>

#include "engineconfigurationmodel.h"
#include "engineconfigproxymodel.h"
#include "engineconfigurationdlg.h"
#include "timecontroldlg.h"
#include "engineselectiondlg.h"

NewTournamentDialog::NewTournamentDialog(EngineManager* engineManager,
					 QWidget *parent)
	: QDialog(parent),
	  m_srcEngineManager(engineManager),
	  ui(new Ui::NewTournamentDialog)
{
	Q_ASSERT(engineManager != 0);
	ui->setupUi(this);

	m_srcEnginesModel = new EngineConfigurationModel(engineManager, this);

	m_proxyModel = new EngineConfigurationProxyModel(this);
	m_proxyModel->setSourceModel(m_srcEnginesModel);
	m_proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
	m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
	m_proxyModel->sort(0);
	m_proxyModel->setDynamicSortFilter(true);

	ui->m_variantCombo->addItems(Chess::BoardFactory::variants());
	connect(ui->m_variantCombo, SIGNAL(currentIndexChanged(QString)),
		this, SLOT(onVariantChanged(QString)));

	int index = ui->m_variantCombo->findText("standard");
	ui->m_variantCombo->setCurrentIndex(index);

	connect(ui->m_addEngineBtn, SIGNAL(clicked()),
		this, SLOT(addEngine()));
	connect(ui->m_removeEngineBtn, SIGNAL(clicked()),
		this, SLOT(removeEngine()));
	connect(ui->m_configureEngineBtn, SIGNAL(clicked()),
		this, SLOT(configureEngine()));
	connect(ui->m_moveEngineUpBtn, SIGNAL(clicked()),
		this, SLOT(moveEngineUp()));
	connect(ui->m_moveEngineDownBtn, SIGNAL(clicked()),
		this, SLOT(moveEngineDown()));

	connect(ui->m_timeControlBtn, SIGNAL(clicked()),
		this, SLOT(changeTimeControl()));
	m_timeControl.setMovesPerTc(40);
	m_timeControl.setTimePerTc(300000);
	ui->m_timeControlBtn->setText(m_timeControl.toVerboseString());

	connect(ui->m_browsePgnoutBtn, SIGNAL(clicked()),
		this, SLOT(browsePgnout()));

	m_addedEnginesManager = new EngineManager(this);
	m_addedEnginesModel = new EngineConfigurationModel(
		m_addedEnginesManager, this);
	ui->m_playersList->setModel(m_addedEnginesModel);

	connect(ui->m_playersList->selectionModel(),
		SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
		this, SLOT(onPlayerSelectionChanged(QItemSelection, QItemSelection)));

	ui->m_moveEngineUpBtn->setIcon(style()->standardIcon(QStyle::SP_ArrowUp));
	ui->m_moveEngineDownBtn->setIcon(style()->standardIcon(QStyle::SP_ArrowDown));

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
	qSort(selected.begin(), selected.end(), qGreater<QModelIndex>());

	foreach (const QModelIndex& index, selected)
		m_addedEnginesManager->removeEngineAt(index.row());

	QPushButton* button = ui->buttonBox->button(QDialogButtonBox::Ok);
	button->setEnabled(m_addedEnginesManager->engineCount() > 1);
}

void NewTournamentDialog::configureEngine()
{
	EngineConfigurationDialog dlg(EngineConfigurationDialog::ConfigureEngine);

	int i = ui->m_playersList->currentIndex().row();
	dlg.applyEngineInformation(m_addedEnginesManager->engineAt(i));

	if (dlg.exec() == QDialog::Accepted)
		m_addedEnginesManager->updateEngineAt(i, dlg.engineConfiguration());
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

void NewTournamentDialog::moveEngineUp()
{
	moveEngine(-1);
}

void NewTournamentDialog::moveEngineDown()
{
	moveEngine(1);
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

void NewTournamentDialog::changeTimeControl()
{
	TimeControlDialog dlg(m_timeControl);
	if (dlg.exec() == QDialog::Accepted)
	{
		m_timeControl = dlg.timeControl();
		ui->m_timeControlBtn->setText(m_timeControl.toVerboseString());
	}
}

void NewTournamentDialog::browsePgnout()
{
	QString str = QFileDialog::getSaveFileName(this,
						   tr("PGN output file"),
						   QString(),
						   tr("Portable Game Notation (*.pgn)"));
	if (!str.isEmpty())
		ui->m_pgnoutEdit->setText(str);
}

Tournament* NewTournamentDialog::createTournament(GameManager* gameManager) const
{
	Q_ASSERT(gameManager != 0);

	Tournament* t = 0;
	if (ui->m_roundRobinRadio->isChecked())
		t = new RoundRobinTournament(gameManager, parent());
	else if (ui->m_gauntletRadio->isChecked())
		t = new GauntletTournament(gameManager, parent());
	else
		return 0;

	t->setPgnCleanupEnabled(false);
	t->setName(ui->m_nameEdit->text());
	t->setSite(ui->m_siteEdit->text());
	t->setVariant(ui->m_variantCombo->currentText());
	t->setPgnOutput(ui->m_pgnoutEdit->text());
	t->setGamesPerEncounter(ui->m_gamesPerEncounterSpin->value());
	t->setRoundMultiplier(ui->m_roundsSpin->value());

	foreach (const EngineConfiguration& config, m_addedEnginesManager->engines())
	{
		t->addPlayer(new EngineBuilder(config),
			     m_timeControl,
			     0,
			     256);
	}

	return t;
}
