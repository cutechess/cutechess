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

#include "resultformatwidget.h"
#include "ui_resultformatwidget.h"
#include <gamemanager.h>
#include <roundrobintournament.h>

ResultFormatWidget::ResultFormatWidget(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::ResultFormatWidget)
{
	ui->setupUi(this);
}

void ResultFormatWidget::setFormats(const QMap<QString, QString>& formats,
				    const QString& currentFormat)
{
	for (auto it = formats.constBegin(); it != formats.constEnd(); ++it)
		ui->m_resultFormatCombo->addItem(it.key(), it.value());
	ui->m_resultFormatCombo->setCurrentIndex(-1);

	connect(ui->m_resultFormatCombo,
		static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
		this, [=](int)
	{
		const QVariant value = ui->m_resultFormatCombo->currentData();
		ui->m_resultFormatEdit->setText(value.toString());
	});

	const QString format(currentFormat);
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
}

ResultFormatWidget::~ResultFormatWidget()
{
	delete ui;
}

QString ResultFormatWidget::resultFormat()
{
	return ui->m_resultFormatEdit->text();
}

