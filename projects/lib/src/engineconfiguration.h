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

/*!
 * \brief The EngineConfiguration class defines a chess engine configuration.
 *
 * \sa EngineConfigurationModel
*/
class EngineConfiguration
{
	public:

		/*! The procol the engine uses for communication. */
		enum ChessEngineProtocol
		{
			/*! The Xboard protocol. */
			Xboard,
			/*! The UCI protocol. */
			UCI
		};

		/*! Creates an empty chess engine configuration. */
		EngineConfiguration();
		/*!
		 * Creates a new chess engine configuration with specified name,
		 * command and protocol settings.
		*/
		EngineConfiguration(const QString& name, const QString& command,
		                    ChessEngineProtocol protocol);

		/*!
		 * Sets the engine's name.
		 *
		 * \sa name()
		*/
		void setName(const QString& name);
		/*!
		 * Sets the command which is used to launch the engine.
		 *
		 * \sa command()
		*/
		void setCommand(const QString& command);
		/*!
		 * Sets the working directory the engine uses.
		 *
		 * \sa workingDirectory()
		*/
		void setWorkingDirectory(const QString& workingDir);
		/*!
		 * Sets the communication protocol the engine uses.
		 *
		 * \sa protocol()
		*/
		void setProtocol(ChessEngineProtocol protocol);

		/*!
		 * Returns the engine's name.
		 *
		 * \sa setName()
		*/
		QString name() const;
		/*!
		 * Returns the command which is used to launch the engine.
		 *
		 * \sa setCommand()
		*/
		QString command() const;
		/*!
		 * Returns the working directory the engine uses.
		 *
		 * \sa setWorkingDirectory()
		*/
		QString workingDirectory() const;
		/*!
		 * Returns the communication protocol the engine uses.
		 *
		 * \sa setProtocol()
		*/
		ChessEngineProtocol protocol() const;

	private:
		QString m_name;
		QString m_command;
		QString m_workingDirectory;
		ChessEngineProtocol m_protocol;

};

#endif // ENGINE_CONFIGURATION_H

