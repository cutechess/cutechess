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


TimeControlWidget::TimeControlWidget(QWidget* parent)
	: QWidget(parent),
	  ui(new Ui::TimeControlWidget),
	  m_tcMode(Tournament)
{
	ui->setupUi(this);

	connect(ui->m_tournamentRadio, &QRadioButton::toggled, [=](bool checked)
	{
		if (checked)
			setTimeControlMode(Tournament);
	});
	connect(ui->m_timePerMoveRadio, &QRadioButton::toggled, [=](bool checked)
	{
		if (checked)
			setTimeControlMode(TimePerMove);
	});
	connect(ui->m_infiniteRadio, &QRadioButton::toggled, [=](bool checked)
	{
		if (checked)
			setTimeControlMode(Infinite);
	});
	connect(ui->m_hourglassRadio, &QRadioButton::toggled, [=](bool checked)
	{
		if (checked)
			setTimeControlMode(Hourglass);
	});
}

void TimeControlWidget::init(const TimeControl& tc)
{
	if (!tc.isValid())
		return;

	if (tc.isInfinite())
	{
		ui->m_infiniteRadio->setChecked(true);
		setTimeControlMode(Infinite);
	}
	else if (tc.isHourglass())
	{
		ui->m_hourglassRadio->setChecked(true);
		setTime(tc.timePerTc());
		setTimeControlMode(Hourglass);
	}
	else if (tc.timePerMove() != 0)
	{
		ui->m_timePerMoveRadio->setChecked(true);
		setTime(tc.timePerMove());
		setTimeControlMode(TimePerMove);
	}
	else
	{
		ui->m_tournamentRadio->setChecked(true);
		ui->m_movesSpin->setValue(tc.movesPerTc());
		ui->m_incrementSpin->setValue(double(tc.timeIncrement()) / 1000.0);
		setTime(tc.timePerTc());
		setTimeControlMode(Tournament);
	}

	ui->m_nodesSpin->setValue(tc.nodeLimit());
	ui->m_pliesSpin->setValue(tc.plyLimit());
	ui->m_marginSpin->setValue(tc.expiryMargin());
}

TimeControlWidget::~TimeControlWidget()
{
	delete ui;
}

void TimeControlWidget::syncWith(TimeControlWidget* other)
{
	disconnect(other, &TimeControlWidget::timeControlModeChanged,
		   this, &TimeControlWidget::onOtherTimeControlModeChanged);

	connect(other->ui->m_tournamentRadio, &QRadioButton::toggled,
		this->ui->m_tournamentRadio, &QRadioButton::setChecked);
	connect(other->ui->m_timePerMoveRadio, &QRadioButton::toggled,
		this->ui->m_timePerMoveRadio, &QRadioButton::setChecked);
	connect(other->ui->m_infiniteRadio, &QRadioButton::toggled,
		this->ui->m_infiniteRadio, &QRadioButton::setChecked);
	connect(other->ui->m_hourglassRadio, &QRadioButton::toggled,
		this->ui->m_hourglassRadio, &QRadioButton::setChecked);
	connect(other->ui->m_movesSpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		this->ui->m_movesSpin, &QSpinBox::setValue);
	connect(other->ui->m_timeSpin, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
		this->ui->m_timeSpin, &QDoubleSpinBox::setValue);
	connect(other->ui->m_timeUnitCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
		this->ui->m_timeUnitCombo, &QComboBox::setCurrentIndex);
	connect(other->ui->m_incrementSpin, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
		this->ui->m_incrementSpin, &QDoubleSpinBox::setValue);
	connect(other->ui->m_nodesSpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		this->ui->m_nodesSpin, &QSpinBox::setValue);
	connect(other->ui->m_pliesSpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		this->ui->m_pliesSpin, &QSpinBox::setValue);
	connect(other->ui->m_marginSpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		this->ui->m_marginSpin, &QSpinBox::setValue);
}

void TimeControlWidget::unsyncWith(TimeControlWidget* other)
{
	disconnect(other->ui->m_tournamentRadio, &QRadioButton::toggled,
		   this->ui->m_tournamentRadio, &QRadioButton::setChecked);
	disconnect(other->ui->m_timePerMoveRadio, &QRadioButton::toggled,
		   this->ui->m_timePerMoveRadio, &QRadioButton::setChecked);
	disconnect(other->ui->m_infiniteRadio, &QRadioButton::toggled,
		   this->ui->m_infiniteRadio, &QRadioButton::setChecked);
	disconnect(other->ui->m_hourglassRadio, &QRadioButton::toggled,
		   this->ui->m_hourglassRadio, &QRadioButton::setChecked);
	disconnect(other->ui->m_movesSpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		   this->ui->m_movesSpin, &QSpinBox::setValue);
	disconnect(other->ui->m_timeSpin, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
		   this->ui->m_timeSpin, &QDoubleSpinBox::setValue);
	disconnect(other->ui->m_timeUnitCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
		   this->ui->m_timeUnitCombo, &QComboBox::setCurrentIndex);
	disconnect(other->ui->m_incrementSpin, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
		   this->ui->m_incrementSpin, &QDoubleSpinBox::setValue);
	disconnect(other->ui->m_nodesSpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		   this->ui->m_nodesSpin, &QSpinBox::setValue);
	disconnect(other->ui->m_pliesSpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		   this->ui->m_pliesSpin, &QSpinBox::setValue);
	disconnect(other->ui->m_marginSpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		   this->ui->m_marginSpin, &QSpinBox::setValue);

	connect(other, &TimeControlWidget::timeControlModeChanged,
		this, &TimeControlWidget::onOtherTimeControlModeChanged);
}

void TimeControlWidget::onOtherTimeControlModeChanged(Mode mode)
{
	if (mode == Hourglass || m_tcMode == Hourglass)
	{
		switch (mode)
		{
		case Tournament:
			ui->m_tournamentRadio->setChecked(true);
			break;
		case TimePerMove:
			ui->m_timePerMoveRadio->setChecked(true);
			break;
		case Infinite:
			ui->m_infiniteRadio->setChecked(true);
			break;
		case Hourglass:
			ui->m_hourglassRadio->setChecked(true);
			break;
		}
	}
}

void TimeControlWidget::setTimeControlMode(Mode mode)
{
	if (m_tcMode == mode)
		return;

	switch (mode)
	{
	case Tournament:
		onTournamentSelected();
		break;
	case TimePerMove:
		onTimePerMoveSelected();
		break;
	case Infinite:
		onInfiniteSelected();
		break;
	case Hourglass:
		onHourglassSelected();
		break;
	}

	m_tcMode = mode;
	emit timeControlModeChanged(mode);
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
