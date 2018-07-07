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

#include "settingsdlg.h"
#include "ui_settingsdlg.h"
#include <QShowEvent>
#include <QSettings>
#include <QFileDialog>
#include <gamemanager.h>
#include "cutechessapp.h"

SettingsDialog::SettingsDialog(QWidget* parent)
	: QDialog(parent),
	  ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);

	readSettings();

	connect(ui->m_highlightLegalMovesCheck, &QCheckBox::toggled,
		this, [=](bool checked)
	{
		QSettings().setValue("ui/highlight_legal_moves", checked);
	});

	connect(ui->m_closeUnusedInitialTabCheck, &QCheckBox::toggled,
		this, [=](bool checked)
	{
		QSettings().setValue("ui/close_unused_initial_tab", checked);
	});

	connect(ui->m_useFullUserNameCheck, &QCheckBox::toggled,
		this, [=](bool checked)
	{
		QSettings().setValue("ui/use_full_user_name", checked);
	});

	connect(ui->m_playersSidesOnClocksCheck, &QCheckBox::toggled,
		this, [=](bool checked)
	{
		QSettings().setValue("ui/display_players_sides_on_clocks", checked);
	});


	connect(ui->m_humanCanPlayAfterTimeoutCheck, &QCheckBox::toggled,
		[=](bool checked)
	{
		QSettings().setValue("games/human_can_play_after_timeout",
				      checked);
	});


	connect(ui->m_concurrencySpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		this, [=](int value)
	{
		QSettings().setValue("tournament/concurrency", value);
		CuteChessApplication::instance()->gameManager()->setConcurrency(value);
	});

	connect(ui->m_siteEdit, &QLineEdit::textChanged,
		[=](const QString& site)
	{
		QSettings().setValue("pgn/site", site);
	});

	connect(ui->m_defaultPgnOutFileEdit, &QLineEdit::textChanged,
		[=](const QString& defaultPgnFile)
	{
		QSettings().setValue("games/default_pgn_output_file", defaultPgnFile);
	});

	connect(ui->m_tournamentDefaultPgnOutFileEdit, &QLineEdit::textChanged,
		[=](const QString& tourFile)
	{
		QSettings().setValue("tournament/default_pgn_output_file", tourFile);
	});

	connect(ui->m_tournamentDefaultEpdOutFileEdit, &QLineEdit::textChanged,
		[=](const QString& tourEpdFile)
	{
		QSettings().setValue("tournament/default_epd_output_file", tourEpdFile);
	});

	connect(ui->m_browseTbPathBtn, &QPushButton::clicked,
		this, &SettingsDialog::browseTbPath);
	connect(ui->m_defaultPgnOutFileBtn, &QPushButton::clicked,
		this, &SettingsDialog::browseDefaultPgnOutFile);
	connect(ui->m_tournamentDefaultPgnOutFileBtn, &QPushButton::clicked,
		this, &SettingsDialog::browseTournamentDefaultPgnOutFile);
	connect(ui->m_tournamentDefaultEpdOutFileBtn, &QPushButton::clicked,
		this, &SettingsDialog::browseTournamentDefaultEpdOutFile);

	ui->m_gameSettings->onHumanCountChanged(0);
	ui->m_gameSettings->enableSettingsUpdates();
	ui->m_tournamentSettings->enableSettingsUpdates();
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

void SettingsDialog::closeEvent(QCloseEvent* event)
{
	if (ui->m_engineManagementWidget->hasConfigChanged())
		ui->m_engineManagementWidget->saveConfig();

	QDialog::closeEvent(event);
}

void SettingsDialog::browseTbPath()
{
	auto dlg = new QFileDialog(
		this, tr("Choose Directory"),
		ui->m_tbPathEdit->text());
	dlg->setFileMode(QFileDialog::Directory);
	dlg->setOption(QFileDialog::ShowDirsOnly);
	dlg->setAttribute(Qt::WA_DeleteOnClose);
	dlg->setAcceptMode(QFileDialog::AcceptOpen);

	connect(dlg, &QFileDialog::fileSelected, [=](const QString& dir)
	{
		ui->m_tbPathEdit->setText(dir);
		QSettings().setValue("ui/tb_path", dir);
	});
	dlg->open();
}

void SettingsDialog::browseDefaultPgnOutFile()
{
	auto dlg = new QFileDialog(
		this, tr("Select PGN output file"),
		QString(),
		tr("Portable Game Notation (*.pgn)"));
	dlg->setAttribute(Qt::WA_DeleteOnClose);
	dlg->setAcceptMode(QFileDialog::AcceptSave);
	connect(dlg,
		&QFileDialog::fileSelected,
		ui->m_defaultPgnOutFileEdit,
		&QLineEdit::setText);
	dlg->open();
}

void SettingsDialog::browseTournamentDefaultPgnOutFile()
{
	auto dlg = new QFileDialog(
		this, tr("Select PGN output file"),
		QString(),
		tr("Portable Game Notation (*.pgn)"));
	dlg->setAttribute(Qt::WA_DeleteOnClose);
	dlg->setAcceptMode(QFileDialog::AcceptSave);
	connect(dlg,
		&QFileDialog::fileSelected,
		ui->m_tournamentDefaultPgnOutFileEdit,
		&QLineEdit::setText);
	dlg->open();
}

void SettingsDialog::browseTournamentDefaultEpdOutFile()
{
	auto dlg = new QFileDialog(
		this, tr("Select EPD output file"),
		QString(),
		tr("Extended Position Description (*.epd)"));
	dlg->setAttribute(Qt::WA_DeleteOnClose);
	dlg->setAcceptMode(QFileDialog::AcceptSave);
	connect(dlg,
		&QFileDialog::fileSelected,
		ui->m_tournamentDefaultEpdOutFileEdit,
		&QLineEdit::setText);
	dlg->open();
}

void SettingsDialog::readSettings()
{
	QSettings s;

	s.beginGroup("ui");
	ui->m_highlightLegalMovesCheck->setChecked(
		s.value("highlight_legal_moves", true).toBool());
	ui->m_closeUnusedInitialTabCheck->setChecked(
		s.value("close_unused_initial_tab", true).toBool());
	ui->m_useFullUserNameCheck->setChecked(
		s.value("use_full_user_name", true).toBool());
	ui->m_playersSidesOnClocksCheck->setChecked(
		s.value("display_players_sides_on_clocks", false).toBool());
	ui->m_tbPathEdit->setText(s.value("tb_path").toString());
	s.endGroup();

	s.beginGroup("pgn");
	ui->m_siteEdit->setText(s.value("site").toString());
	s.endGroup();

	s.beginGroup("games");
	ui->m_humanCanPlayAfterTimeoutCheck
		->setChecked(s.value("human_can_play_after_timeout", true).toBool());
	ui->m_defaultPgnOutFileEdit
		->setText(s.value("default_pgn_output_file").toString());
	s.endGroup();

	s.beginGroup("tournament");
	ui->m_tournamentDefaultPgnOutFileEdit
		->setText(s.value("default_pgn_output_file").toString());
	ui->m_tournamentDefaultEpdOutFileEdit
		->setText(s.value("default_epd_output_file").toString());
	ui->m_concurrencySpin->setValue(s.value("concurrency", 1).toInt());
	s.endGroup();
}
