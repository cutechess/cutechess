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

/**
 * EngineConfigurationModel is used to hold EngineConfiguration objects
 * and presents them to different views.
 *
 * EngineConfigurationModel is the 'model' part in the Model-View architecture.
 * It holds the the data and provides methods to modify it.
 *
 * Almost all of the method calls are inherited from QAbstractItemModel. Refer
 * to QAbstractItemModel's documentation for better overview of each method.
*/
class EngineConfigurationModel : public QAbstractListModel
{
	Q_OBJECT

	public:
		/**
		 * Creates an empty EngineConfigurationModel object.
		 * @param parent Parent object.
		*/
		EngineConfigurationModel(QObject* parent = 0);
		/**
		 * Creates a new EngineConfigurationModel object.
		 * @param configurations Existing list of engine configurations to use.
		 * @param parent Parent object.
		*/
		EngineConfigurationModel(const QList<EngineConfiguration>& configurations,
		                         QObject* parent = 0);
		
		/**
		 * Returns the number of engine configurations in the model.
		 * @param parent Parent item. NOTE: This parameter is always ignored
		 * in this model because items don't have a parent-child relationship.
		 * @return Number of engine configurations (rows) in the model.
		*/
		int rowCount(const QModelIndex& parent = QModelIndex()) const;
		/**
		 * Returns the number of data entries a single item has.
		 * This method should be used only by the view classes.
		 * @param parent Parent. NOTE: This parameter is always ignored in
		 * this model because items don't have a parent-child relationship.
		 * @return Number of data entries in the model.
		*/
		int columnCount(const QModelIndex& parent = QModelIndex()) const;

		/**
		 * Returns information about a chess engine configuration with a given
		 * index and role.
		 * This method should be used only by the view classes.
		 * @param index The index of the item which is used to retrieve the
		 * information.
		 * @param role The role that is used to retrieve the information.
		 * @return Information about a chess engine configuration.
		*/
		QVariant data(const QModelIndex& index, int role) const;
		/**
		 * Returns header information from a given section, orientation and
		 * role.
		 * @param section Section that is used to retrieve the header
		 * information.
		 * @param orientation Orientation that is used to retrieve the header
		 * information.
		 * @param role Role that is used to retrieve the header information.
		 * @return Header information.
		*/
		QVariant headerData(int section, Qt::Orientation orientation,
		                    int role = Qt::DisplayRole) const;
		/**
		 * Inserts empty chess engine configurations to the model.
		 * @param row The location where the new configurations will be inserted.
		 * @param count The number of new configurations to be inserted.
		 * @param parent Parent item. NOTE: This parameter is always ignored in
		 * this model because items don't have a parent-child relationship.
		 * @return True if configurations were inserted successfully, false
		 * otherwise.
		*/
		bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex());
		/**
		 * Removes chess engine configurations from the model.
		 * @param row The location where the configurations will be removed.
		 * @param count The number of configurations to remove.
		 * @param parent Parent item. NOTE: This parameter is always ignored in
		 * this model because items don't have a parent-child relationship.
		 * @return True if configurations were remove successfully, false
		 * otherwise.
		*/
		bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
		/**
		 * Modifies chess engine configuration's information in the model.
		 * @param index The index of the item to be updated.
		 * @param value The new value.
		 * @param role Role to use.
		 * @return True if the configuration was updated successfully, false
		 * otherwise.
		*/
		bool setData(const QModelIndex& index, const QVariant& value,
		             int role = Qt::EditRole);

		/**
		 * Adds new chess engine configuration to the model.
		 * This method is provided for convenience.
		 * @param configuration New configuration to add.
		*/
		void addConfiguration(const EngineConfiguration& configuration);
		/**
		 * Returns all chess engine configurations this model has.
		 * @return All chess engine configurations in this model.
		*/
		QList<EngineConfiguration> configurations() const;
	
	private:
		QList<EngineConfiguration> m_configurations;

};

#endif // ENGINE_CONFIGURATION_MODEL_H

