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

#include "gamesettingswidget.h"
#include "ui_gamesettingswidget.h"
#include <QFileDialog>
#include <QSettings>

#include <board/boardfactory.h>
#include <board/syzygytablebase.h>
#include <engineconfiguration.h>
#include <openingsuite.h>
#include <polyglotbook.h>
#include "timecontroldlg.h"

GameSettingsWidget::GameSettingsWidget(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::GameSettingsWidget),
	  m_board(nullptr),
	  m_isValid(true)
{
	ui->setupUi(this);

	ui->m_variantCombo->addItems(Chess::BoardFactory::variants());
	connect(ui->m_variantCombo, SIGNAL(currentIndexChanged(QString)),
		this, SIGNAL(variantChanged(QString)));
	int index = ui->m_variantCombo->findText("standard");
	ui->m_variantCombo->setCurrentIndex(index);

	connect(ui->m_timeControlBtn, SIGNAL(clicked()),
		this, SLOT(showTimeControlDialog()));
	m_timeControl.setMovesPerTc(40);
	m_timeControl.setTimePerTc(300000);
	ui->m_timeControlBtn->setText(m_timeControl.toVerboseString());

	connect(ui->m_browseOpeningSuiteBtn, &QPushButton::clicked, this, [=]()
	{
		auto dlg = new QFileDialog(this, tr("Select opening suite"), QString(),
			tr("PGN/EPD files (*.pgn *.epd)"));
		connect(dlg, &QFileDialog::fileSelected,
			ui->m_openingSuiteEdit, &QLineEdit::setText);
		dlg->setAttribute(Qt::WA_DeleteOnClose);
		dlg->open();
	});

	connect(ui->m_browsePolyglotFile, &QPushButton::clicked, this, [=]()
	{
		auto dlg = new QFileDialog(this, tr("Select opening book"), QString(),
			tr("Polyglot files (*.bin)"));
		connect(dlg, &QFileDialog::fileSelected,
			ui->m_polyglotFileEdit, &QLineEdit::setText);
		dlg->setAttribute(Qt::WA_DeleteOnClose);
		dlg->open();
	});

	connect(ui->m_drawMoveNumberSpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
	{
		ui->m_drawMoveCountSpin->setEnabled(value > 0);
		ui->m_drawScoreSpin->setEnabled(value > 0);
	});

	connect(ui->m_resignMoveCountSpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int value)
	{
		ui->m_resignScoreSpin->setEnabled(value > 0);
	});

	m_defaultPalette = ui->m_fenEdit->palette();
	connect(ui->m_fenEdit, &QLineEdit::textChanged,
		this, &GameSettingsWidget::validateFen);
	connect(this, &GameSettingsWidget::variantChanged, [=]()
	{
		validateFen(ui->m_fenEdit->text());
	});

	connect(ui->m_openingSuiteEdit, &QLineEdit::textChanged, [=](const QString& str)
	{
		ui->m_openingSuiteDepthSpin->setEnabled(!str.isEmpty());
		ui->m_seqOrderRadio->setEnabled(!str.isEmpty());
		ui->m_randomOrderRadio->setEnabled(!str.isEmpty());
		ui->m_fenEdit->setEnabled(str.isEmpty());
	});

	connect(ui->m_polyglotFileEdit, &QLineEdit::textChanged, [=](const QString& str)
	{
		ui->m_polyglotDepthSpin->setEnabled(!str.isEmpty());
		ui->m_ramAccessRadio->setEnabled(!str.isEmpty());
		ui->m_diskAccessRadio->setEnabled(!str.isEmpty());
	});

	readSettings();
}

GameSettingsWidget::~GameSettingsWidget()
{
	delete m_board;
	delete ui;
}

bool GameSettingsWidget::isValid() const
{
	return m_isValid;
}

QString GameSettingsWidget::chessVariant() const
{
	return ui->m_variantCombo->currentText();
}

TimeControl GameSettingsWidget::timeControl() const
{
	return m_timeControl;
}

bool GameSettingsWidget::pondering() const
{
	return ui->m_ponderingCheck->isChecked();
}

GameAdjudicator GameSettingsWidget::adjudicator() const
{
	GameAdjudicator ret;
	ret.setDrawThreshold(ui->m_drawMoveNumberSpin->value(),
			     ui->m_drawMoveCountSpin->value(),
			     ui->m_drawScoreSpin->value());
	ret.setResignThreshold(ui->m_resignMoveCountSpin->value(),
			       -ui->m_resignScoreSpin->value());
	ret.setMaximumGameLength(ui->m_maxGameLengthSpin->value());
	ret.setTablebaseAdjudication(ui->m_tbCheck->isChecked());

	return ret;
}

