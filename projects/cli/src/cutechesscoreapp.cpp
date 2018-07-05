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
#include <QDir>
#include <mersenne.h>
#include <enginemanager.h>
#include <gamemanager.h>
#include <board/syzygytablebase.h>
#include <cstdlib>
#include <cstdio>


CuteChessCoreApplication::CuteChessCoreApplication(int& argc, char* argv[])
	: QCoreApplication(argc, argv),
	  m_engineManager(nullptr),
	  m_gameManager(nullptr)
{
	Mersenne::initialize(QTime(0,0,0).msecsTo(QTime::currentTime()));

	QCoreApplication::setOrganizationName(QLatin1String("cutechess"));
	QCoreApplication::setOrganizationDomain(QLatin1String("cutechess.com"));
	QCoreApplication::setApplicationName(QLatin1String("cutechess"));

	// Use Ini format on all platforms
	QSettings::setDefaultFormat(QSettings::IniFormat);

	qInstallMessageHandler(CuteChessCoreApplication::messageHandler);

	// Load the engines
	QString configFile("engines.json");
	if (!QFile::exists(configFile))
		configFile = configPath() + "/" + configFile;
	engineManager()->loadEngines(configFile);
}

CuteChessCoreApplication::~CuteChessCoreApplication()
{
}

void CuteChessCoreApplication::messageHandler(QtMsgType type,
					      const QMessageLogContext &context,
					      const QString &message)
{
	QByteArray msg = message.toLocal8Bit();
	switch (type)
	{
	case QtInfoMsg:
		fprintf(stdout, "%s\n", msg.constData());
		break;
	case QtDebugMsg:
		fprintf(stdout, "Debug: %s\n", msg.constData());
		break;
	case QtWarningMsg:
		fprintf(stderr, "Warning: %s\n", msg.constData());
		break;
	case QtCriticalMsg:
		fprintf(stderr, "Critical: %s (%s:%u, %s)\n", msg.constData(),
			context.file, context.line, context.function);
		break;
	case QtFatalMsg:
		fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", msg.constData(),
			context.file, context.line, context.function);
		abort();
	}
}

QString CuteChessCoreApplication::configPath()
{
	// QDesktopServices requires QtGui
	QSettings settings;
	QFileInfo fi(settings.fileName());
	QDir dir(fi.absolutePath());

	if (!dir.exists())
		dir.mkpath(fi.absolutePath());

	return fi.absolutePath();
}

EngineManager* CuteChessCoreApplication::engineManager()
{
	if (m_engineManager == nullptr)
		m_engineManager = new EngineManager(this);

	return m_engineManager;
}

GameManager* CuteChessCoreApplication::gameManager()
{
	if (m_gameManager == nullptr)
		m_gameManager = new GameManager(this);

	return m_gameManager;
}

CuteChessCoreApplication* CuteChessCoreApplication::instance()
{
	return static_cast<CuteChessCoreApplication*>(QCoreApplication::instance());
}
