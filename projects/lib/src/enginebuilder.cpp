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

#include "enginebuilder.h"
#include <QDir>
#include "engineprocess.h"
#include "enginefactory.h"


EngineBuilder::EngineBuilder(const EngineConfiguration& config)
	: PlayerBuilder(config.name()),
	  m_config(config)
{
}

bool EngineBuilder::isHuman() const
{
	return false;
}

ChessPlayer* EngineBuilder::create(QObject* receiver,
				   const char* method,
				   QObject* parent,
				   QString* error) const
{
	QString workDir = m_config.workingDirectory();
	QString cmd = m_config.command().trimmed();
	QString stderrFile = m_config.stderrFile();

	if (cmd.isEmpty())
	{
		setError(error, tr("Empty engine command"));
		return nullptr;
	}

	if (!EngineFactory::protocols().contains(m_config.protocol()))
	{
		setError(error, tr("Unknown chess protocol: %1")
			 .arg(m_config.protocol()));
		return nullptr;
	}

	EngineProcess* process = new EngineProcess();

	if (workDir.isEmpty())
	{
		process->setWorkingDirectory(QDir::tempPath());

		QFileInfo cmdInfo(cmd);
		if (cmdInfo.isFile())
			cmd = cmdInfo.absoluteFilePath();
	}
	else
		process->setWorkingDirectory(workDir);

	if (!stderrFile.isEmpty())
		process->setStandardErrorFile(stderrFile, QIODevice::Append);

	process->start(cmd, m_config.arguments());

	bool ok = process->waitForStarted();
	if (!ok)
	{
		setError(error, tr("Cannot execute command: %1")
			 .arg(m_config.command()));
		delete process;
		return nullptr;
	}

	ChessEngine* engine = EngineFactory::create(m_config.protocol());
	Q_ASSERT(engine != nullptr);

	engine->setParent(parent);
	if (receiver != nullptr && method != nullptr)
		QObject::connect(engine, SIGNAL(debugMessage(QString)),
				 receiver, method);
	engine->setDevice(process);
	engine->applyConfiguration(m_config);

	engine->start();
	return engine;
}

void EngineBuilder::setError(QString* error, const QString& message) const
{
	QChar sep = error ? '\n' : ' ';
	QString str(tr("Cannot start engine %1:%2%3")
		    .arg(name()).arg(sep).arg(message));

	if (error != nullptr)
		*error = str;
	else
		qWarning("%s", qUtf8Printable(str));
}
