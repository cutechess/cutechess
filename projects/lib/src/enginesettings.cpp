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

#include "enginesettings.h"

EngineSettings::EngineSettings()
	: m_whiteEvalPov(false)
{
}

const QStringList& EngineSettings::arguments() const
{
	return m_arguments;
}

const QStringList& EngineSettings::initStrings() const
{
	return m_initStrings;
}

const TimeControl& EngineSettings::timeControl() const
{
	return m_timeControl;
}

TimeControl& EngineSettings::timeControl()
{
	return m_timeControl;
}

const QVector<EngineSettings::CustomSetting>& EngineSettings::customSettings() const
{
	return m_customSettings;
}

bool EngineSettings::whiteEvalPov() const
{
	return m_whiteEvalPov;
}


void EngineSettings::addArgument(const QString& argument)
{
	m_arguments.append(argument);
}

void EngineSettings::addInitString(const QString& str)
{
	m_initStrings.append(str);
}

void EngineSettings::addCustomSetting(const QString& name, const QVariant& value)
{
	CustomSetting setting = {name, value};
	m_customSettings.append(setting);
}

void EngineSettings::setTimeControl(const TimeControl& timeControl)
{
	m_timeControl = timeControl;
}

void EngineSettings::setWhiteEvalPov(bool whiteEvalPov)
{
	m_whiteEvalPov = whiteEvalPov;
}
