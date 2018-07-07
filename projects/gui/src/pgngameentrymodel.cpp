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

#include "pgngameentrymodel.h"
#include <QtConcurrentFilter>
#include <pgngameentry.h>


struct EntryContains
{
	EntryContains(const QList<const PgnGameEntry*>& entries,
		      const PgnGameFilter& filter)
		: m_entries(entries), m_filter(filter) { }

	typedef bool result_type;

	inline bool operator()(int index)
	{
		return m_entries.at(index)->match(m_filter);
	}

	const QList<const PgnGameEntry*>& m_entries;
	PgnGameFilter m_filter;
};


PgnGameEntryModel::PgnGameEntryModel(QObject* parent)
	: QAbstractItemModel(parent),
	  m_entryCount(0)
{
	connect(&m_watcher, SIGNAL(resultsReadyAt(int,int)),
		this, SLOT(onResultsReady()));
}

const PgnGameEntry* PgnGameEntryModel::entryAt(int row) const
{
	return m_entries.at(m_filtered.resultAt(row));
}

int PgnGameEntryModel::sourceIndex(int row) const
{
	return m_filtered.resultAt(row);
}

int PgnGameEntryModel::entryCount() const
{
	return m_filtered.resultCount();
}

void PgnGameEntryModel::setEntries(const QList<const PgnGameEntry*>& entries)
{
	m_watcher.cancel();
	m_watcher.waitForFinished();

	m_entries = entries;

	if (entries.size() > m_indexes.size())
	{
		m_indexes.reserve(entries.size());
		for (int i = m_indexes.size(); i < entries.size(); i++)
			m_indexes.append(i);
	}

	applyFilter(m_filter);
}

void PgnGameEntryModel::onResultsReady()
{
	if (m_entryCount < 1024)
		fetchMore(QModelIndex());
}

void PgnGameEntryModel::applyFilter(const PgnGameFilter& filter)
{
	beginResetModel();
	m_entryCount = 0;

	m_filtered = QtConcurrent::filtered(m_indexes.constBegin(),
					    m_indexes.constBegin() + m_entries.size(),
					    EntryContains(m_entries, filter));

	m_watcher.setFuture(m_filtered);
	endResetModel();
}

void PgnGameEntryModel::setFilter(const PgnGameFilter& filter)
{
	m_watcher.cancel();
	m_watcher.waitForFinished();

	m_filter = filter;
	applyFilter(filter);
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

	return qMin(m_entryCount, m_filtered.resultCount());
}

int PgnGameEntryModel::columnCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;

	return 7;
}

QVariant PgnGameEntryModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() >= m_filtered.resultCount() || index.row() < 0)
		return QVariant();

	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		PgnGameEntry::TagType tagType = PgnGameEntry::TagType(index.column());
		return entryAt(index.row())->tagValue(tagType);
	}

	return QVariant();
}

QVariant PgnGameEntryModel::headerData(int section, Qt::Orientation orientation,
				   int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
	{
		switch (section)
		{
		case 0:
			return tr("Event");
		case 1:
			return tr("Site");
		case 2:
			return tr("Date");
		case 3:
			return tr("Round");
		case 4:
			return tr("White");
		case 5:
			return tr("Black");
		case 6:
			return tr("Result");
		default:
			return QVariant();
		}
	}

	return QVariant();
}

bool PgnGameEntryModel::canFetchMore(const QModelIndex& parent) const
{
	Q_UNUSED(parent);

	return m_entryCount < m_filtered.resultCount();
}

void PgnGameEntryModel::fetchMore(const QModelIndex& parent)
{
	Q_UNUSED(parent);

	int remainder = m_filtered.resultCount() - m_entryCount;
	int entriesToFetch = qMin(1024, remainder);
	if (entriesToFetch <= 0)
		return;

	beginInsertRows(QModelIndex(), m_entryCount, m_entryCount + entriesToFetch - 1);
	m_entryCount += entriesToFetch;
	endInsertRows();
}
