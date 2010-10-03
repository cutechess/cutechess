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

#include "enginemanager.h"
#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <qjson/parser.h>
#include <qjson/serializer.h>
#include <QDebug>


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

void EngineManager::loadEngines(const QString& fileName)
{
	QFile input(fileName);
	QJson::Parser parser;
	bool ok = false;

	if (!QFile::exists(fileName))
		return;

	if (!input.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qWarning() << "cannot open engine configuration file:" << fileName;
		return;
	}

	const QVariantList engines = parser.parse(&input, &ok).toList();

	if (!ok)
	{
		qWarning() << "bad engine configuration file line" <<
			parser.errorLine() << "in" << fileName << ":"
				<< parser.errorString();
		return;
	}

	foreach(const QVariant& engine, engines)
		addEngine(EngineConfiguration(engine));
}

void EngineManager::saveEngines(const QString& fileName)
{
	QVariantList engines;
	foreach (const EngineConfiguration& config, m_engines)
		engines << config.toVariant();

	QJson::Serializer serializer;
	QFile output(fileName);

	if (!output.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		qWarning() << "cannot open engine configuration file:" << fileName;
		return;
	}

	QTextStream out(&output);
	out << serializer.serialize(engines);
}
