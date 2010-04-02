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
#include <QSortFilterProxyModel>
#include <enginemanager.h>
#include "cutechessapp.h"
#include "engineconfigurationmodel.h"
#include "engineconfigurationdlg.h"


EngineManagementDialog::EngineManagementDialog(QWidget* parent)
	: QDialog(parent),
	  m_filteredModel(new QSortFilterProxyModel(this))
{
	setupUi(this);

	// Setup a copy of the Engine Manager to manage adding, deleting and
	// configuring engines _within_ this dialog.
	m_engineManager = new EngineManager(this);
	m_engineManager->setEngines(
		CuteChessApplication::instance()->engineManager()->engines());

	// Set up a filtered model
	m_filteredModel->setSourceModel(new EngineConfigurationModel(
		m_engineManager, this));

	m_filteredModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

	m_enginesList->setModel(m_filteredModel);

	connect(m_searchEngineEdit, SIGNAL(textChanged(const QString&)),
		this, SLOT(updateSearch(const QString&)));
	
	// Signals for updating the UI
	connect(m_enginesList->selectionModel(),
		SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
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
	m_configureBtn->setEnabled(
		m_enginesList->selectionModel()->hasSelection());

	m_removeBtn->setEnabled(
		m_enginesList->selectionModel()->hasSelection());
}

void EngineManagementDialog::updateSearch(const QString& terms)
{
	m_filteredModel->setFilterWildcard(terms);
	m_clearBtn->setEnabled(!terms.isEmpty());
}

void EngineManagementDialog::addEngine()
{
	EngineConfigurationDialog dlg(EngineConfigurationDialog::AddEngine, this);
	if (dlg.exec() == QDialog::Accepted)
		m_engineManager->addEngine(dlg.engineConfiguration());
}

void EngineManagementDialog::configureEngine()
{
	// Map the index from the filtered model to the original model
	int index = m_filteredModel->mapToSource(
		m_enginesList->selectionModel()->currentIndex()).row();

	EngineConfigurationDialog dlg(EngineConfigurationDialog::ConfigureEngine, this);
	dlg.applyEngineInformation(m_engineManager->engines().at(index));

	if (dlg.exec() == QDialog::Accepted)
		m_engineManager->updateEngineAt(index, dlg.engineConfiguration());
}

void EngineManagementDialog::removeEngine()
{
	const QList<QModelIndex> selected =
		m_enginesList->selectionModel()->selectedIndexes();

	foreach (const QModelIndex& index, selected)
	{
		// Map the index from the filtered model to the original model
		m_engineManager->removeEngineAt(
			m_filteredModel->mapToSource(index).row());
	}
}

QList<EngineConfiguration> EngineManagementDialog::engines() const
{
	return m_engineManager->engines();
}
