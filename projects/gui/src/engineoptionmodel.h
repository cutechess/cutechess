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

#ifndef ENGINE_OPTION_MODEL_H
#define ENGINE_OPTION_MODEL_H

#include <QAbstractItemModel>

class EngineOption;

class EngineOptionModel : public QAbstractItemModel
{
	Q_OBJECT

	public:
		EngineOptionModel(QObject* parent = nullptr);
		EngineOptionModel(QList<EngineOption*> options, QObject* parent = nullptr);

		void setOptions(const QList<EngineOption*>& options);

		// Inherited from QAbstractItemModel
		virtual QModelIndex index(int row, int column,
					  const QModelIndex& parent = QModelIndex()) const;
		virtual QModelIndex parent(const QModelIndex& index) const;
		virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
		virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
		virtual QVariant data(const QModelIndex& index, int role) const;
		virtual QVariant headerData(int section, Qt::Orientation orientation,
					    int role = Qt::DisplayRole) const;
		virtual Qt::ItemFlags flags(const QModelIndex& index) const;
		virtual bool setData(const QModelIndex& index, const QVariant& value,
				     int role = Qt::EditRole);

	private:
		QList<EngineOption*> m_options;
};

#endif // ENGINE_OPTION_MODEL_H
