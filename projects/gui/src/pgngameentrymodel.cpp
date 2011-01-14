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

#include "pgngameentrymodel.h"

const QStringList PgnGameEntryModel::s_headers = (QStringList() <<
	tr("White") << tr("Black") << tr("Event") << tr("Site") << tr("Result"));

PgnGameEntryModel::PgnGameEntryModel(QObject* parent)
	: QAbstractItemModel(parent)
{
}

void PgnGameEntryModel::setEntries(const QList<PgnGameEntry>& entries)
{
	m_entries = entries;
	reset();
}

QModelIndex PgnGameEntryModel::index(int row, int column,
                                 const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	return createIndex(row, column);
}

QModelIndex PgnGameEntryModel::parent(const QModelIndex& index) const
{
	Q_UNUSED(index);

	return QModelIndex();
}

int PgnGameEntryModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;

	return m_entries.count();
}

int PgnGameEntryModel::columnCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;

	return s_headers.count();
}

QVariant PgnGameEntryModel::data(const QModelIndex& index, int role) const
{
	if (index.isValid() && role == Qt::DisplayRole)
	{
		const PgnGameEntry entry = m_entries.at(index.row());

		switch (index.column())
		{
			case 0:
				return entry.white();

			case 1:
				return entry.black();

			case 2:
				return entry.event();

			case 3:
				return entry.site();

			case 4:
				return entry.result().toShortString();

			default:
				return QVariant();
		}
	}
	return QVariant();
}

QVariant PgnGameEntryModel::headerData(int section, Qt::Orientation orientation,
                                   int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
		return s_headers.at(section);

	return QVariant();
}
