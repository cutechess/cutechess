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

#include "plaintextlog.h"
#include <QContextMenuEvent>
#include <QMenu>


PlainTextLog::PlainTextLog(QWidget* parent)
	: QPlainTextEdit(parent)
{
	setReadOnly(true);
}

PlainTextLog::PlainTextLog(const QString& text, QWidget* parent)
	: QPlainTextEdit(text, parent)
{
	setReadOnly(true);
}

void PlainTextLog::contextMenuEvent(QContextMenuEvent* event)
{
	QMenu* menu = createStandardContextMenu();

	menu->addSeparator();
	menu->addAction(tr("Clear Log"), this, SLOT(clear()));

	menu->addSeparator();
	menu->addAction(tr("Save Log to File..."), this, SIGNAL(saveLogToFileRequest()));

	menu->exec(event->globalPos());
	delete menu;
}

