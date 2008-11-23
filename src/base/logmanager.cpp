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

#include "logmanager.h"
#include "loggerbase.h"
#include "stdoutlogger.h"

template<> LogManager* ManagerBase<LogManager>::instance = 0;

LogManager::LogManager()
{
	addLogger(new StdOutLogger());
	setLoggingLevel(QtDebugMsg);
}

LogManager::~LogManager()
{
	qDeleteAll(m_loggers);
}

void LogManager::messageHandler(QtMsgType type, const char *message)
{
	Manager::get()->getLogManager()->log(type, message);
}

void LogManager::restoreDefaultLogger()
{
	qDeleteAll(m_loggers);
	addLogger(new StdOutLogger());
}

void LogManager::addLogger(LoggerBase* logger)
{
	Q_ASSERT(logger);
	m_loggers.append(logger);
}

void LogManager::log(QtMsgType type, const char *message)
{
	if (type >= loggingLevel())
	{
		for (QList<LoggerBase*>::const_iterator i = m_loggers.begin(); i != m_loggers.end(); i++)
		{
			(*i)->log(type, message);
		}
	}
}

void LogManager::setLoggingLevel(QtMsgType level)
{
	m_loggingLevel = level;
}

QtMsgType LogManager::loggingLevel() const
{
	return m_loggingLevel;
}

