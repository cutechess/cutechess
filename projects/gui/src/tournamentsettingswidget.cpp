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
#include "gamemanager.h"
#include "roundrobintournament.h"

TournamentSettingsWidget::TournamentSettingsWidget(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::TournamentSettingsWidget)
{
	ui->setupUi(this);

	connect(ui->m_tournamentTypeGroup, QOverload<QAbstractButton *, bool>::of(&QButtonGroup::buttonToggled),
		[=](QAbstractButton *button, bool checked)
	{
		if (!checked)
			return;

		bool roundsEnabled = true;
		bool seedsEnabled = false;
		if (button == ui->m_knockoutRadio)
		{
			roundsEnabled = false;
			seedsEnabled = true;
		}
		else if (button == ui->m_gauntletRadio)
		{
			seedsEnabled = true;
		}
		ui->m_seedsSpin->setEnabled(seedsEnabled);
		ui->m_roundsSpin->setEnabled(roundsEnabled);

		emit tournamentTypeChanged(tournamentType());
	});

	// Update repeats after rounds
	connect(ui->m_roundsSpin, QOverload<int>::of(&QSpinBox::valueChanged),
		ui->m_repeatSpin, &GameRepetitionSpinBox::setRounds);
	// Update repeats after games per encounter
	connect(ui->m_gamesPerEncounterSpin, QOverload<int>::of(&QSpinBox::valueChanged),
		ui->m_repeatSpin, &GameRepetitionSpinBox::setGamesPerEncounter);
	// Update repeats after tournament type changed
	connect(this, &TournamentSettingsWidget::tournamentTypeChanged,
		ui->m_repeatSpin, &GameRepetitionSpinBox::setTournamentType);

	GameManager gameManager;
	RoundRobinTournament tournament(&gameManager);
	const auto map = tournament.namedResultFormats();
	for (auto it = map.constBegin(); it != map.constEnd(); ++it)
		ui->m_resultFormatCombo->addItem(it.key(), it.value());
	ui->m_resultFormatCombo->setCurrentIndex(-1);

	connect(ui->m_resultFormatCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
		this, [=]()
	{
		const QVariant value = ui->m_resultFormatCombo->currentData();
		ui->m_resultFormatEdit->setText(value.toString());
	});

	readSettings();
}

TournamentSettingsWidget::~TournamentSettingsWidget()
{
	delete ui;
}

QString TournamentSettingsWidget::tournamentType() const
{
	auto btn = ui->m_tournamentTypeGroup->checkedButton();

	if (btn == ui->m_roundRobinRadio)
		return "round-robin";
	else if (btn == ui->m_gauntletRadio)
		return "gauntlet";
	else if (btn == ui->m_knockoutRadio)
		return "knockout";
	else if (btn == ui->m_pyramidRadio)
		return "pyramid";

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

int TournamentSettingsWidget::openingRepetitions() const
{
	return ui->m_repeatSpin->value();
}

bool TournamentSettingsWidget::engineRecovery() const
{
	return ui->m_recoverCheck->isChecked();
}

bool TournamentSettingsWidget::savingOfUnfinishedGames() const
{
	return ui->m_saveUnfinishedGamesCheck->isChecked();
}

bool TournamentSettingsWidget::swappingSides() const
{
	return ui->m_swapCheck->isChecked();
}

QString TournamentSettingsWidget::resultFormat() const
{
	return ui->m_resultFormatEdit->text();
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
	else if (type == "pyramid")
		ui->m_pyramidRadio->setChecked(true);
	ui->m_repeatSpin->setTournamentType(type);

	ui->m_seedsSpin->setValue(s.value("seeds", 0).toInt());
	ui->m_gamesPerEncounterSpin->setValue(s.value("games_per_encounter", 1).toInt());
	ui->m_roundsSpin->setValue(s.value("rounds", 1).toInt());
	ui->m_waitSpin->setValue(s.value("wait", 0.0).toDouble());
	ui->m_repeatSpin->setValue(s.value("repeats").toInt());
	ui->m_recoverCheck->setChecked(s.value("recover").toBool());
	ui->m_saveUnfinishedGamesCheck->setChecked(
		s.value("save_unfinished_games", true).toBool());
	ui->m_swapCheck->setChecked(s.value("swap_sides", true).toBool());

	QString format = s.value("result_format").toString();
	if (format.isEmpty())
	{
		int defaultIdx = ui->m_resultFormatCombo->findText("default");
		ui->m_resultFormatCombo->setCurrentIndex(defaultIdx);
	}
	else
	{
		ui->m_resultFormatCombo->addItem("setting", QVariant(format));
		int index = ui->m_resultFormatCombo->findData(format);
		ui->m_resultFormatCombo->setCurrentIndex(index);
	}

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
	connect(ui->m_pyramidRadio, &QRadioButton::toggled, [=](bool checked)
	{
		if (checked)
			QSettings().setValue("tournament/type", "pyramid");
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

	connect(ui->m_repeatSpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		[=](int value)
	{
		QSettings().setValue("tournament/repeats", value);
	});
	connect(ui->m_recoverCheck, &QCheckBox::toggled, [=](bool checked)
	{
		QSettings().setValue("tournament/recover", checked);
	});
	connect(ui->m_saveUnfinishedGamesCheck, &QCheckBox::toggled, [=](bool checked)
	{
		QSettings().setValue("tournament/save_unfinished_games", checked);
	});
	connect(ui->m_swapCheck, &QCheckBox::toggled, [=](bool checked)
	{
		QSettings().setValue("tournament/swap_sides", checked);
	});
	connect(ui->m_resultFormatEdit, &QLineEdit::textChanged, [=](const QString &text)
	{
		QSettings().setValue("tournament/result_format", text);
	});
}
