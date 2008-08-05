#include <QDebug>

#include "engineconfigurationmodel.h"

EngineConfigurationModel::EngineConfigurationModel(QObject* parent)
	: QAbstractListModel(parent)
{
}

int EngineConfigurationModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)

	return m_configurations.count();
}

QVariant EngineConfigurationModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();
	
	if (index.row() > m_configurations.count())
		return QVariant();
	
	if (role == Qt::DisplayRole)
	{
		switch (index.column())
		{
			case 0:
				return m_configurations.at(index.row()).name();

			case 1:
				return m_configurations.at(index.row()).command();

			case 2:
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
	if (role != Qt::DisplayRole)
		return QVariant();
	
	if (orientation == Qt::Horizontal)
	{
		switch (section)
		{
			case 0:
				return QString(tr("Name"));

			case 1:
				return QString(tr("Command"));

			case 2:
				return QString(tr("Protocol"));
				
			default:
				return QVariant();
		}
	}

	return QVariant();
}

void EngineConfigurationModel::addEngineConfiguration(const EngineConfiguration& configuration)
{
	beginInsertRows(QModelIndex(), m_configurations.count(), m_configurations.count() + 1);

	m_configurations << configuration;

	endInsertRows();
}

QList<EngineConfiguration> EngineConfigurationModel::engineConfigurations() const
{
	return m_configurations;
}

