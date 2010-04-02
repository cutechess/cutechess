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


const QStringList EngineConfigurationModel::m_headers = (QStringList() <<
	tr("Name") << tr("Command") << tr("Working Directory") << tr("Protocol"));

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

	return m_engineManager->engines().count();
}

int EngineConfigurationModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)

	return m_headers.count();
}

QVariant EngineConfigurationModel::data(const QModelIndex& index, int role) const
{
	if (index.isValid() && role == Qt::DisplayRole)
	{
		switch (index.column())
		{
			case 0:
				return m_engineManager->engines().at(index.row()).name();

			case 1:
				return m_engineManager->engines().at(index.row()).command();

			case 2:
				return m_engineManager->engines().at(index.row()).workingDirectory();

			case 3:
				return m_engineManager->engines().at(index.row()).protocol();

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
		return m_headers.at(section);

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
