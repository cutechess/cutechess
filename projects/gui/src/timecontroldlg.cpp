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


TimeControlDialog::TimeControlDialog(const TimeControl& tc,
				     QWidget *parent)
	: QDialog(parent),
	  ui(new Ui::TimeControlDialog)
{
	ui->setupUi(this);

	connect(ui->m_tournamentRadio, SIGNAL(clicked()),
		this, SLOT(onTournamentSelected()));
	connect(ui->m_timePerMoveRadio, SIGNAL(clicked()),
		this, SLOT(onTimePerMoveSelected()));
	connect(ui->m_infiniteRadio, SIGNAL(clicked()),
		this, SLOT(onInfiniteSelected()));

	if (!tc.isValid())
		return;

	if (tc.isInfinite())
	{
		ui->m_infiniteRadio->setChecked(true);
		onInfiniteSelected();
	}
	else if (tc.timePerMove() != 0)
	{
		ui->m_timePerMoveRadio->setChecked(true);
		setTime(tc.timePerMove());
		onTimePerMoveSelected();
	}
	else
	{
		ui->m_tournamentRadio->setChecked(true);
		ui->m_movesSpin->setValue(tc.movesPerTc());
		ui->m_incrementSpin->setValue(double(tc.timeIncrement()) / 1000.0);
		setTime(tc.timePerTc());
		onTournamentSelected();
	}

	ui->m_nodesSpin->setValue(tc.nodeLimit());
	ui->m_pliesSpin->setValue(tc.plyLimit());
	ui->m_marginSpin->setValue(tc.expiryMargin());
}

TimeControlDialog::~TimeControlDialog()
{
	delete ui;
}

void TimeControlDialog::onTournamentSelected()
{
	ui->m_movesSpin->setEnabled(true);
	ui->m_timeSpin->setEnabled(true);
	ui->m_timeUnitCombo->setEnabled(true);
	ui->m_incrementSpin->setEnabled(true);
	ui->m_marginSpin->setEnabled(true);
}

void TimeControlDialog::onTimePerMoveSelected()
{
	ui->m_movesSpin->setEnabled(false);
	ui->m_timeSpin->setEnabled(true);
	ui->m_timeUnitCombo->setEnabled(true);
	ui->m_incrementSpin->setEnabled(false);
	ui->m_marginSpin->setEnabled(true);
}

void TimeControlDialog::onInfiniteSelected()
{
	ui->m_movesSpin->setEnabled(false);
	ui->m_timeSpin->setEnabled(false);
	ui->m_timeUnitCombo->setEnabled(false);
	ui->m_incrementSpin->setEnabled(false);
	ui->m_marginSpin->setEnabled(false);
}

int TimeControlDialog::timeToMs() const
{
	switch (ui->m_timeUnitCombo->currentIndex())
	{
	case Seconds:
		return ui->m_timeSpin->value() * 1000.0;
	case Minutes:
		return ui->m_timeSpin->value() * 60000.0;
	case Hours:
		return ui->m_timeSpin->value() * 3600000.0;
	default:
		return 0;
	}
}

void TimeControlDialog::setTime(int ms)
{
	Q_ASSERT(ms >= 0);

	if (ms == 0 || ms % 60000 != 0)
	{
		ui->m_timeUnitCombo->setCurrentIndex(Seconds);
		ui->m_timeSpin->setValue(double(ms) / 1000.0);
	}
	else if (ms % 3600000 != 0)
	{
		ui->m_timeUnitCombo->setCurrentIndex(Minutes);
		ui->m_timeSpin->setValue(double(ms) / 60000.0);
	}
	else
	{
		ui->m_timeUnitCombo->setCurrentIndex(Hours);
		ui->m_timeSpin->setValue(double(ms) / 3600000.0);
	}
}

TimeControl TimeControlDialog::timeControl() const
{
	TimeControl tc;
	if (ui->m_infiniteRadio->isChecked())
		tc.setInfinity(true);
	else if (ui->m_timePerMoveRadio->isChecked())
		tc.setTimePerMove(timeToMs());
	else if (ui->m_tournamentRadio->isChecked())
	{
		tc.setMovesPerTc(ui->m_movesSpin->value());
		tc.setTimePerTc(timeToMs());
		tc.setTimeIncrement(ui->m_incrementSpin->value() * 1000.0);
	}

	tc.setNodeLimit(ui->m_nodesSpin->value());
	tc.setPlyLimit(ui->m_pliesSpin->value());
	tc.setExpiryMargin(ui->m_marginSpin->value());

	return tc;
}
