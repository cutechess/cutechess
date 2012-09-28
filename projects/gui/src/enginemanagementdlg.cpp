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

#include "enginemanagementdlg.h"
#include "ui_enginemanagementdlg.h"

#include <QSortFilterProxyModel>

#include <enginemanager.h>

#include "cutechessapp.h"
#include "engineconfigurationmodel.h"
#include "engineconfigurationdlg.h"


EngineManagementDialog::EngineManagementDialog(QWidget* parent)
	: QDialog(parent),
	  m_filteredModel(new QSortFilterProxyModel(this)),
	  ui(new Ui::EngineManagementDialog)
{
	ui->setupUi(this);

	// Setup a copy of the Engine Manager to manage adding, deleting and
	// configuring engines _within_ this dialog.
	m_engineManager = new EngineManager(this);
	m_engineManager->setEngines(
		CuteChessApplication::instance()->engineManager()->engines());

	// Set up a filtered model
	m_filteredModel->setSourceModel(new EngineConfigurationModel(
		m_engineManager, this));
	m_filteredModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
	m_filteredModel->setSortCaseSensitivity(Qt::CaseInsensitive);
	m_filteredModel->sort(0);
	m_filteredModel->setDynamicSortFilter(true);
	ui->m_enginesList->setModel(m_filteredModel);

	connect(ui->m_searchEngineEdit, SIGNAL(textChanged(QString)),
		this, SLOT(updateSearch(QString)));
	
	// Signals for updating the UI
	connect(ui->m_enginesList->selectionModel(),
		SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		this, SLOT(updateUi()));
	connect(ui->m_enginesList, SIGNAL(doubleClicked(QModelIndex)),
		this, SLOT(configureEngine(QModelIndex)));
	
	// Add button
	connect(ui->m_addBtn, SIGNAL(clicked(bool)), this, SLOT(addEngine()));

	// Configure button
	connect(ui->m_configureBtn, SIGNAL(clicked(bool)), this,
		SLOT(configureEngine()));
	
	// Remove button
	connect(ui->m_removeBtn, SIGNAL(clicked(bool)), this, SLOT(removeEngine()));
}

EngineManagementDialog::~EngineManagementDialog()
{
	delete ui;
}

void EngineManagementDialog::updateUi()
{
	// Enable buttons depending on item selections
	ui->m_configureBtn->setEnabled(
		ui->m_enginesList->selectionModel()->hasSelection());

	ui->m_removeBtn->setEnabled(
		ui->m_enginesList->selectionModel()->hasSelection());
}

void EngineManagementDialog::updateSearch(const QString& terms)
{
	m_filteredModel->setFilterWildcard(terms);
	ui->m_clearBtn->setEnabled(!terms.isEmpty());
}

void EngineManagementDialog::addEngine()
{
	EngineConfigurationDialog dlg(EngineConfigurationDialog::AddEngine, this);
	if (dlg.exec() == QDialog::Accepted)
		m_engineManager->addEngine(dlg.engineConfiguration());
}

void EngineManagementDialog::configureEngine()
{
	configureEngine(ui->m_enginesList->currentIndex());
}

void EngineManagementDialog::configureEngine(const QModelIndex& index)
{
	// Map the index from the filtered model to the original model
	int row = m_filteredModel->mapToSource(index).row();

	EngineConfigurationDialog dlg(EngineConfigurationDialog::ConfigureEngine, this);
	dlg.applyEngineInformation(m_engineManager->engines().at(row));

	if (dlg.exec() == QDialog::Accepted)
		m_engineManager->updateEngineAt(row, dlg.engineConfiguration());
}

void EngineManagementDialog::removeEngine()
{
	const QItemSelection selection =
		m_filteredModel->mapSelectionToSource(ui->m_enginesList->selectionModel()->selection());
	QModelIndexList selected = selection.indexes();
	qSort(selected.begin(), selected.end(), qGreater<QModelIndex>());

	foreach (const QModelIndex& index, selected)
		m_engineManager->removeEngineAt(index.row());
}

QList<EngineConfiguration> EngineManagementDialog::engines() const
{
	return m_engineManager->engines();
}
