/*
    This file is part of SloppyGUI.

    SloppyGUI is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SloppyGUI is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SloppyGUI.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QSortFilterProxyModel>
#include "enginemanagementdlg.h"
#include "engineconfigurationdlg.h"
#include "engineconfigurationmodel.h"

EngineManagementDialog::EngineManagementDialog(
	EngineConfigurationModel* engineConfigurations, QWidget* parent)
	: QDialog(parent)
{
	setupUi(this);

	m_originalModel = engineConfigurations;

	// Create a filtered model
	m_filteredModel = new QSortFilterProxyModel(this);
	m_filteredModel->setSourceModel(engineConfigurations);
	m_filteredModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

	m_enginesList->setModel(m_filteredModel);

	connect(m_filterEnginesEdit, SIGNAL(textChanged(const QString&)),
		m_filteredModel, SLOT(setFilterWildcard(const QString&)));
	
	// Signals for updating the UI
	connect(m_enginesList, SIGNAL(clicked(const QModelIndex&)),
		this, SLOT(updateUi()));
	connect(m_filteredModel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
		this, SLOT(updateUi()));
	
	// Add button
	connect(m_addBtn, SIGNAL(clicked(bool)), this, SLOT(addEngine()));

	// Configure button
	connect(m_configureBtn, SIGNAL(clicked(bool)), this,
		SLOT(configureEngine()));
	
	// Remove button
	connect(m_removeBtn, SIGNAL(clicked(bool)), this, SLOT(removeEngine()));
}

void EngineManagementDialog::updateUi()
{
	// Enable buttons depending on item selections
	if (m_filteredModel->rowCount() > 0)
	{
		if (m_enginesList->selectionModel()->hasSelection())
		{
			m_configureBtn->setEnabled(true);
			m_removeBtn->setEnabled(true);
		}
		else
		{
			m_configureBtn->setEnabled(false);
			m_removeBtn->setEnabled(false);
		}
	}
	else
	{
		m_configureBtn->setEnabled(false);
		m_removeBtn->setEnabled(false);
	}
}

void EngineManagementDialog::addEngine()
{
	EngineConfigurationDialog dlg(EngineConfigurationDialog::AddEngine, this);
	if (dlg.exec() == QDialog::Accepted)
	{
		m_originalModel->addConfiguration(dlg.engineConfiguration());
	}
}

void EngineManagementDialog::configureEngine()
{
	// Map the index from the filtered model to the original model
	QModelIndex index = m_filteredModel->mapToSource(
		m_enginesList->selectionModel()->currentIndex());

	QList<EngineConfiguration> engines = m_originalModel->configurations();

	EngineConfigurationDialog dlg(EngineConfigurationDialog::EditEngine, this);
	dlg.applyEngineInformation(engines.at(index.row()));

	if (dlg.exec() == QDialog::Accepted)
	{
		// Update the configuration in the model
		m_originalModel->setConfiguration(index, dlg.engineConfiguration());
	}
}

void EngineManagementDialog::removeEngine()
{
	QList<QModelIndex> selected =
		m_enginesList->selectionModel()->selectedIndexes();

	foreach (QModelIndex index, selected)
	{
		// Map the index from the filtered model to the original model
		m_originalModel->removeRows(
			m_filteredModel->mapToSource(index).row(), 1);
	}
}

