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

#ifndef PGN_GAME_ENTRY_MODEL_H
#define PGN_GAME_ENTRY_MODEL_H

#include <QAbstractItemModel>
#include <QList>
#include <QFuture>
#include <QFutureWatcher>
#include <pgngamefilter.h>
class PgnGameEntry;

/*!
 * \brief Supplies PGN game entry information to views.
 */
class PgnGameEntryModel : public QAbstractItemModel
{
	Q_OBJECT

	public:
		/*! Constructs a PGN game entry model with the given \a parent. */
		PgnGameEntryModel(QObject* parent = nullptr);

		/*! Returns the PGN entry at \a row. */
		const PgnGameEntry* entryAt(int row) const;
		/*!
		 * Returns the total number of PGN game entries matching the
		 * current filter.
		 *
		 * \note Unlike rowCount() this method also includes entries that are
		 * not yet fetched into the model.
		 */
		int entryCount() const;
		/*!
		 * Returns the index in the source data that corresponds to
		 * \a row in the model.
		 */
		int sourceIndex(int row) const;
		/*! Associates a list of PGN game entries with this model. */
		void setEntries(const QList<const PgnGameEntry*>& entries);

		// Inherited from QAbstractItemModel
		virtual QModelIndex index(int row, int column,
					  const QModelIndex& parent = QModelIndex()) const;
		virtual QModelIndex parent(const QModelIndex& index) const;
		virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
		virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
		virtual QVariant data(const QModelIndex& index, int role) const;
		virtual QVariant headerData(int section, Qt::Orientation orientation,
					    int role = Qt::DisplayRole) const;

	public slots:
		/*! Sets the filter for filtering the contents of the database. */
		void setFilter(const PgnGameFilter& filter);

	protected:
		// Inherited from QAbstractItemModel
		virtual bool canFetchMore(const QModelIndex& parent) const;
		virtual void fetchMore(const QModelIndex& parent);

	private slots:
		void onResultsReady();

	private:
		void applyFilter(const PgnGameFilter& filter);

		QList<const PgnGameEntry*> m_entries;
		QVector<int> m_indexes;
		int m_entryCount;
		QFuture<int> m_filtered;
		QFutureWatcher<int> m_watcher;
		PgnGameFilter m_filter;
};

#endif // PGN_GAME_ENTRY_MODEL_H
