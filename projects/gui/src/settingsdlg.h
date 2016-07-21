#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H


#include <QDialog>

namespace Ui {
	class SettingsDialog;
}

class SettingsDialog : public QDialog
{
	Q_OBJECT

	public:
		SettingsDialog(QWidget* parent = nullptr);
		/*! Destroys the dialog. */
		virtual ~SettingsDialog();

	private:
		void readSettings();

		Ui::SettingsDialog* ui;
};

#endif // SETTINGSDIALOG_H
