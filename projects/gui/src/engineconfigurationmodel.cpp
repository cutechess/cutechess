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

#include "engineconfigurationmodel.h"
#include <enginemanager.h>


EngineConfigurationModel::EngineConfigurationModel(EngineManager* engineManager, QObject* parent)
	: QAbstractListModel(parent), m_engineManager(engineManager)
{
	Q_ASSERT(m_engineManager != 0);

	connect(m_engineManager, SIGNAL(engineAdded(int)), this,
		SLOT(onEngineAdded(int)));
	connect(m_engineManager, SIGNAL(engineAboutToBeRemoved(int)), this,
		SLOT(onEngineAboutToBeRemoved(int)));
	connect(m_engineManager, SIGNAL(engineUpdated(int)), this,
		SLOT(onEngineUpdated(int)));
	connect(m_engineManager, SIGNAL(enginesReset()), this,
		SLOT(onEnginesReset()));
}

int EngineConfigurationModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)

	return m_engineManager->engineCount();
}

int EngineConfigurationModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)

	return 5;
}

QVariant EngineConfigurationModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		const EngineConfiguration engine(m_engineManager->engineAt(index.row()));

		switch (index.column())
		{
		case 0:
			return engine.name();
		case 1:
			return engine.command();
		case 2:
			return engine.workingDirectory();
		case 3:
			return engine.protocol();
		case 4:
			return engine.supportedVariants();
		default:
			return QVariant();
		}
	}

	return QVariant();
}

QVariant EngineConfigurationModel::headerData(int section, Qt::Orientation orientation,
                                              int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
	{
		switch (section)
		{
		case 0:
			return tr("Name");
		case 1:
			return tr("Command");
		case 2:
			return tr("Working Directory");
		case 3:
			return tr("Protocol");
		case 4:
			return tr("Variants");
		default:
			return QVariant();
		}
	}

	return QVariant();
}

void EngineConfigurationModel::onEngineAdded(int index)
{
	beginInsertRows(QModelIndex(), index, index);
	endInsertRows();
}

void EngineConfigurationModel::onEngineAboutToBeRemoved(int index)
{
	beginRemoveRows(QModelIndex(), index, index);
	endRemoveRows();
}

void EngineConfigurationModel::onEngineUpdated(int index)
{
	QModelIndex indexToUpdate = this->index(index);
	emit dataChanged(indexToUpdate, indexToUpdate);
}

void EngineConfigurationModel::onEnginesReset()
{
	reset();
}
