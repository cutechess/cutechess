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

#include "enginemanagementwidget.h"
#include "ui_enginemanagementwidget.h"

#include <QSortFilterProxyModel>
#include <QFileDialog>
#include <QSettings>
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

	// Default location for new engines
	QString dir = QSettings().value("ui/default_engine_location").toString();
	ui->m_defaultLocationEdit->setText(dir);
	connect(ui->m_browseDefaultLocationBtn, SIGNAL(clicked()),
		this, SLOT(browseDefaultLocation()));

	updateEngineCount();
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
	updateEngineCount();
}

void EngineManagementWidget::updateEngineCount()
{
	QString text = tr("Showing %1 of %2 engines")
		       .arg(m_filteredModel->rowCount())
		       .arg(m_engineManager->engineCount());
	ui->m_engineCountLabel->setText(text);
}

void EngineManagementWidget::addEngine()
{
	auto dlg = new EngineConfigurationDialog(
		EngineConfigurationDialog::AddEngine, this);
	dlg->setAttribute(Qt::WA_DeleteOnClose);
	dlg->setReservedNames(m_engineManager->engineNames());

	connect(dlg, &EngineConfigurationDialog::accepted, [=]()
	{
		m_engineManager->addEngine(dlg->engineConfiguration());
		m_hasChanged = true;
	});
	dlg->open();
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

	QSet<QString> names = m_engineManager->engineNames();
	names.remove(config.name());

	auto dlg = new EngineConfigurationDialog(
		EngineConfigurationDialog::ConfigureEngine, this);
	dlg->setAttribute(Qt::WA_DeleteOnClose);
	dlg->applyEngineInformation(config);
	dlg->setReservedNames(names);

	connect(dlg, &EngineConfigurationDialog::accepted, [=]()
	{
		m_engineManager->updateEngineAt(row, dlg->engineConfiguration());
		m_hasChanged = true;
	});
	dlg->open();
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
		for (const QModelIndex& index : qAsConst(selected))
			m_engineManager->removeEngineAt(index.row());
		m_hasChanged = true;
	}
}

void EngineManagementWidget::browseDefaultLocation()
{
	auto dlg = new QFileDialog(
		this, tr("Choose Directory"),
		ui->m_defaultLocationEdit->text());
	dlg->setFileMode(QFileDialog::Directory);
	dlg->setOption(QFileDialog::ShowDirsOnly);
	dlg->setAttribute(Qt::WA_DeleteOnClose);
	dlg->setAcceptMode(QFileDialog::AcceptOpen);

	connect(dlg, &QFileDialog::fileSelected, [=](const QString& dir)
	{
		ui->m_defaultLocationEdit->setText(dir);
		QSettings().setValue("ui/default_engine_location", dir);
	});
	dlg->open();
}
