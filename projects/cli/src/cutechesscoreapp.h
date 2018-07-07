/*
    This file is part of Cute Chess.
    Copyright (C) 2008-2018 Cute Chess authors

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

#ifndef CUTE_CHESS_CORE_APPLICATION_H
#define CUTE_CHESS_CORE_APPLICATION_H

#include <QCoreApplication>
#include <QMessageLogContext>

class EngineManager;
class GameManager;

class CuteChessCoreApplication : public QCoreApplication
{
	Q_OBJECT

	public:
		CuteChessCoreApplication(int& argc, char* argv[]);
		virtual ~CuteChessCoreApplication();

		QString configPath();
		EngineManager* engineManager();
		GameManager* gameManager();
		static CuteChessCoreApplication* instance();

		static void messageHandler(QtMsgType type,
					   const QMessageLogContext &context,
					   const QString &message);
	private:
		EngineManager* m_engineManager;
		GameManager* m_gameManager;
};

#endif  // CUTE_CHESS_CORE_APPLICATION_H