OpeningSuite* GameSettingsWidget::openingSuite() const
{
	QString fen = ui->m_fenEdit->text();
	if (!fen.isEmpty())
		return new OpeningSuite(fen);

	QString file = ui->m_openingSuiteEdit->text();
	if (file.isEmpty())
		return nullptr;

	OpeningSuite::Format format = OpeningSuite::PgnFormat;
	if (file.toLower().endsWith(".epd"))
		format = OpeningSuite::EpdFormat;

	OpeningSuite::Order order = OpeningSuite::SequentialOrder;
	if (ui->m_randomOrderRadio->isChecked())
		order = OpeningSuite::RandomOrder;

	auto suite = new OpeningSuite(file, format, order, 0);
	if (!suite->initialize())
	{
		delete suite;
		return nullptr;
	}

	return suite;
}

int GameSettingsWidget::openingSuiteDepth() const
{
	return ui->m_openingSuiteDepthSpin->value();
}

OpeningBook* GameSettingsWidget::openingBook() const
{
	QString file = ui->m_polyglotFileEdit->text();
	if (file.isEmpty())
		return nullptr;

	auto mode = OpeningBook::Ram;
	if (ui->m_diskAccessRadio->isChecked())
		mode = OpeningBook::Disk;
	auto book = new PolyglotBook(mode);
	if (!book->read(file))
	{
		delete book;
		return nullptr;
	}

	return book;
}

int GameSettingsWidget::bookDepth() const
{
	return ui->m_polyglotDepthSpin->value();
}

void GameSettingsWidget::applyEngineConfiguration(EngineConfiguration* config)
{
	Q_ASSERT(config != nullptr);

	config->setPondering(pondering());
}

void GameSettingsWidget::readSettings()
{
	QSettings s;

	bool tbOk = false;
	QString tbPath = s.value("ui/tb_path").toString();
	if (!tbPath.isEmpty())
	{
		tbOk = SyzygyTablebase::initialize({ tbPath }) &&
		       SyzygyTablebase::tbAvailable(3);
		ui->m_tbCheck->setEnabled(tbOk);
	}

	s.beginGroup("games");

	ui->m_variantCombo->setCurrentText(s.value("variant").toString());
	m_timeControl.readSettings(&s);
	ui->m_timeControlBtn->setText(m_timeControl.toVerboseString());

	s.beginGroup("opening_suite");
	ui->m_fenEdit->setText(s.value("fen").toString());
	ui->m_openingSuiteEdit->setText(s.value("file").toString());
	ui->m_openingSuiteDepthSpin->setValue(s.value("depth", 1).toInt());
	if (s.value("random_order").toBool())
		ui->m_randomOrderRadio->setChecked(true);
	s.endGroup();

	s.beginGroup("opening_book");
	ui->m_polyglotFileEdit->setText(s.value("file").toString());
	ui->m_polyglotDepthSpin->setValue(s.value("depth", 10).toInt());
	if (s.value("disk_access").toBool())
		ui->m_diskAccessRadio->setChecked(true);
	s.endGroup();

	s.beginGroup("draw_adjudication");
	ui->m_drawMoveNumberSpin->setValue(s.value("move_number").toInt());
	ui->m_drawMoveCountSpin->setValue(s.value("move_count").toInt());
	ui->m_drawScoreSpin->setValue(s.value("score").toInt());
	s.endGroup();

	s.beginGroup("resign_adjudication");
	ui->m_resignMoveCountSpin->setValue(s.value("move_count").toInt());
	ui->m_resignScoreSpin->setValue(s.value("score").toInt());
	s.endGroup();

	s.beginGroup("game_length");
	ui->m_maxGameLengthSpin->setValue(s.value("max_moves", 0).toInt());
	s.endGroup();

	ui->m_tbCheck->setChecked(tbOk && s.value("use_tb").toBool());
	ui->m_ponderingCheck->setChecked(s.value("pondering").toBool());

	s.endGroup();
}

