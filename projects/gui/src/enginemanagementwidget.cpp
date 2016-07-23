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

#include "enginemanagementwidget.h"
#include "ui_enginemanagementwidget.h"

#include <QSortFilterProxyModel>
#include <functional>
#include <algorithm>

#include <enginemanager.h>

#include "cutechessapp.h"
#include "engineconfigurationmodel.h"
#include "engineconfigurationdlg.h"


EngineManagementWidget::EngineManagementWidget(QWidget* parent)
	: QWidget(parent),
	  m_engineManager(CuteChessApplication::instance()->engineManager()),
	  m_hasChanged(false),
	  m_filteredModel(new QSortFilterProxyModel(this)),
	  ui(new Ui::EngineManagementWidget)
{
	ui->setupUi(this);

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

EngineManagementWidget::~EngineManagementWidget()
{
	delete ui;
}

bool EngineManagementWidget::hasConfigChanged() const
{
	return m_hasChanged;
}

void EngineManagementWidget::saveConfig()
{
	QString confPath = CuteChessApplication::instance()->configPath();
	m_engineManager->saveEngines(confPath + QLatin1String("/engines.json"));
}

void EngineManagementWidget::updateUi()
{
	// Enable buttons depending on item selections
	ui->m_configureBtn->setEnabled(
		ui->m_enginesList->selectionModel()->hasSelection());

	ui->m_removeBtn->setEnabled(
		ui->m_enginesList->selectionModel()->hasSelection());
}

void EngineManagementWidget::updateSearch(const QString& terms)
{
	m_filteredModel->setFilterWildcard(terms);
}

void EngineManagementWidget::addEngine()
{
	EngineConfigurationDialog dlg(EngineConfigurationDialog::AddEngine, this);
	dlg.setReservedNames(m_engineManager->engineNames());
	if (dlg.exec() == QDialog::Accepted)
	{
		m_engineManager->addEngine(dlg.engineConfiguration());
		m_hasChanged = true;
	}
}

void EngineManagementWidget::configureEngine()
{
	configureEngine(ui->m_enginesList->currentIndex());
}

void EngineManagementWidget::configureEngine(const QModelIndex& index)
{
	// Map the index from the filtered model to the original model
	int row = m_filteredModel->mapToSource(index).row();
	const EngineConfiguration& config = m_engineManager->engineAt(row);

	EngineConfigurationDialog dlg(EngineConfigurationDialog::ConfigureEngine, this);
	dlg.applyEngineInformation(config);

	QSet<QString> names = m_engineManager->engineNames();
	names.remove(config.name());
	dlg.setReservedNames(names);

	if (dlg.exec() == QDialog::Accepted)
	{
		m_engineManager->updateEngineAt(row, dlg.engineConfiguration());
		m_hasChanged = true;
	}
}

void EngineManagementWidget::removeEngine()
{
	const QItemSelection selection =
		m_filteredModel->mapSelectionToSource(ui->m_enginesList->selectionModel()->selection());
	QModelIndexList selected = selection.indexes();

	// Can't use std::greater because operator> isn't implemented
	// for QModelIndex.
	std::sort(selected.begin(), selected.end(),
	[](const QModelIndex &a, const QModelIndex &b)
	{
		return b < a;
	});

	if (!selected.isEmpty())
	{
		foreach (const QModelIndex& index, selected)
			m_engineManager->removeEngineAt(index.row());
		m_hasChanged = true;
	}
}
