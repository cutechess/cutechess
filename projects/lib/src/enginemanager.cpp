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

#include <QSettings>

#include "enginemanager.h"

EngineManager::EngineManager(QObject* parent)
	: QObject(parent)
{
}

EngineManager::~EngineManager()
{
}

void EngineManager::addEngine(const EngineConfiguration& engine)
{
	m_engines << engine;

	emit engineAdded(m_engines.size() - 1);
}

void EngineManager::updateEngineAt(int index, const EngineConfiguration& engine)
{
	m_engines[index] = engine;

	emit engineUpdated(index);
}

void EngineManager::removeEngineAt(int index)
{
	emit engineAboutToBeRemoved(index);

	m_engines.removeAt(index);
}

QList<EngineConfiguration> EngineManager::engines() const
{
	return m_engines;
}

void EngineManager::setEngines(const QList<EngineConfiguration>& engines)
{
	m_engines = engines;

	emit enginesReset();
}

void EngineManager::loadEngines()
{
	QSettings settings;

	int size = settings.beginReadArray("engines");
	for (int i = 0; i < size; i++)
	{
		settings.setArrayIndex(i);

		EngineConfiguration config;
		config.setName(settings.value("name").toString());
		config.setCommand(settings.value("command").toString());
		config.setWorkingDirectory(
			settings.value("working_directory").toString());
		config.setProtocol(ChessEngine::Protocol(
			settings.value("protocol").toInt()));

		addEngine(config);
	}
	settings.endArray();
}

void EngineManager::saveEngines()
{
	QSettings settings;

	settings.beginWriteArray("engines");
	for (int i = 0; i < m_engines.size(); i++)
	{
		settings.setArrayIndex(i);
		settings.setValue("name", m_engines.at(i).name());
		settings.setValue("command", m_engines.at(i).command());
		settings.setValue("working_directory", m_engines.at(i).workingDirectory());
		settings.setValue("protocol", m_engines.at(i).protocol());
	}
	settings.endArray();

	// Make sure that the settings are flushed to disk now
	settings.sync();
}
