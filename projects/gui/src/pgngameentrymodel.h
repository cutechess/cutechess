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

#ifndef PGN_GAME_ENTRY_MODEL_H
#define PGN_GAME_ENTRY_MODEL_H

#include <QAbstractItemModel>
#include <QStringList>
#include <QList>
#include <QFuture>
#include <QFutureWatcher>
#include <pgngameentry.h>
class PgnGameFilter;

/*!
 * \brief Supplies PGN game entry information to views.
 */
class PgnGameEntryModel : public QAbstractItemModel
{
	Q_OBJECT

	public:
		/*! Constructs a PGN game entry model with the given \a parent. */
		PgnGameEntryModel(QObject* parent = 0);

		/*! Returns the PGN entry at \a row. */
		PgnGameEntry entryAt(int row) const;
		/*! Associates a list of PGN game entris with this model. */
		void setEntries(const QList<PgnGameEntry>& entries);

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

		static const QStringList s_headers;

		QList<PgnGameEntry> m_entries;
		int m_entryCount;
		QFuture<PgnGameEntry> m_filtered;
		QFutureWatcher<PgnGameEntry> m_watcher;
};

#endif // PGN_GAME_ENTRY_MODEL_H
