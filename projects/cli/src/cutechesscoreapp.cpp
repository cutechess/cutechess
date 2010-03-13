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

#include <QSettings>
#include <QTime>
#include <cstdlib>
#include <cstdio>

#include <enginemanager.h>

#include "cutechesscoreapp.h"

EngineManager* CuteChessCoreApplication::s_engineManager = 0;

CuteChessCoreApplication::CuteChessCoreApplication(int& argc, char* argv[])
	: QCoreApplication(argc, argv)
{
	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

	QCoreApplication::setOrganizationName(QLatin1String("cutechess"));
	QCoreApplication::setOrganizationDomain(QLatin1String("cutechess.org"));
	QCoreApplication::setApplicationName(QLatin1String("cutechess"));

	// Use Ini format on all platforms
	QSettings::setDefaultFormat(QSettings::IniFormat);

	qInstallMsgHandler(CuteChessCoreApplication::messageHandler);
}

CuteChessCoreApplication::~CuteChessCoreApplication()
{
	delete s_engineManager;
	s_engineManager = 0;
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
	if (s_engineManager == 0)
		s_engineManager = new EngineManager();

	return s_engineManager;
}

CuteChessCoreApplication* CuteChessCoreApplication::instance()
{
	return static_cast<CuteChessCoreApplication*>(QCoreApplication::instance());
}
