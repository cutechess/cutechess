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

#ifndef ENGINESETTINGS_H
#define ENGINESETTINGS_H

#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVector>
#include "timecontrol.h"

/*!
 * \brief Chess engine settings
 *
 * This class contains all the settings that are sent to a chess
 * engine during its initialization. The settings can be sent via
 * the ChessEngine::applySettings() method.
 *
 * \sa ChessEngine
 * \sa EngineConfiguration
 */
class LIB_EXPORT EngineSettings
{
	public:
		/*!
		 * \brief A setting that will only be sent to UCI
		 * engines.
		 */
		struct UciSetting
		{
			QString name;	//!< ucioption name
			QVariant value;	//!< ucioption value
		};

		/*!
		 * Creates a new empty EngineSettings object.
		 * The default values are never sent to an engine.
		 */
		EngineSettings();

		/*! Command line arguments. */
		const QStringList& arguments() const;
		/*! The number of cpus the engine should use. */
		int concurrency() const;
		/*! Path to endgame bitbases (eg. Scorpio format). */
		QString egbbPath() const;
		/*! Path to endgame tablebases (eg. Nalimov format). */
		QString egtbPath() const;
		/*! The engine's time control. */
		const TimeControl& timeControl() const;
		/*! The engine's time control. */
		TimeControl& timeControl();
		/*
		 * A list of UCI settings.
		 * \note Only settings supported by the engine are sent.
		 */
		const QVector<UciSetting>& uciSettings() const;
		/*! Returns true if evaluation is from white's perspective. */
		bool whiteEvalPov() const;

		/*! Adds a new command line argument. */
		void addArgument(const QString& argument);
		/*! Adds a new UCI setting. */
		void addUciSetting(const QString& name, const QVariant& value);
		/*! Sets the number of cpus to use. */
		void setConcurrency(int concurrency);
		/*! Sets the path to endgame bitbases. */
		void setEgbbPath(const QString& path);
		/*! Sets the path to endgame tablebases. */
		void setEgtbPath(const QString& path);
		/*! Sets the time control. */
		void setTimeControl(const TimeControl& timeControl);
		/*! Sets white evaluation point of view. */
		void setWhiteEvalPov(bool whiteEvalPov);

	private:
		TimeControl m_timeControl;
		int m_concurrency;
		QStringList m_arguments;
		QString m_egbbPath;
		QString m_egtbPath;
		QVector<UciSetting> m_uciSettings;
		bool m_whiteEvalPov;
};

#endif // ENGINESETTINGS_H
