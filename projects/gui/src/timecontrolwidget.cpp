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

#include "timecontrolwidget.h"
#include "ui_timecontrolwidget.h"


TimeControlWidget::TimeControlWidget(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::TimeControlWidget)
{
	ui->setupUi(this);
}

void TimeControlWidget::init(const TimeControl& tc)
{
	connect(ui->m_tournamentRadio, SIGNAL(clicked()),
		this, SLOT(onTournamentSelected()));
	connect(ui->m_timePerMoveRadio, SIGNAL(clicked()),
		this, SLOT(onTimePerMoveSelected()));
	connect(ui->m_infiniteRadio, SIGNAL(clicked()),
		this, SLOT(onInfiniteSelected()));
	connect(ui->m_hourglassRadio, SIGNAL(clicked()),
		this, SLOT(onHourglassSelected()));
	connect(ui->m_hourglassRadio, SIGNAL(toggled(bool)),
		this, SIGNAL(hourglassToggled(bool)));

	if (!tc.isValid())
		return;

	if (tc.isInfinite())
	{
		ui->m_infiniteRadio->setChecked(true);
		onInfiniteSelected();
	}
	else if (tc.isHourglass())
	{
		ui->m_hourglassRadio->setChecked(true);
		setTime(tc.timePerTc());
		onHourglassSelected();
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

TimeControlWidget::~TimeControlWidget()
{
	delete ui;
}

void TimeControlWidget::onTournamentSelected()
{
	ui->m_movesSpin->setEnabled(true);
	ui->m_timeSpin->setEnabled(true);
	ui->m_timeUnitCombo->setEnabled(true);
	ui->m_incrementSpin->setEnabled(true);
	ui->m_marginSpin->setEnabled(true);
}

void TimeControlWidget::onTimePerMoveSelected()
{
	ui->m_movesSpin->setEnabled(false);
	ui->m_timeSpin->setEnabled(true);
	ui->m_timeUnitCombo->setEnabled(true);
	ui->m_incrementSpin->setEnabled(false);
	ui->m_marginSpin->setEnabled(true);
}

void TimeControlWidget::onInfiniteSelected()
{
	ui->m_movesSpin->setEnabled(false);
	ui->m_timeSpin->setEnabled(false);
	ui->m_timeUnitCombo->setEnabled(false);
	ui->m_incrementSpin->setEnabled(false);
	ui->m_marginSpin->setEnabled(false);
}

void TimeControlWidget::onHourglassSelected()
{
	ui->m_movesSpin->setEnabled(false);
	ui->m_timeSpin->setEnabled(true);
	ui->m_timeUnitCombo->setEnabled(true);
	ui->m_incrementSpin->setEnabled(false);
	ui->m_marginSpin->setEnabled(true);
}

void TimeControlWidget::setHourglassMode(bool enabled)
{
	ui->m_groupBox->setEnabled(!enabled);

	if (enabled)
	{
		ui->m_hourglassRadio->setChecked(true);
		onHourglassSelected();
	}
	else
	{
		ui->m_tournamentRadio->setChecked(true);
		onTournamentSelected();
	}
}

void TimeControlWidget::disableHourglassRadio()
{
	ui->m_hourglassRadio->setEnabled(false);
	if (ui->m_hourglassRadio->isChecked())
		ui->m_groupBox->setEnabled(false);
}

int TimeControlWidget::timeToMs() const
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

void TimeControlWidget::setTime(int ms)
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

TimeControl TimeControlWidget::timeControl() const
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
	else if (ui->m_hourglassRadio->isChecked())
	{
		tc.setHourglass(true);
		tc.setTimePerTc(timeToMs());
		tc.setTimeIncrement(0);
	}

	tc.setNodeLimit(ui->m_nodesSpin->value());
	tc.setPlyLimit(ui->m_pliesSpin->value());
	tc.setExpiryMargin(ui->m_marginSpin->value());

	return tc;
}
