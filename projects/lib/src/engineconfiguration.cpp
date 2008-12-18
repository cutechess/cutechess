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

#include "engineconfiguration.h"

EngineConfiguration::EngineConfiguration()
	: m_protocol(Xboard)
{

}

EngineConfiguration::EngineConfiguration(const QString& name,
                                         const QString& command,
                                         ChessEngineProtocol protocol)
	: m_name(name),
	  m_command(command),
	  m_protocol(protocol)
{

}

void EngineConfiguration::setName(const QString& name)
{
	m_name = name;
}

void EngineConfiguration::setCommand(const QString& command)
{
	m_command = command;
}

void EngineConfiguration::setProtocol(EngineConfiguration::ChessEngineProtocol protocol)
{
	m_protocol = protocol;
}

void EngineConfiguration::setWorkingDirectory(const QString& workingDir)
{
	m_workingDirectory = workingDir;
}

QString EngineConfiguration::name() const
{
	return m_name;
}

QString EngineConfiguration::command() const
{
	return m_command;
}

QString EngineConfiguration::workingDirectory() const
{
	return m_workingDirectory;
}

EngineConfiguration::ChessEngineProtocol EngineConfiguration::protocol() const
{
	return m_protocol;
}
