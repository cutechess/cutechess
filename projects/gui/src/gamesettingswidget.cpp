/*
    This file is part of Cute Chess.

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

#include <board/boardfactory.h>
#include <engineconfiguration.h>
#include <openingsuite.h>
#include <polyglotbook.h>
#include "timecontroldlg.h"

GameSettingsWidget::GameSettingsWidget(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::GameSettingsWidget)
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

	connect(ui->m_openingSuiteEdit, &QLineEdit::textChanged, [=](const QString& str)
	{
		ui->m_openingSuiteDepthSpin->setEnabled(!str.isEmpty());
		ui->m_seqOrderRadio->setEnabled(!str.isEmpty());
		ui->m_randomOrderRadio->setEnabled(!str.isEmpty());
	});

	connect(ui->m_polyglotFileEdit, &QLineEdit::textChanged, [=](const QString& str)
	{
		ui->m_polyglotDepthSpin->setEnabled(!str.isEmpty());
		ui->m_ramAccessRadio->setEnabled(!str.isEmpty());
		ui->m_diskAccessRadio->setEnabled(!str.isEmpty());
	});
}

GameSettingsWidget::~GameSettingsWidget()
{
	delete ui;
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

	return ret;
}

OpeningSuite* GameSettingsWidget::openingSuite() const
{
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

void GameSettingsWidget::onHumanCountChanged(int count)
{
	ui->m_drawAdjudicationGroup->setEnabled(count == 0);
	ui->m_resignAdjudicationGroup->setEnabled(count < 2);
	ui->m_ponderingCheck->setEnabled(count < 2);
	ui->m_openingBookGroup->setEnabled(count < 2);
}

void GameSettingsWidget::showTimeControlDialog()
{
	TimeControlDialog dlg(m_timeControl);
	if (dlg.exec() == QDialog::Accepted)
	{
		m_timeControl = dlg.timeControl();
		ui->m_timeControlBtn->setText(m_timeControl.toVerboseString());
	}
}
