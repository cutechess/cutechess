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

#include <board/boardfactory.h>
#include <engineconfiguration.h>
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

	return ret;
}

void GameSettingsWidget::applyEngineConfiguration(EngineConfiguration* config)
{
	Q_ASSERT(config != nullptr);

	config->setPondering(pondering());
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