void GameSettingsWidget::enableSettingsUpdates()
{
	connect(ui->m_variantCombo, &QComboBox::currentTextChanged,
		[=](const QString& variant)
	{
		QSettings().setValue("games/variant", variant);
	});

	connect(this, &GameSettingsWidget::timeControlChanged, [=]()
	{
		QSettings s;
		s.beginGroup("games");
		m_timeControl.writeSettings(&s);
		s.endGroup();
	});

	connect(ui->m_tbCheck, &QCheckBox::toggled, [=](bool checked)
	{
		QSettings().setValue("games/use_tb", checked);
	});

	connect(ui->m_fenEdit, &QLineEdit::textChanged, [=](const QString& fen)
	{
		QSettings().setValue("games/opening_suite/fen", fen);
	});
	connect(ui->m_openingSuiteEdit, &QLineEdit::textChanged,
		[=](const QString& file)
	{
		QSettings().setValue("games/opening_suite/file", file);
	});
	connect(ui->m_openingSuiteDepthSpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		[=](int depth)
	{
		QSettings().setValue("games/opening_suite/depth", depth);
	});
	connect(ui->m_randomOrderRadio, &QRadioButton::toggled, [=](bool checked)
	{
		QSettings().setValue("games/opening_suite/random_order", checked);
	});

	connect(ui->m_polyglotFileEdit, &QLineEdit::textChanged,
		[=](const QString& file)
	{
		QSettings().setValue("games/opening_book/file", file);
	});
	connect(ui->m_polyglotDepthSpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		[=](int depth)
	{
		QSettings().setValue("games/opening_book/depth", depth);
	});
	connect(ui->m_diskAccessRadio, &QRadioButton::toggled, [=](bool checked)
	{
		QSettings().setValue("games/opening_book/disk_access", checked);
	});

	connect(ui->m_drawMoveNumberSpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		[=](int moveNumber)
	{
		QSettings().setValue("games/draw_adjudication/move_number", moveNumber);
	});
	connect(ui->m_drawMoveCountSpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		[=](int moveCount)
	{
		QSettings().setValue("games/draw_adjudication/move_count", moveCount);
	});
	connect(ui->m_drawScoreSpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		[=](int score)
	{
		QSettings().setValue("games/draw_adjudication/score", score);
	});

	connect(ui->m_resignMoveCountSpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		[=](int moveCount)
	{
		QSettings().setValue("games/resign_adjudication/move_count", moveCount);
	});
	connect(ui->m_resignScoreSpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		[=](int score)
	{
		QSettings().setValue("games/resign_adjudication/score", score);
	});

	connect(ui->m_maxGameLengthSpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		[=](int moveCount)
	{
		QSettings().setValue("games/game_length/max_moves", moveCount);
	});

	connect(ui->m_tbCheck, &QCheckBox::toggled, [=](bool checked)
	{
		QSettings().setValue("games/use_tb", checked);
	});

	connect(ui->m_ponderingCheck, &QCheckBox::toggled, [=](bool checked)
	{
		QSettings().setValue("games/pondering", checked);
	});
}

void GameSettingsWidget::onHumanCountChanged(int count)
{
	ui->m_drawAdjudicationGroup->setEnabled(count == 0);
	ui->m_resignAdjudicationGroup->setEnabled(count < 2);
	ui->m_gameLengthGroup->setEnabled(count < 2);
	ui->m_ponderingCheck->setEnabled(count < 2);
	ui->m_openingBookGroup->setEnabled(count < 2);
}

void GameSettingsWidget::validateFen(const QString& fen)
{
	ui->m_openingSuiteEdit->setEnabled(fen.isEmpty());
	ui->m_browseOpeningSuiteBtn->setEnabled(fen.isEmpty());
	ui->m_openingSuiteDepthSpin->setEnabled(fen.isEmpty());
	ui->m_seqOrderRadio->setEnabled(fen.isEmpty());
	ui->m_randomOrderRadio->setEnabled(fen.isEmpty());

	QString variant = chessVariant();
	if (!m_board || m_board->variant() != variant)
	{
		delete m_board;
		m_board = Chess::BoardFactory::create(variant);
	}
	if (!fen.isEmpty() && !m_board->setFenString(fen))
	{
		auto palette = ui->m_fenEdit->palette();
		palette.setColor(QPalette::Text, Qt::red);
		ui->m_fenEdit->setPalette(palette);
		m_isValid = false;
		emit statusChanged(false);
	}
	else
	{
		ui->m_fenEdit->setPalette(m_defaultPalette);
		m_isValid = true;
		emit statusChanged(true);
	}
}

void GameSettingsWidget::showTimeControlDialog()
{
	TimeControlDialog dlg(m_timeControl);
	if (dlg.exec() == QDialog::Accepted)
	{
		m_timeControl = dlg.timeControl();
		ui->m_timeControlBtn->setText(m_timeControl.toVerboseString());
		emit timeControlChanged();
	}
}
