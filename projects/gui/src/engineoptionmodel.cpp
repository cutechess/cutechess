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

#include "engineoptionmodel.h"

#include <engineoption.h>
#include <enginebuttonoption.h>


EngineOptionModel::EngineOptionModel(QObject* parent)
	: QAbstractItemModel(parent)
{
}

EngineOptionModel::EngineOptionModel(QList<EngineOption*> options, QObject* parent)
	: QAbstractItemModel(parent),
	  m_options(options)
{
}

void EngineOptionModel::setOptions(const QList<EngineOption*>& options)
{
	beginResetModel();
	m_options = options;
	endResetModel();
}

QModelIndex EngineOptionModel::index(int row, int column,
                                     const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	return createIndex(row, column);
}

QModelIndex EngineOptionModel::parent(const QModelIndex& index) const
{
	Q_UNUSED(index);

	return QModelIndex();
}

int EngineOptionModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;

	return m_options.count();
}

int EngineOptionModel::columnCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;

	return 3;
}

QVariant EngineOptionModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();
	const EngineOption* option = m_options.at(index.row());

	if (role == Qt::DisplayRole)
	{
		switch (index.column())
		{
			case 0:
				return option->name();

			case 1:
				// see Qt::CheckStateRole
				if (option->value().type() == QVariant::Bool)
					return QVariant();

				return option->value();

			case 2:
				return option->alias();

			default:
				return QVariant();
		}
	}
	else if (role == Qt::CheckStateRole)
	{
		if (index.column() == 1 && option->value().type() == QVariant::Bool)
			return option->value().toBool() ? Qt::Checked : Qt::Unchecked;

		return QVariant();
	}
	else if (role == Qt::EditRole)
	{
		switch (index.column())
		{
			case 0:
				return option->name();

			case 1:
				return option->toVariant();

			case 2:
				return option->alias();

			default:
				return QVariant();
		}
	}

	return QVariant();
}

QVariant EngineOptionModel::headerData(int section, Qt::Orientation orientation,
                                       int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
	{
		switch (section)
		{
		case 0:
			return tr("Name");
		case 1:
			return tr("Value");
		case 2:
			return tr("Alias");
		default:
			return QVariant();
		}
	}

	return QVariant();
}

Qt::ItemFlags EngineOptionModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::ItemFlags(Qt::NoItemFlags);

	Qt::ItemFlags defaultFlags =
		Qt::ItemFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

	if (index.column() == 0)
		return defaultFlags;
	if (index.column() == 2)
		return Qt::ItemFlags(defaultFlags | Qt::ItemIsEditable);

	if (index.column() == 1)
	{
		const auto option = m_options.at(index.row());
		if (!option->isEditable())
			return defaultFlags;

		// make check options checkable
		if (option->value().type() == QVariant::Bool)
			return Qt::ItemFlags(defaultFlags | Qt::ItemIsUserCheckable);

		return Qt::ItemFlags(defaultFlags | Qt::ItemIsEditable);
	}

	return defaultFlags;
}

bool EngineOptionModel::setData(const QModelIndex& index, const QVariant& data,
                                int role)
{
	Q_UNUSED(role);

	if (!index.isValid())
		return false;

	EngineOption* option = m_options.at(index.row());
	if (index.column() == 0)
	{
		const QString stringData = data.toString();
		if (stringData.isEmpty())
			return false;

		option->setName(stringData);
		return true;
	}
	else if (index.column() == 1)
	{
		if (role == Qt::CheckStateRole)
		{
			option->setValue(data.toInt() == Qt::Checked);
			return true;
		}
		else if (option->isValid(data))
		{
			option->setValue(data);
			return true;
		}
	}
	else if (index.column() == 2)
	{
		option->setAlias(data.toString());
		return true;
	}
	return false;
}
