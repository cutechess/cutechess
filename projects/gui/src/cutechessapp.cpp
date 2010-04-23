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

#include "cutechessapp.h"
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QTime>
#include <QSettings>
#include <QFileInfo>
#include <enginemanager.h>


CuteChessApplication::CuteChessApplication(int& argc, char* argv[])
	: QApplication(argc, argv), m_engineManager(0)
{
	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

	QCoreApplication::setOrganizationName("cutechess");
	QCoreApplication::setOrganizationDomain("cutechess.org");
	QCoreApplication::setApplicationName("cutechess");

	// Use Ini format on all platforms
	QSettings::setDefaultFormat(QSettings::IniFormat);

	// Load the engines
	engineManager()->loadEngines(configPath() + QLatin1String("/engines.json"));
}

CuteChessApplication::~CuteChessApplication()
{
}

CuteChessApplication* CuteChessApplication::instance()
{
	return static_cast<CuteChessApplication*>(QApplication::instance());
}

QString CuteChessApplication::configPath()
{
	// We want to have the exact same config path in "gui" and
	// "cli" applications so that they can share resources
	QSettings settings;
	QFileInfo fi(settings.fileName());
	if (!QFile::exists(fi.absolutePath()))
	{
		QDir dir;
		dir.mkpath(fi.absolutePath());
	}

	return fi.absolutePath();
}

EngineManager* CuteChessApplication::engineManager()
{
	if (m_engineManager == 0)
		m_engineManager = new EngineManager(this);

	return m_engineManager;
}
