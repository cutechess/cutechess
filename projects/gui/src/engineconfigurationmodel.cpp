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

#include "engineconfigurationmodel.h"
#include <enginemanager.h>
#include <QBrush>


EngineConfigurationModel::EngineConfigurationModel(EngineManager* engineManager, QObject* parent)
	: QAbstractListModel(parent), m_engineManager(engineManager)
{
	Q_ASSERT(m_engineManager != nullptr);

	connect(m_engineManager, SIGNAL(engineAdded(int)), this,
		SLOT(onEngineAdded(int)));
	connect(m_engineManager, SIGNAL(engineAboutToBeRemoved(int)), this,
		SLOT(onEngineAboutToBeRemoved(int)));
	connect(m_engineManager, SIGNAL(engineUpdated(int)), this,
		SLOT(onEngineUpdated(int)));
	connect(m_engineManager, SIGNAL(enginesReset()), this,
		SLOT(onEnginesReset()));
}

EngineConfigurationModel::~EngineConfigurationModel()
{
}

QString EngineConfigurationModel::chessVariant() const
{
	return m_chessVariant;
}

void EngineConfigurationModel::setChessVariant(const QString& variant)
{
	if (variant == m_chessVariant)
		return;

	beginResetModel();
	m_chessVariant = variant;
	endResetModel();
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
	else if (role == Qt::ForegroundRole)
	{
		if (m_chessVariant.isEmpty())
			return QVariant();

		const auto engine = m_engineManager->engineAt(index.row());
		if (!engine.supportsVariant(m_chessVariant))
			return QBrush(Qt::red);

		int count = 0;
		const auto& engines = m_engineManager->engines();
		for (const EngineConfiguration& engine2: engines)
			if (engine.name() == engine2.name())
				if (++count > 1)
					return QBrush(Qt::gray);

		return QVariant();
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
	beginResetModel();
	endResetModel();
}
