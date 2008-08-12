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

#ifndef STDOUTLOGGER_H
#define STDOUTLOGGER_H

#include "loggerbase.h"

/*!
 * \brief The StdOutLogger class provides logging output to standard output and
 * standard error.
 *
 * Depending on the message's type the message is printed to different
 * output steam. Debug messages are printed to standard output and all
 * other types (warnings, critical message and fatal messages) are
 * printed to standard error.
*/
class StdOutLogger : public LoggerBase
{
	public:
		/*!
		 * Prints a new log message to standard output / error.
		 *
		 * The message's type defines the output steam. Only debug
		 * messages are printed to standard output, all other
		 * messages are printed to standard error.
		*/
		void log(QtMsgType type, const char *message);
};

#endif // STDOUTLOGGER_H

