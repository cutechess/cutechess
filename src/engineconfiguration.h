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

/**
 * EngineConfiguration is a class for storing a chess engine configuration.
 *
 * EngineConfiguration holds information about a single chess engine. This
 * information includes the engine's name, the engine's command and
 * protocol.
*/
class EngineConfiguration
{
	public:

		/** The procol the engine uses for communication. */
		enum ChessEngineProtocol
		{
			Xboard,
			UCI
		};

		/**
		 * Creates an empty EngineConfiguration object.
		*/
		EngineConfiguration();
		/**
		 * Creates a new EngineConfiguration object.
		 * @param name The engine's name.
		 * @param command The command which used to launch the engine.
		 * @param protocol The communication protocol the engine uses.
		*/
		EngineConfiguration(const QString& name, const QString& command,
		                    ChessEngineProtocol protocol);

		/**
		 * Sets the engine's name.
		 * @param name Name for the engine.
		*/
		void setName(const QString& name);
		/**
		 * Sets the command which is used to launch the engine.
		 * @param command Command for the engine.
		*/
		void setCommand(const QString& command);
		/**
		 * Sets the communication protocol the engine uses.
		 * @param protocol Protocol for the engine.
		*/
		void setProtocol(ChessEngineProtocol protocol);

		/**
		 * Returns the engine's name.
		 * @return Name of the engine.
		*/
		QString name() const;
		/**
		 * Returns the command which is used to launch the engine.
		 * @return Command of the engine.
		*/
		QString command() const;
		/**
		 * Returns the communication protocol the engine uses.
		 * @return Protocol of the engine.
		*/
		ChessEngineProtocol protocol() const;
	
	private:
		QString m_name;
		QString m_command;
		ChessEngineProtocol m_protocol;

};

#endif // ENGINE_CONFIGURATION_H

