/*
    This file is part of Cute Chess.
    Copyright (C) 2008-2019 Cute Chess authors

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

#include "boardsettingswidget.h"
#include "ui_boardsettingswidget.h"
#include "boardview/boardsettings.h"
#include <QSettings>
#include <QDirIterator>
#include <QTimer>

#include "gameviewer.h"
#include "pgngame.h"
#include "boardview/boardscene.h"
#include <qcolordialog.h>



BoardSettingsWidget::BoardSettingsWidget(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::BoardSettingsWidget),
	  m_updateRequested(false)
{
	ui->setupUi(this);

	m_gameViewer = new GameViewer(Qt::Horizontal, nullptr, false, true);
	ui->m_viewerLayout->insertWidget(ui->m_viewerLayout->count(), m_gameViewer);
	readSettings();
	m_gameViewer->setGame(new PgnGame());

	connect(ui->m_pieceSetCombo, SIGNAL(currentIndexChanged(int)), this,
		SLOT(updatePreview()));
	connect(ui->m_pieceSizeSpin, SIGNAL(valueChanged(int)), this,
		SLOT(updatePreview()));
	connect(ui->m_lightSquareColorEdit, SIGNAL(textChanged(const QString&)), this,
		SLOT(updatePreview()));
	connect(ui->m_darkSquareColorEdit, SIGNAL(textChanged(const QString&)), this,
		SLOT(updatePreview()));
	connect(ui->m_borderColorEdit, SIGNAL(textChanged(const QString&)), this,
		SLOT(updatePreview()));
	connect(ui->m_coordTextColorEdit, SIGNAL(textChanged(const QString&)), this,
		SLOT(updatePreview()));

	connect(ui->m_lightSquareColorBtn, SIGNAL(clicked()), this, SLOT(onLightSquareColorBtnClicked()));
	connect(ui->m_darkSquareColorBtn, SIGNAL(clicked()), this, SLOT(onDarkSquareColorBtnClicked()));
	connect(ui->m_borderColorBtn, SIGNAL(clicked()), this, SLOT(onBorderColorBtnClicked()));
	connect(ui->m_coordTextColorBtn, SIGNAL(clicked()), this, SLOT(onCoordTextColorBtnClicked()));
}

BoardSettingsWidget::~BoardSettingsWidget()
{
	delete m_gameViewer;
	delete ui;
}

void BoardSettingsWidget::readPieceSetEntries()
{
	ui->m_pieceSetCombo->clear();

	const QString prefix(":/pieces/");
	QDirIterator it(prefix, QDirIterator::Subdirectories);
	while (it.hasNext()) {
		QString s = it.next();
		QString text = it.fileName().remove(".svg");
		if (!it.fileInfo().isDir())
			ui->m_pieceSetCombo->addItem(text, s);
	}
}

void BoardSettingsWidget::readPieceSet(QVariant value)
{
	if (!value.isValid())
		value = QSettings().value("ui/board/piece_set");

	int index = ui->m_pieceSetCombo->findData(value);

	if (index > -1)
		ui->m_pieceSetCombo->setCurrentIndex(index);
	else
		ui->m_pieceSetCombo->setCurrentText("default");
}

void BoardSettingsWidget::readSettings()
{
	// settings from file
	QSettings s;

	readPieceSetEntries();
	readPieceSet();

	QVariant value = QSettings().value("ui/board/piece_size_factor");
	ui->m_pieceSizeSpin->setValue(value.toInt());
	value = QSettings().value("ui/board/light_square_color");
	ui->m_lightSquareColorEdit->setText(value.toString());
	value = QSettings().value("ui/board/dark_square_color");
	ui->m_darkSquareColorEdit->setText(value.toString());
	value = QSettings().value("ui/board/border_color");
	ui->m_borderColorEdit->setText(value.toString());
	value = QSettings().value("ui/board/coord_text_color");
	ui->m_coordTextColorEdit->setText(value.toString());;

}

void BoardSettingsWidget::enableSettingsUpdates()
{
	connect(ui->m_pieceSetCombo, &QComboBox::currentTextChanged,
		[=](const QString&)
	{
		QSettings().setValue("ui/board/piece_set",
				     ui->m_pieceSetCombo->currentData());
	});

	connect(ui->m_pieceSizeSpin,
		static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		[=](int value)
	{
		QSettings().setValue("ui/board/piece_size_factor", value);
	});

	connect(ui->m_lightSquareColorEdit, &QLineEdit::textChanged,
		[=](const QString& s)
	{
		QSettings().setValue("ui/board/light_square_color", s);
	});

	connect(ui->m_darkSquareColorEdit, &QLineEdit::textChanged,
		[=](const QString& s)
	{
		QSettings().setValue("ui/board/dark_square_color", s);
	});

	connect(ui->m_borderColorEdit, &QLineEdit::textChanged,
		[=](const QString& s)
	{
		QSettings().setValue("ui/board/border_color", s);
	});

	connect(ui->m_coordTextColorEdit, &QLineEdit::textChanged,
		[=](const QString& s)
	{
		QSettings().setValue("ui/board/coord_text_color", s);
	});

}

void BoardSettingsWidget::readBoardSettings(const BoardSettings* boardSettings)
{
	// settings from instance of BoardSettings
	if (boardSettings == nullptr)
		return;

	if (boardSettings->initialized())
	{
		QVariant pieceSet = boardSettings->value("b/piece_set");
		readPieceSet(pieceSet);

		QVariant value = boardSettings->value("b/piece_size_factor");
		ui->m_pieceSizeSpin->setValue(value.toInt());
		value = boardSettings->value("b/light_square_color");
		ui->m_lightSquareColorEdit->setText(value.toString());
		value = boardSettings->value("b/dark_square_color");
		ui->m_darkSquareColorEdit->setText(value.toString());
		value = boardSettings->value("b/border_color");
		ui->m_borderColorEdit->setText(value.toString());
		value = boardSettings->value("b/coord_text_color");
		ui->m_coordTextColorEdit->setText(value.toString());;
	}
}

BoardSettings BoardSettingsWidget::boardSettings()
{
	BoardSettings settings;
	settings.set("b/piece_set", ui->m_pieceSetCombo->currentData());
	settings.set("b/piece_size_factor", ui->m_pieceSizeSpin->value());
	settings.set("b/light_square_color", ui->m_lightSquareColorEdit->text());
	settings.set("b/dark_square_color", ui->m_darkSquareColorEdit->text());
	settings.set("b/border_color", ui->m_borderColorEdit->text());
	settings.set("b/coord_text_color", ui->m_coordTextColorEdit->text());
	settings.setInitialized(true);
	return settings;
}

void BoardSettingsWidget::onContextChanged(const BoardSettings* settings)
{
	readSettings();
	readBoardSettings(settings);

	m_oldSettings = boardSettings();
}

void BoardSettingsWidget::restore()
{
	onContextChanged(&m_oldSettings);
}

void BoardSettingsWidget::enableButtons(bool enabled)
{
	ui->m_lightSquareColorBtn->setEnabled(enabled);
	ui->m_darkSquareColorBtn->setEnabled(enabled);
	ui->m_borderColorBtn->setEnabled(enabled);
	ui->m_coordTextColorBtn->setEnabled(enabled);
}
void BoardSettingsWidget::onLightSquareColorBtnClicked()
{
	enableButtons(false);
	showColorDialog(ui->m_lightSquareColorEdit);
}

void BoardSettingsWidget::onDarkSquareColorBtnClicked()
{
	enableButtons(false);
	showColorDialog(ui->m_darkSquareColorEdit);
}

void BoardSettingsWidget::onBorderColorBtnClicked()
{
	enableButtons(false);
	showColorDialog(ui->m_borderColorEdit);
}

void BoardSettingsWidget::onCoordTextColorBtnClicked()
{
	enableButtons(false);
	showColorDialog(ui->m_coordTextColorEdit);
}

void BoardSettingsWidget::showColorDialog(QLineEdit* edit)
{
	QColorDialog dlg(this);
	dlg.setOptions(QColorDialog::DontUseNativeDialog | QColorDialog::NoButtons);
	connect (&dlg, SIGNAL(currentColorChanged(QColor)), this,
		 SLOT(onDialogColorChanged(QColor)));
	connect (this, SIGNAL(colorChanged(QString)), edit, SLOT(setText(QString)));

	dlg.show();
	QColor color = QColor(edit->text());
	if (color.isValid())
		dlg.setCurrentColor(color);
	dlg.move(dlg.x() + dlg.width(), dlg.y());

	if (dlg.exec())
	{
		color = dlg.selectedColor();
		if (color.isValid())
			edit->setText(color.name());
	}

	disconnect (&dlg, SIGNAL(currentColorChanged(QColor)), this,
		 SLOT(onDialogColorChanged(QColor)));
	disconnect (this, SIGNAL(colorChanged(QString)), edit, SLOT(setText(QString)));
	enableButtons(true);
}

void BoardSettingsWidget::onDialogColorChanged(QColor color)
{
	if (color.isValid())
		emit colorChanged(color.name());
}

void BoardSettingsWidget::updatePreview()
{
	if (m_updateRequested)
		return;

	m_updateRequested = true;
	QTimer::singleShot(10, this, [=]()
	{
		if (ui->m_pieceSetCombo->currentIndex() > -1)
		{
			QString pieceSet(ui->m_pieceSetCombo->currentData().toString());
			m_testSettings = boardSettings();
			m_gameViewer->boardScene()->setBoardSettings(&m_testSettings);
			m_gameViewer->boardScene()->applyBoardSettings();
		}
		m_updateRequested = false;
	});
}
