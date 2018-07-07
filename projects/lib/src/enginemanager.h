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

#ifndef ENGINE_MANAGER_H
#define ENGINE_MANAGER_H

#include <QSet>
#include "engineconfiguration.h"

/*!
 * \brief Manages chess engines and their configurations.
 *
 * \sa EngineConfiguration
 */
class LIB_EXPORT EngineManager : public QObject
{
	Q_OBJECT

	public:
		/*! Creates a new EngineManager. */
		EngineManager(QObject* parent = nullptr);
		virtual ~EngineManager();

		/*! Returns the number of available engines. */
		int engineCount() const;
		/*! Returns the engine at \a index. */
		EngineConfiguration engineAt(int index) const;

		/*! Adds \a engine to the list of available engines. */
		void addEngine(const EngineConfiguration& engine);
		/*! Updates the engine at \a index with \a engine. */
		void updateEngineAt(int index, const EngineConfiguration& engine);
		/*! Removes the engine at \a index. */
		void removeEngineAt(int index);

		/*! Returns the available engines. */
		QList<EngineConfiguration> engines() const;
		/*! Sets the available engines to \a engines. */
		void setEngines(const QList<EngineConfiguration>& engines);

		/*!
		 * Returns true if \a variant is supported by all engines;
		 * otherwise returns false.
		 */
		bool supportsVariant(const QString& variant) const;

		void loadEngines(const QString& fileName);
		void saveEngines(const QString& fileName);

		/*! Returns the names of all configured engines. */
		QSet<QString> engineNames() const;

	signals:
		/*!
		 * Emitted when all previously queried engine information is now
		 * invalid and has to be queried again.
		 */
		void enginesReset();
		/*! Emitted when an engine is added to \a index. */
		void engineAdded(int index);
		/*!
		 * Emitted when an engine at \a index is about to be removed.
		 * \note This signal is emitted before the engine is removed so
		 * the index position is still valid.
		 */
		void engineAboutToBeRemoved(int index);
		/*! Emitted when an engine is updated at \a index. */
		void engineUpdated(int index);

	private:
		QList<EngineConfiguration> m_engines;

};

#endif // ENGINE_MANAGER_H
