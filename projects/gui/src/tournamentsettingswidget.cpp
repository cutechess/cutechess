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

#include "tournamentsettingswidget.h"
#include "ui_tournamentsettingswidget.h"
#include <QSettings>

TournamentSettingsWidget::TournamentSettingsWidget(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::TournamentSettingsWidget)
{
	ui->setupUi(this);

	connect(ui->m_knockoutRadio, &QRadioButton::toggled, [=](bool checked)
	{
		ui->m_roundsSpin->setEnabled(!checked);
		ui->m_seedsSpin->setEnabled(checked);
	});

	readSettings();
}

TournamentSettingsWidget::~TournamentSettingsWidget()
{
	delete ui;
}

QString TournamentSettingsWidget::tournamentType() const
{
	if (ui->m_roundRobinRadio->isChecked())
		return "round-robin";
	else if (ui->m_gauntletRadio->isChecked())
		return "gauntlet";
	else if (ui->m_knockoutRadio->isChecked())
		return "knockout";

	Q_UNREACHABLE();
	return QString();
}

int TournamentSettingsWidget::gamesPerEncounter() const
{
	return ui->m_gamesPerEncounterSpin->value();
}

int TournamentSettingsWidget::rounds() const
{
	return ui->m_roundsSpin->value();
}

int TournamentSettingsWidget::seedCount() const
{
	return ui->m_seedsSpin->value();
}

int TournamentSettingsWidget::delayBetweenGames() const
{
	return int(ui->m_waitSpin->value() * 1000.0);
}

bool TournamentSettingsWidget::openingRepetition() const
{
	return ui->m_repeatCheck->isChecked();
}

bool TournamentSettingsWidget::engineRecovery() const
{
	return ui->m_recoverCheck->isChecked();
}

void TournamentSettingsWidget::readSettings()
{
	QSettings s;
	s.beginGroup("tournament");

	QString type = s.value("type").toString();
	if (type == "round-robin")
		ui->m_roundRobinRadio->setChecked(true);
	else if (type == "gauntlet")
		ui->m_gauntletRadio->setChecked(true);
	else if (type == "knockout")
		ui->m_knockoutRadio->setChecked(true);

	ui->m_seedsSpin->setValue(s.value("seeds", 0).toInt());
	ui->m_gamesPerEncounterSpin->setValue(s.value("games_per_encounter", 1).toInt());
	ui->m_roundsSpin->setValue(s.value("rounds", 1).toInt());
	ui->m_waitSpin->setValue(s.value("wait", 0.0).toDouble());

	ui->m_repeatCheck->setChecked(s.value("repeat").toBool());
	ui->m_recoverCheck->setChecked(s.value("recover").toBool());

	s.endGroup();
}

void TournamentSettingsWidget::enableSettingsUpdates()
{
	connect(ui->m_roundRobinRadio, &QRadioButton::toggled, [=](bool checked)
	{
		if (checked)
			QSettings().setValue("tournament/type", "round-robin");
	});
	connect(ui->m_gauntletRadio, &QRadioButton::toggled, [=](bool checked)
	{
		if (checked)
			QSettings().setValue("tournament/type", "gauntlet");
	});
	connect(ui->m_knockoutRadio, &QRadioButton::toggled, [=](bool checked)
	{
		if (checked)
			QSettings().setValue("tournament/type", "knockout");
	});

	connect(ui->m_seedsSpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		[=](int value)
	{
		QSettings().setValue("tournament/seeds", value);
	});
	connect(ui->m_gamesPerEncounterSpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		[=](int value)
	{
		QSettings().setValue("tournament/games_per_encounter", value);
	});
	connect(ui->m_roundsSpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		[=](int value)
	{
		QSettings().setValue("tournament/rounds", value);
	});
	connect(ui->m_waitSpin, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
		[=](double value)
	{
		QSettings().setValue("tournament/wait", value);
	});

	connect(ui->m_repeatCheck, &QCheckBox::toggled, [=](bool checked)
	{
		QSettings().setValue("tournament/repeat", checked);
	});
	connect(ui->m_recoverCheck, &QCheckBox::toggled, [=](bool checked)
	{
		QSettings().setValue("tournament/recover", checked);
	});
}
