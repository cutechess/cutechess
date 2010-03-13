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

#include <enginemanager.h>
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
		EngineConfigurationModel(EngineManager* engineManager, QObject* parent = 0);
		
		// Inherited from QAbstractListModel
		int rowCount(const QModelIndex& parent = QModelIndex()) const;
		int columnCount(const QModelIndex& parent = QModelIndex()) const;
		QVariant data(const QModelIndex& index, int role) const;
		QVariant headerData(int section, Qt::Orientation orientation,
		                    int role = Qt::DisplayRole) const;

	private slots:
		void onEngineAdded(int index);
		void onEngineAboutToBeRemoved(int index);
		void onEngineUpdated(int index);
		void onEnginesReset();

	private:
		static const QStringList m_headers;
		EngineManager* m_engineManager;

};

#endif // ENGINE_CONFIGURATION_MODEL_H

