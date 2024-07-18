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

#include "pairtimecontroldlg.h"
#include "ui_pairtimecontroldlg.h"

PairTimeControlDialog::PairTimeControlDialog(const TimeControl& tc1, const TimeControl& tc2,
                                             QWidget* parent)
    : QDialog(parent), ui(new Ui::PairTimeControlDialog)
{
	ui->setupUi(this);

	connect(ui->m_sameTc, &QToolButton::toggled, [=](bool checked)
	{
		if (checked)
			syncTimeControls();
		else
			unsyncTimeControls();
	});

	if (!tc1.isValid())
		return;

	ui->m_timeControlWidgetWhite->init(tc1);
	ui->m_timeControlWidgetBlack->init(tc2.isValid() ? tc2 : tc1);

	const bool inSync = tc1 == tc2;
	ui->m_sameTc->setChecked(inSync);

	if (inSync)
		syncTimeControls();
}

PairTimeControlDialog::~PairTimeControlDialog()
{
	delete ui;
}

void PairTimeControlDialog::syncTimeControls()
{
	// First, copy all settings from white to black
	const auto whiteTc(ui->m_timeControlWidgetWhite->timeControl());
	ui->m_timeControlWidgetBlack->init(whiteTc);

	// Then, propagate all inputs from white to black and vice versa
	ui->m_timeControlWidgetBlack->syncWith(ui->m_timeControlWidgetWhite);
	ui->m_timeControlWidgetWhite->syncWith(ui->m_timeControlWidgetBlack);
}

void PairTimeControlDialog::unsyncTimeControls()
{
	ui->m_timeControlWidgetBlack->unsyncWith(ui->m_timeControlWidgetWhite);
	ui->m_timeControlWidgetWhite->unsyncWith(ui->m_timeControlWidgetBlack);
}

TimeControl PairTimeControlDialog::timeControl(Chess::Side side) const
{
	if (side == Chess::Side::Black && ui->m_timeControlWidgetBlack->isEnabled())
		return ui->m_timeControlWidgetBlack->timeControl();
	return ui->m_timeControlWidgetWhite->timeControl();
}
