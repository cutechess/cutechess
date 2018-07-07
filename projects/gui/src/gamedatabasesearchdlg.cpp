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

#include "gamedatabasesearchdlg.h"
#include "ui_gamedatabasesearchdlg.h"

GameDatabaseSearchDialog::GameDatabaseSearchDialog(QWidget* parent)
	: QDialog(parent, Qt::Window),
	  ui(new Ui::GameDatabaseSearchDialog)
{
	ui->setupUi(this);

	ui->m_maxDateEdit->setDate(QDate::currentDate());
	connect(ui->m_resultCombo, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onResultChanged(int)));
}

GameDatabaseSearchDialog::~GameDatabaseSearchDialog()
{
	delete ui;
}

void GameDatabaseSearchDialog::onResultChanged(int index)
{
	if (index == PgnGameFilter::AnyResult)
	{
		ui->m_invertResultCheck->setCheckState(Qt::Unchecked);
		ui->m_invertResultCheck->setEnabled(false);
	}
	else
		ui->m_invertResultCheck->setEnabled(true);
}

PgnGameFilter GameDatabaseSearchDialog::filter() const
{
	PgnGameFilter filter;

	filter.setEvent(ui->m_eventEdit->text());
	filter.setSite(ui->m_siteEdit->text());

	if (ui->m_minDateCheck->isChecked())
		filter.setMinDate(ui->m_minDateEdit->date());
	if (ui->m_maxDateCheck->isChecked())
		filter.setMaxDate(ui->m_maxDateEdit->date());

	filter.setMinRound(ui->m_minRoundSpin->value());
	filter.setMaxRound(ui->m_maxRoundSpin->value());

	filter.setResult(PgnGameFilter::Result(ui->m_resultCombo->currentIndex()));
	filter.setResultInverted(ui->m_invertResultCheck->isChecked());

	int side = ui->m_playerSideCombo->currentIndex() - 1;
	if (side == -1)
		side = Chess::Side::NoSide;
	filter.setPlayer(ui->m_playerEdit->text(), Chess::Side::Type(side));
	filter.setOpponent(ui->m_opponentEdit->text());

	return filter;
}
