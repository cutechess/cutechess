#include "settingsdlg.h"
#include "ui_settingsdlg.h"
#include <QShowEvent>
#include <settingsmanager.h>

SettingsDialog::SettingsDialog(SettingsManager* manager, QWidget* parent)
	: QDialog(parent),
	  ui(new Ui::SettingsDialog),
	  m_manager(manager)
{
	ui->setupUi(this);

	connect(ui->m_buttonBox, &QDialogButtonBox::accepted, this, [=]()
	{
		// General settings
		m_manager->setValue("highlight_legal_moves", ui->m_highlightLegalMovesCheck->isChecked());

		// Tournament settings
		m_manager->setValue("concurrency", ui->m_concurrencySpin->value());

		m_manager->save();
		accept();
	});
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

void SettingsDialog::showEvent(QShowEvent* event)
{
	// General settings
	ui->m_highlightLegalMovesCheck->setChecked(m_manager->value("highlight_legal_moves").toBool());

	// Tournament settings
	ui->m_concurrencySpin->setValue(m_manager->value("concurrency").toInt());

	QDialog::showEvent(event);
}
