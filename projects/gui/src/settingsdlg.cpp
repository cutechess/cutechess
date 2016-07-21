#include "settingsdlg.h"
#include "ui_settingsdlg.h"
#include <QShowEvent>
#include <QSettings>

SettingsDialog::SettingsDialog(QWidget* parent)
	: QDialog(parent),
	  ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);
	connect(ui->m_buttonBox, SIGNAL(accepted()),
		this, SLOT(onAccepted()));
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

void SettingsDialog::onAccepted()
{
	QSettings s;

	s.beginGroup("ui");
	s.setValue("highlight_legal_moves", ui->m_highlightLegalMovesCheck->isChecked());
	s.endGroup();

	s.beginGroup("tournament");
	s.setValue("concurrency", ui->m_concurrencySpin->value());
	s.endGroup();

	accept();
}

void SettingsDialog::showEvent(QShowEvent* event)
{
	QSettings s;

	s.beginGroup("ui");
	ui->m_highlightLegalMovesCheck->setChecked(s.value("highlight_legal_moves").toBool());
	s.endGroup();

	s.beginGroup("tournament");
	ui->m_concurrencySpin->setValue(s.value("concurrency").toInt());
	s.endGroup();

	QDialog::showEvent(event);
}
