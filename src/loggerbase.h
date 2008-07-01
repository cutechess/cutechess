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

#ifndef LOGGERBASE_H
#define LOGGERBASE_H

#include <QtGlobal>

/**
 * LoggerBase class defines a common interface for logger classes.
 *
 * @see LogManager
 * @see GuiLogger
 * @see StdOutLogger
*/
class LoggerBase
{
	public:
		LoggerBase() { }
		virtual ~LoggerBase() { }

		/**
		 * Logs a new messages.
		 * @param type Message's type.
		 * @param message The message.
		*/
		virtual void log(QtMsgType type, const char *message) = 0;
};

#endif // LOGGERBASE_H
