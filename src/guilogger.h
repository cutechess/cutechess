/*
    This file is part of SloppyGUI.

    SloppyGUI is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SloppyGUI is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SloppyGUI.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GUILOGGER_H
#define GUILOGGER_H

#include "loggerbase.h"

class QTextEdit;

/**
 * GuiLogger class provides logging output to QTextEdit.
 *
 * Like in the LogManager's case, you should not use this class directly but
 * add GuiLogger to the list of loggers (LogManager::addLogger) and use Qt's
 * own debugging functions qDebug(), qWarning(), qCritical() and qFatal() to
 * do the logging. See Qt's documentation how to use these functions.
 *
 * In addition to the log message GuiLogger logs the time when the message
 * was added. This functionality is not configurable.
 *
 * The message's type does not affect the output; all messages are handled
 * similarly.
*/
class GuiLogger : public LoggerBase
{
	public:
		/**
		 * Creates a new GuiLogger object.
		 * @param logWidget The widget which will receive the log messages.
		*/
		GuiLogger(QTextEdit* logWidget);

		/**
		 * Appends a new log messages to the QTextEdit widget.
		 * Message's type does not affect the output.
		 * The current time is added before the log message.
		 * @param type Message's type.
		 * @param message The message.
		*/
		void log(QtMsgType type, const char *message);
	
	private:
		QTextEdit* m_widget;
};

#endif // GUILOGGER_H

