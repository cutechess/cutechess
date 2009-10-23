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

#ifndef ENGINE_CONFIGURATION_H
#define ENGINE_CONFIGURATION_H

#include <QString>
#include <QStringList>
#include <QVariant>
#include <QMap>

/*!
 * \brief The EngineConfiguration class defines a chess engine configuration.
 *
 * \sa EngineConfigurationModel
 */
class LIB_EXPORT EngineConfiguration
{
	public:

		/*! Creates an empty chess engine configuration. */
		EngineConfiguration();
		/*!
		 * Creates a new chess engine configuration with specified name,
		 * command and protocol settings.
		 */
		EngineConfiguration(const QString& name, const QString& command,
		                    int protocol);

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
		void setProtocol(int protocol);

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
		int protocol() const;

		/*! Returns the command line arguments sent to the engine. */
		QStringList arguments() const;
		/*! Sets the command line arguments sent to the engine. */
		void setArguments(const QStringList& arguments);
		/*! Adds new command line argument. */
		void addArgument(const QString& argument);

		/*! Returns the initialization strings sent to the engine. */
		QStringList initStrings() const;
		/*! Sets the initialization strings sent to the engine. */
		void setInitStrings(const QStringList& initStrings);
		/*! Adds new initialization string. */
		void addInitString(const QString& initString);

		/*! Returns the custom options sent to the engine. */
		QMap<QString, QVariant> customOptions() const;
		/*! Sets the custom options sent to the engine. */
		void setCustomOptions(const QMap<QString, QVariant> options);
		/*! Adds new custom option. */
		void addCustomOption(const QString& name, const QVariant& value);

		/*! Sets white evaluation point of view. */
		bool whiteEvalPov() const;
		/*! Returns true if evaluation is from white's point of view. */
		void setWhiteEvalPov(bool whiteEvalPov);

	private:
		QString m_name;
		QString m_command;
		QString m_workingDirectory;
		int m_protocol;
		QStringList m_arguments;
		QStringList m_initStrings;
		QMap<QString, QVariant> m_customOptions;
		bool m_whiteEvalPov;

};

#endif // ENGINE_CONFIGURATION_H
