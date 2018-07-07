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

#ifndef PGN_TAGS_MODEL_H
#define PGN_TAGS_MODEL_H

#include <QAbstractItemModel>
#include <QList>
#include <QPair>

/*!
 * \brief Supplies PGN tag information to views.
 */
class PgnTagsModel : public QAbstractItemModel
{
	Q_OBJECT

	public:
		/*! Constructs a model with the given \a parent. */
		PgnTagsModel(QObject* parent = nullptr);
		/*! Associates \a tags with this model. */
		void setTags(const QList<QPair<QString, QString> >& tags);

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
		void setTag(const QString& name, const QString& value);

	private:
		QList< QPair<QString, QString> > m_tags;
};

#endif // PGN_TAGS_MODEL_H
