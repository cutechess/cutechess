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

#include "cutechesscoreapp.h"
#include <QSettings>
#include <QTime>
#include <QFileInfo>
#include <enginemanager.h>
#include <cstdlib>
#include <cstdio>


CuteChessCoreApplication::CuteChessCoreApplication(int& argc, char* argv[])
	: QCoreApplication(argc, argv), m_engineManager(0)
{
	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

	QCoreApplication::setOrganizationName(QLatin1String("cutechess"));
	QCoreApplication::setOrganizationDomain(QLatin1String("cutechess.org"));
	QCoreApplication::setApplicationName(QLatin1String("cutechess"));

	// Use Ini format on all platforms
	QSettings::setDefaultFormat(QSettings::IniFormat);

	qInstallMsgHandler(CuteChessCoreApplication::messageHandler);

	// Load the engines
	// Note that we can't use QDesktopServices because of dependencies
	QSettings settings;
	QFileInfo fi(settings.fileName());
	engineManager()->loadEngines(fi.absolutePath() +
		QLatin1String("/engines.json"));
}

CuteChessCoreApplication::~CuteChessCoreApplication()
{
}

void CuteChessCoreApplication::messageHandler(QtMsgType type,
                                              const char* message)
{
	switch (type)
	{
		case QtDebugMsg:
			fprintf(stdout, "%s\n", message);
		break;

		case QtWarningMsg:
			fprintf(stderr, "Warning: %s\n", message);
		break;

		case QtCriticalMsg:
			fprintf(stderr, "Critical: %s\n", message);
		break;

		case QtFatalMsg:
			fprintf(stderr, "Fatal: %s\n", message);
			abort();
	}
}

EngineManager* CuteChessCoreApplication::engineManager()
{
	if (m_engineManager == 0)
		m_engineManager = new EngineManager(this);

	return m_engineManager;
}

CuteChessCoreApplication* CuteChessCoreApplication::instance()
{
	return static_cast<CuteChessCoreApplication*>(QCoreApplication::instance());
}
