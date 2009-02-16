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

#ifndef ENGINE_CONFIGURATION_MODEL_H
#define ENGINE_CONFIGURATION_MODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QStringList>

#include <engineconfiguration.h>

/*!
 * \brief The EngineConfigurationModel class represents a chess engine
 * configuration based model.
 *
 * %EngineConfigurationModel is the \e model part in the Model-View
 * architecture.
 *
 * The model contains the data providing view classes access to it. When the
 * model is updated (either from the view or by calling methods), all the
 * views are notified about the updates.
 *
 * %EngineConfigurationModel is a \e list based model; its items don't have
 * parent-child relationship.
 *
 * Almost all of the method calls are inherited from \e QAbstractItemModel. Refer
 * to QAbstractItemModel's documentation for better overview of each method.
*/
class EngineConfigurationModel : public QAbstractListModel
{
	Q_OBJECT

	public:
		/*!
		 * Creates an empty model.
		*/
		EngineConfigurationModel(QObject* parent = 0);
		/*!
		 * Creates a new model from a list of \a configurations.
		*/
		EngineConfigurationModel(const QList<EngineConfiguration>& configurations,
		                         QObject* parent = 0);
		
		/*!
		 * Returns the number of chess engine configurations in the model.
		 *
		 * \b Note: The \a parent parameter is always ignored in this model
		 * because items don't have a parent-child relationship.
		*/
		int rowCount(const QModelIndex& parent = QModelIndex()) const;
		/*!
		 * Returns the number of columns the model has.
		 *
		 * \b Note: This method should be used only by the view classes.
		 *
		 * \b Note: The \a parent parameter is always ignored in this model
		 * because items don't have a parent-child relationship.
		*/
		int columnCount(const QModelIndex& parent = QModelIndex()) const;

		/*!
		 * Returns the data stored under the given \a role for the item
		 * referred by \a index.
		 *
		 * If the data cannot be returned, an invalid QVariant is
		 * returned.
		*/
		QVariant data(const QModelIndex& index, int role) const;
		/*!
		 * Returns the data for the given \a role and \a section in the
		 * header with specified \a orientation.
		 *
		 * \b Note: This method should be used only by the view classes.
		*/
		QVariant headerData(int section, Qt::Orientation orientation,
		                    int role = Qt::DisplayRole) const;
		/*!
		 * Inserts \a count of empty chess engine configurations to the model
		 * before given \a row.
		 *
		 * Returns true if the rows were successfully inserted; otherwise
		 * return false.
		 *
		 * \b Note: The \a parent parameter is always ignored in
		 * this model because items don't have a parent-child relationship.
		*/
		bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex());
		/*!
		 * Removes \a count of chess engine configurations starting from given
		 * \a row.
		 *
		 * Returns true if the rows were successfully removed; otherwise
		 * returns false.
		 *
		 * \b Note: The \a parent parameter is always ignored in
		 * this model because items don't have a parent-child relationship.
		*/
		bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
		/*!
		 * Sets the \a role data for the item at \a index to \a value.
		 *
		 * Returns true if the data was updated successfully; otherwise
		 * returns false.
		*/
		bool setData(const QModelIndex& index, const QVariant& value,
		             int role = Qt::EditRole);

		/*!
		 * Adds new chess engine configuration to the model.
		 *
		 * This method is provided for convenience.
		*/
		void addConfiguration(const EngineConfiguration& configuration);
		/*! Returns the configuration in \a index. */
		EngineConfiguration configuration(const QModelIndex& index) const;
		/*!
		 * Returns all chess engine configurations in this model.
		*/
		QList<EngineConfiguration> configurations() const;
		/*!
		 * Sets the chess engine configuration in \a index to
		 * \a configuration.
		 *
		 * Returns true if the data was updated succesfully; otherwise
		 * return false.
		 *
		 * This method is provided for convenience.
		*/
		bool setConfiguration(const QModelIndex& index,
		                      const EngineConfiguration& configuration);
	
	private:
		static const QStringList m_headers;
		QList<EngineConfiguration> m_configurations;

};

#endif // ENGINE_CONFIGURATION_MODEL_H

