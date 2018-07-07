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

#ifndef ENGINE_CONFIGURATION_MODEL_H
#define ENGINE_CONFIGURATION_MODEL_H

#include <QAbstractListModel>
class EngineManager;


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
		EngineConfigurationModel(EngineManager* engineManager, QObject* parent = nullptr);
		/*! Destroys the model. */
		virtual ~EngineConfigurationModel();

		/*!
		 * Returns the chosen chess variant.
		 *
		 * By default an empty string is returned, meaning that engines
		 * are not filtered by their supported variants.
		 */
		QString chessVariant() const;
		/*! Sets the chosen chess variant to \a variant. */
		void setChessVariant(const QString& variant);
		
		// Inherited from QAbstractListModel
		virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
		virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
		virtual QVariant data(const QModelIndex& index, int role) const;
		virtual QVariant headerData(int section, Qt::Orientation orientation,
					    int role = Qt::DisplayRole) const;

	private slots:
		void onEngineAdded(int index);
		void onEngineAboutToBeRemoved(int index);
		void onEngineUpdated(int index);
		void onEnginesReset();

	private:
		EngineManager* m_engineManager;
		QString m_chessVariant;
};

#endif // ENGINE_CONFIGURATION_MODEL_H

