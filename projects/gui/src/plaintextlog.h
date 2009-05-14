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

#ifndef PLAIN_TEXT_LOG_H
#define PLAIN_TEXT_LOG_H

#include <QPlainTextEdit>

class QContextMenuEvent;
class QAction;

class PlainTextLog : public QPlainTextEdit
{
	Q_OBJECT

	public:
		PlainTextLog(QWidget* parent = 0);
		PlainTextLog(const QString& text, QWidget* parent = 0);

	signals:
		void saveLogToFileRequest();

	protected:
		virtual void contextMenuEvent(QContextMenuEvent* event);

	protected slots:
		void saveLogToFile();

};

#endif // PLAIN_TEXT_LOG_H

