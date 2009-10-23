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
#include <QVariant>
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
		/*! \brief A custom setting. */
		struct CustomSetting
		{
			QString name;	//!< Setting name
			QVariant value;	//!< Setting value
		};

		/*!
		 * Creates a new empty EngineSettings object.
		 * The default values are never sent to an engine.
		 */
		EngineSettings();

		/*! The engine's time control. */
		const TimeControl& timeControl() const;
		/*! The engine's time control. */
		TimeControl& timeControl();
		/*! Sets the time control. */
		void setTimeControl(const TimeControl& timeControl);

	private:
		TimeControl m_timeControl;
};

#endif // ENGINESETTINGS_H
