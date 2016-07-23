#include "settingsdlg.h"
#include "ui_settingsdlg.h"
#include <QShowEvent>
#include <QSettings>
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

	connect(ui->m_concurrencySpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		this, [=](int value)
	{
		QSettings().setValue("tournament/concurrency", value);
		CuteChessApplication::instance()->gameManager()->setConcurrency(value);
	});
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

void SettingsDialog::readSettings()
{
	QSettings s;

	s.beginGroup("ui");
	ui->m_highlightLegalMovesCheck->setChecked(s.value("highlight_legal_moves").toBool());
	s.endGroup();

	s.beginGroup("tournament");
	ui->m_concurrencySpin->setValue(s.value("concurrency").toInt());
	s.endGroup();
}
