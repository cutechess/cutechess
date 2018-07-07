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

#include "plaintextlog.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

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
	auto saveAct = menu->addAction(tr("Save Log to File..."));
	connect(saveAct, &QAction::triggered, this, [=]()
	{
		auto dlg = new QFileDialog(this, tr("Save Log"), QString(),
			tr("Text Files (*.txt);;All Files (*.*)"));
		connect(dlg, &QFileDialog::fileSelected, this, &PlainTextLog::saveLogToFile);
		dlg->setAttribute(Qt::WA_DeleteOnClose);
		dlg->setAcceptMode(QFileDialog::AcceptSave);
		dlg->open();
	});

	menu->exec(event->globalPos());
	delete menu;
}

void PlainTextLog::saveLogToFile(const QString& fileName)
{
	if (fileName.isEmpty())
		return;

	QFile file(fileName);
	if (!file.open(QFile::WriteOnly | QFile::Text))
	{
		QFileInfo fileInfo(file);

		QMessageBox msgBox(this);
		msgBox.setIcon(QMessageBox::Warning);

		switch (file.error())
		{
			case QFile::OpenError:
			case QFile::PermissionsError:
				msgBox.setText(
					tr("The file \"%1\" could not be saved because "
					   "of insufficient privileges.")
					.arg(fileInfo.fileName()));

				msgBox.setInformativeText(
					tr("Try selecting a location where you have "
					   "the permissions to create files."));
			break;

			case QFile::TimeOutError:
				msgBox.setText(
					tr("The file \"%1\" could not be saved because "
					   "the operation timed out.")
					.arg(fileInfo.fileName()));

				msgBox.setInformativeText(
					tr("Try saving the file to a local or another "
					   "network disk."));
			break;

			default:
				msgBox.setText(tr("The file \"%1\" could not be saved.")
					.arg(fileInfo.fileName()));

				msgBox.setInformativeText(file.errorString());
			break;
		}
		msgBox.exec();
		return;
	}

	QTextStream out(&file);
	out << toPlainText();
}

