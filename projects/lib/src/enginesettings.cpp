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
	: m_concurrency(0)
{
}

const QStringList& EngineSettings::arguments() const
{
	return m_arguments;
}

int EngineSettings::concurrency() const
{
	return m_concurrency;
}

QString EngineSettings::egbbPath() const
{
	return m_egbbPath;
}

QString EngineSettings::egtbPath() const
{
	return m_egtbPath;
}

const TimeControl& EngineSettings::timeControl() const
{
	return m_timeControl;
}

TimeControl& EngineSettings::timeControl()
{
	return m_timeControl;
}

const QVector<EngineSettings::UciSetting>& EngineSettings::uciSettings() const
{
	return m_uciSettings;
}


void EngineSettings::addArgument(const QString& argument)
{
	m_arguments.append(argument);
}

void EngineSettings::setConcurrency(int concurrency)
{
	Q_ASSERT(concurrency >= 0);
	m_concurrency = concurrency;
}

void EngineSettings::setEgbbPath(const QString& path)
{
	m_egbbPath = path;
}

void EngineSettings::setEgtbPath(const QString& path)
{
	m_egtbPath = path;
}

void EngineSettings::setTimeControl(const TimeControl& timeControl)
{
	m_timeControl = timeControl;
}

void EngineSettings::addUciSetting(const QString& name, const QVariant& value)
{
	UciSetting setting = {name, value};
	m_uciSettings.append(setting);
}
