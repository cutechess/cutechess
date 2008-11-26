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

#include <QTextEdit>
#include <QTime>

#include "guilogger.h"

GuiLogger::GuiLogger(QTextEdit* logWidget)
{
	Q_ASSERT(logWidget);
	m_widget = logWidget;
}

void GuiLogger::log(QtMsgType type, const char *message)
{
	Q_UNUSED(type)

	QString msg;
	msg.append("[").append(QTime::currentTime().toString("HH:mm:ss")).append("] ");
	msg.append(QString::fromUtf8(message));
	m_widget->append(msg);
}

