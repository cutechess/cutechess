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

#include <base/loggerbase.h>

class QTextEdit;

/*!
 * \brief The GuiLogger class provides logging output to QTextEdit.
 *
 * In addition to the log message %GuiLogger logs the time when the message
 * was added. This functionality is not configurable.
 *
 * The message's type does not affect the output; all messages are handled
 * similarly.
*/
class GuiLogger : public LoggerBase
{
	public:
		/*!
		 * Creates a new GuiLogger with \a logWidget as the target widget.
		*/
		GuiLogger(QTextEdit* logWidget);

		/*!
		 * Appends a new log messages to the target widget.
		 *
		 * Message's type does not affect the output.
		 *
		 * The current time is added before the log message.
		*/
		void log(QtMsgType type, const char *message);
	
	private:
		QTextEdit* m_widget;
};

#endif // GUILOGGER_H

