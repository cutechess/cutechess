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

#ifndef ENGINE_MANAGER_H
#define ENGINE_MANAGER_H

#include "engineconfiguration.h"

class LIB_EXPORT EngineManager : public QObject
{
	Q_OBJECT

	public:
		EngineManager(QObject* parent);
		~EngineManager();

		void addEngine(const EngineConfiguration& engine);
		void updateEngineAt(int index, const EngineConfiguration& engine);
		void removeEngineAt(int index);

		QList<EngineConfiguration> engines() const;
		void setEngines(const QList<EngineConfiguration>& engines);

	signals:
		void enginesReset();
		void engineAdded(int index);
		void engineRemoved(int index);
		void engineUpdated(int index);

	private:
		QList<EngineConfiguration> m_engines;

};

#endif // ENGINE_MANAGER_H
