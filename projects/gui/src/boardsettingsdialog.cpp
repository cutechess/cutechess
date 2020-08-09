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

#include "ui_boardsettingsdlg.h"
#include "boardsettingsdialog.h"
#include <QShowEvent>
#include <QSettings>
#include <ui_boardsettingswidget.h>

BoardSettingsDialog::BoardSettingsDialog(QWidget* parent)
	: QDialog(parent),
	  ui(new Ui::BoardSettingsDlg)
{
	ui->setupUi(this);
	readSettings();

	connect(this, SIGNAL(rejected()), ui->m_boardSettingsWidget, SLOT(restore()));
}

BoardSettingsDialog::~BoardSettingsDialog()
{
	delete ui;
}

void BoardSettingsDialog::onContextChanged(const BoardSettings* boardSettings)
{
	ui->m_boardSettingsWidget->onContextChanged(boardSettings);
}

BoardSettings BoardSettingsDialog::boardSettings()
{
	return ui->m_boardSettingsWidget->boardSettings();
}

void BoardSettingsDialog::closeEvent(QCloseEvent* event)
{
/*
	if (ui->m_engineManagementWidget->hasConfigChanged())
		ui->m_engineManagementWidget->saveConfig();
*/
	QDialog::closeEvent(event);
}

void BoardSettingsDialog::readSettings()
{
	QSettings s;

	s.beginGroup("ui");
	s.endGroup();
}



