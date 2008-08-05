#include "engineconfiguration.h"

EngineConfiguration::EngineConfiguration()
{
	m_protocol = Xboard;
}

EngineConfiguration::EngineConfiguration(const QString& name,
                                         const QString& command,
                                         ChessEngineProtocol protocol)
{
	m_name = name;
	m_command = command;
	m_protocol = protocol;
}

EngineConfiguration::~EngineConfiguration()
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

QString EngineConfiguration::name() const
{
	return m_name;
}

QString EngineConfiguration::command() const
{
	return m_command;
}

EngineConfiguration::ChessEngineProtocol EngineConfiguration::protocol() const
{
	return m_protocol;
}

