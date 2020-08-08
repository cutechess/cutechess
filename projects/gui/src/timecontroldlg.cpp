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

#include "timecontroldlg.h"
#include "ui_timecontroldlg.h"

TimeControlDialog::TimeControlDialog(const TimeControl& tc1,
				     const TimeControl& tc2,
				     QWidget *parent)
	: QDialog(parent),
	  ui(new Ui::TimeControlDialog)
{
	ui->setupUi(this);

	// If tc2 has default value then use a simplified dialog
	if (tc2 == TimeControl())
	{
		ui->m_timeControlGroupBoxBlack->setEnabled(false);
		ui->m_timeControlGroupBoxWhite->setCheckable(false);
	}

	ui->m_timeControlGroupBoxBlack->setVisible(false);
	adjustSize();

	connect(ui->m_timeControlGroupBoxWhite, &QGroupBox::toggled, [=](bool checked)
	{
		ui->m_timeControlWidgetWhite->setEnabled(true);
		ui->m_timeControlGroupBoxBlack->setEnabled(!checked);
		ui->m_timeControlGroupBoxBlack->setVisible(!checked);
		ui->m_timeControlWidgetBlack->setEnabled(!checked);

		ui->m_timeControlGroupBoxWhite->setTitle(checked ? tr("Both Sides")
								 : tr("White"));
		adjustSize();
	});

	if (!tc1.isValid())
		return;

	ui->m_timeControlWidgetWhite->init(tc1);
	ui->m_timeControlWidgetBlack->init(tc2.isValid() ? tc2 : tc1);

	ui->m_timeControlGroupBoxWhite->setChecked(tc1 == tc2 || !tc2.isValid());
	ui->m_timeControlWidgetBlack->disableHourglassRadio();

	connect (ui->m_timeControlWidgetWhite, SIGNAL(hourglassToggled(bool)),
		 ui->m_timeControlWidgetBlack, SLOT(setHourglassMode(bool)));

	ui->m_timeControlWidgetBlack->setHourglassMode(tc1.isHourglass());
}

TimeControlDialog::~TimeControlDialog()
{
	delete ui;
}

TimeControl TimeControlDialog::timeControlWhite() const
{
	return ui->m_timeControlWidgetWhite->timeControl();
}

TimeControl TimeControlDialog::timeControlBlack() const
{
	if (ui->m_timeControlWidgetBlack->isEnabled())
		return ui->m_timeControlWidgetBlack->timeControl();
	else
		return ui->m_timeControlWidgetWhite->timeControl();
}

