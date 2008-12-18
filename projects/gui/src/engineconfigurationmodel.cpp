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

#include "engineconfigurationmodel.h"

const QStringList EngineConfigurationModel::m_headers = (QStringList() <<
	tr("Name") << tr("Command") << tr("Working Directory") << tr("Protocol"));

EngineConfigurationModel::EngineConfigurationModel(QObject* parent)
	: QAbstractListModel(parent)
{
}

EngineConfigurationModel::EngineConfigurationModel(
	const QList<EngineConfiguration>& configurations, QObject* parent)
	: QAbstractListModel(parent),
	  m_configurations(configurations)
{

}

int EngineConfigurationModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)

	return m_configurations.count();
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
				return m_configurations.at(index.row()).name();

			case 1:
				return m_configurations.at(index.row()).command();

			case 2:
				return m_configurations.at(index.row()).workingDirectory();

			case 3:
				return m_configurations.at(index.row()).protocol();

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
		return m_headers.at(section);
	}

	return QVariant();
}

bool EngineConfigurationModel::insertRows(int row, int count,
                                          const QModelIndex& parent)
{
	Q_UNUSED(parent)

	beginInsertRows(QModelIndex(), row, row + count - 1);

	for (int i = 0; i < count; i++)
	{
		EngineConfiguration config(" ", " ", EngineConfiguration::Xboard);
		m_configurations.insert(row, config);
	}

	endInsertRows();
	return true;
}

bool EngineConfigurationModel::removeRows(int row, int count,
                                          const QModelIndex& parent)
{
	Q_UNUSED(parent)

	beginRemoveRows(QModelIndex(), row, row + count - 1);

	for (int i = 0; i < count; i++)
	{
		m_configurations.removeAt(row);
	}

	endRemoveRows();
	return true;
}

bool EngineConfigurationModel::setData(const QModelIndex& index,
                                       const QVariant& value, int role)
{
	if (index.isValid() && role == Qt::EditRole)
	{
		EngineConfiguration config = m_configurations.at(index.row());

		switch (index.column())
		{
			case 0:
				config.setName(value.toString());
			break;

			case 1:
				config.setCommand(value.toString());
			break;

			case 2:
				config.setWorkingDirectory(value.toString());

			case 3:
				config.setProtocol(EngineConfiguration::ChessEngineProtocol(
					value.toInt()));
			break;

			default:
				return false;
		}

		m_configurations[index.row()] = config;
		emit(dataChanged(index, index));

		return true;
	}
	return false;
}

void EngineConfigurationModel::addConfiguration(const EngineConfiguration& configuration)
{
	beginInsertRows(QModelIndex(), m_configurations.count(), m_configurations.count());

	m_configurations << configuration;

	endInsertRows();
}

QList<EngineConfiguration> EngineConfigurationModel::configurations() const
{
	return m_configurations;
}

bool EngineConfigurationModel::setConfiguration(const QModelIndex& index,
                                                const EngineConfiguration& configuration)
{
	if (index.isValid())
	{
		m_configurations[index.row()] = configuration;
		emit(dataChanged(index, index));
		return true;
	}
	return false;
}
