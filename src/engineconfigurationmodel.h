/*
    This file is part of SloppyGUI.

    SloppyGUI is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SloppyGUI is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SloppyGUI.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ENGINE_CONFIGURATION_MODEL_H
#define ENGINE_CONFIGURATION_MODEL_H

#include <QAbstractListModel>
#include <QList>

#include "engineconfiguration.h"

class EngineConfigurationModel : public QAbstractListModel
{
	Q_OBJECT

	public:
		EngineConfigurationModel(QObject* parent = 0);
		EngineConfigurationModel(const QList<EngineConfiguration>& configurations,
		                         QObject* parent = 0);
		
		int rowCount(const QModelIndex& parent = QModelIndex()) const;
		int columnCount(const QModelIndex& parent = QModelIndex()) const;
		QVariant data(const QModelIndex& index, int role) const;
		QVariant headerData(int section, Qt::Orientation orientation,
		                    int role = Qt::DisplayRole) const;
		bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex());
		bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
		bool setData(const QModelIndex& index, const QVariant& value,
		             int role = Qt::EditRole);

		void addEngineConfiguration(const EngineConfiguration& configuration);
		QList<EngineConfiguration> engineConfigurations() const;
	
	private:
		QList<EngineConfiguration> m_configurations;

};

#endif // ENGINE_CONFIGURATION_MODEL_H

