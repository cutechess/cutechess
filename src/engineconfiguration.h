#ifndef ENGINE_CONFIGURATION_H
#define ENGINE_CONFIGURATION_H

#include <QString>

class EngineConfiguration
{
	public:

		enum ChessEngineProtocol
		{
			Xboard,
			UCI
		};

		EngineConfiguration();
		EngineConfiguration(const QString& name, const QString& command,
		                    ChessEngineProtocol protocol);
		~EngineConfiguration();

		void setName(const QString& name);
		void setCommand(const QString& command);
		void setProtocol(ChessEngineProtocol protocol);

		QString name() const;
		QString command() const;
		ChessEngineProtocol protocol() const;
	
	private:
		QString m_name;
		QString m_command;
		ChessEngineProtocol m_protocol;

};

#endif // ENGINE_CONFIGURATION_H

