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

#include "enginebuilder.h"
#include <QDir>
#include <QtDebug>
#include "engineprocess.h"
#include "enginefactory.h"


EngineBuilder::EngineBuilder(const EngineConfiguration& config)
	: PlayerBuilder(config.name()),
	  m_config(config)
{
}

ChessPlayer* EngineBuilder::create(QObject* receiver,
				   const char* method,
				   QObject* parent) const
{
	QString path(QDir::currentPath());
	EngineProcess* process = new EngineProcess();

	QString workDir = m_config.workingDirectory();
	QString cmd = m_config.command();

	if (workDir.isEmpty())
	{
		process->setWorkingDirectory(QDir::tempPath());

		QFileInfo cmdInfo(cmd);
		if (cmdInfo.isFile())
			cmd = cmdInfo.absoluteFilePath();
	}
	else
	{
		// Make sure the path to the executable is resolved
		// in the engine's working directory
		if (!QDir::setCurrent(workDir))
		{
			qWarning() << "Invalid working directory:" << workDir;
			delete process;
			return 0;
		}
		process->setWorkingDirectory(QDir::currentPath());
	}

	if (!m_config.arguments().isEmpty())
		process->start(cmd, m_config.arguments());
	else
		process->start(cmd);
	bool ok = process->waitForStarted();

	if (!workDir.isEmpty())
		QDir::setCurrent(path);
	if (!ok)
	{
		qWarning() << "Cannot start engine" << m_config.command();
		delete process;
		return 0;
	}

	ChessEngine* engine = EngineFactory::create(m_config.protocol().toLower());
	if (!engine)
	{
		delete process;
		qWarning() << "Protocol does not exist" << m_config.protocol();
		return 0;
	}

	engine->setParent(parent);
	if (receiver != 0 && method != 0)
		QObject::connect(engine, SIGNAL(debugMessage(QString)),
				 receiver, method);
	engine->setDevice(process);
	engine->applyConfiguration(m_config);

	engine->start();
	return engine;
}
