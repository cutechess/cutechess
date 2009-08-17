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

#include "gamedatabasemodel.h"

const QStringList GameDatabaseModel::m_headers = (QStringList() <<
	tr("White") << tr("Black") << tr("Event") << tr("Site") << tr("Result"));

GameDatabaseModel::GameDatabaseModel(QObject* parent)
	: QAbstractItemModel(parent)
{
}

QModelIndex GameDatabaseModel::index(int row, int column,
                                     const QModelIndex& parent) const
{
	Q_UNUSED(parent);

	if (!hasIndex(row, column, parent))
		return QModelIndex();

	return createIndex(row, column);
}

QModelIndex GameDatabaseModel::parent(const QModelIndex& index) const
{
	Q_UNUSED(index);

	return QModelIndex();
}

bool GameDatabaseModel::hasChildren(const QModelIndex& parent) const
{
	Q_UNUSED(parent);

	return false;
}

int GameDatabaseModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)

	return 0;
}

int GameDatabaseModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)

	return m_headers.count();
}

QVariant GameDatabaseModel::data(const QModelIndex& index, int role) const
{
	Q_UNUSED(index)
	Q_UNUSED(role)

	return QVariant();
}

QVariant GameDatabaseModel::headerData(int section, Qt::Orientation orientation,
                                       int role) const
{
	if (role == Qt::DisplayRole)
	{
		if (orientation == Qt::Horizontal)
			return m_headers.at(section);
		else
			return QString::number(section + 1);
	}

	return QVariant();
}
