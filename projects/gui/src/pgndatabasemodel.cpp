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

#include "pgndatabasemodel.h"


PgnDatabaseModel::PgnDatabaseModel(GameDatabaseManager* gameDatabaseManager,
                                   QObject* parent)
	: QAbstractItemModel(parent),
	  m_gameDatabaseManager(gameDatabaseManager)
{
	Q_ASSERT(m_gameDatabaseManager);

	connect(m_gameDatabaseManager, SIGNAL(databaseAdded(int)), this,
		SLOT(onDatabaseAdded(int)));
	connect(m_gameDatabaseManager, SIGNAL(databaseAboutToBeRemoved(int)), this,
		SLOT(onDatabaseAboutToBeRemoved(int)));
	connect(m_gameDatabaseManager, SIGNAL(databasesReset()), this,
		SLOT(onDatabasesReset()));
}

void PgnDatabaseModel::onDatabaseAdded(int index)
{
	beginInsertRows(QModelIndex(), index, index);
	endInsertRows();
}

void PgnDatabaseModel::onDatabaseAboutToBeRemoved(int index)
{
	beginRemoveRows(QModelIndex(), index, index);
	endRemoveRows();
}

void PgnDatabaseModel::onDatabasesReset()
{
	beginResetModel();
	endResetModel();
}

QModelIndex PgnDatabaseModel::index(int row, int column,
                                 const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	return createIndex(row, column);
}

QModelIndex PgnDatabaseModel::parent(const QModelIndex& index) const
{
	Q_UNUSED(index);

	return QModelIndex();
}

int PgnDatabaseModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;

	return m_gameDatabaseManager->databases().count();
}

int PgnDatabaseModel::columnCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;

	return 1;
}

QVariant PgnDatabaseModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	const PgnDatabase* db = m_gameDatabaseManager->databases().at(index.row());

	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		switch (index.column())
		{
			case 0:
				return db->displayName();

			default:
				return QVariant();
		}
	}
	else if (role == Qt::ToolTipRole && index.column() == 0)
		return db->fileName();

	return QVariant();
}

QVariant PgnDatabaseModel::headerData(int section, Qt::Orientation orientation,
                                   int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
	{
		switch (section)
		{
		case 0:
			return tr("Database");
		default:
			return QVariant();
		}
	}

	return QVariant();
}

Qt::ItemFlags PgnDatabaseModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::ItemFlags(Qt::NoItemFlags);

	Qt::ItemFlags defaultFlags =
		Qt::ItemFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

	// database's name is editable
	if (index.column() == 0)
		return Qt::ItemFlags(defaultFlags | Qt::ItemIsEditable);

	return defaultFlags;
}

bool PgnDatabaseModel::setData(const QModelIndex& index, const QVariant& data,
                                int role)
{
	Q_UNUSED(role);

	if (!index.isValid())
		return false;

	// only database's name should be editable
	if (index.column() == 0)
	{
		PgnDatabase* db = m_gameDatabaseManager->databases().at(index.row());
		db->setDisplayName(data.toString());
		m_gameDatabaseManager->setModified(true);

		return true;
	}
	return false;
}
