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

#include "pgntagsmodel.h"

PgnTagsModel::PgnTagsModel(QObject* parent)
	: QAbstractItemModel(parent)
{
}

void PgnTagsModel::setTags(const QList< QPair<QString, QString> >& tags)
{
	beginResetModel();
	m_tags = tags;
	endResetModel();
}

QModelIndex PgnTagsModel::index(int row, int column,
                             const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	return createIndex(row, column);
}

QModelIndex PgnTagsModel::parent(const QModelIndex& index) const
{
	Q_UNUSED(index);

	return QModelIndex();
}

int PgnTagsModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;

	return m_tags.size();
}

int PgnTagsModel::columnCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;

	return 2;
}

QVariant PgnTagsModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		switch (index.column())
		{
		case 0:
			return m_tags.at(index.row()).first;
		case 1:
			return m_tags.at(index.row()).second;
		default:
			return QVariant();
		}
	}

	return QVariant();
}

QVariant PgnTagsModel::headerData(int section, Qt::Orientation orientation,
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
		default:
			return QVariant();
		}
	}

	return QVariant();
}

void PgnTagsModel::setTag(const QString& name, const QString& value)
{
	for (int i = 0; i < m_tags.size(); i++)
	{
		if (m_tags.at(i).first == name)
		{
			if (value.isEmpty())
			{
				beginRemoveRows(QModelIndex(), i, i);
				m_tags.removeAt(i);
				endRemoveRows();
			}
			else
			{
				m_tags[i].second = value;
				QModelIndex index = this->index(i, 1);
				emit dataChanged(index, index);
			}

			return;
		}
	}

	if (value.isEmpty())
		return;

	int pos = m_tags.size();
	beginInsertRows(QModelIndex(), pos, pos);
	m_tags.append(qMakePair(name, value));
	endInsertRows();
}
