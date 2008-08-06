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

