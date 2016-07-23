#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
	class SettingsDialog;
}

/*!
 * \brief Dialog for configuring global settings
 */
class SettingsDialog : public QDialog
{
	Q_OBJECT

	public:
		/*! Creates a new SettingsDialog. */
		SettingsDialog(QWidget* parent = nullptr);
		/*! Destroys the dialog. */
		virtual ~SettingsDialog();

	protected:
		// Inherited from QDialog
		virtual void closeEvent(QCloseEvent* event);

	private:
		void readSettings();

		Ui::SettingsDialog* ui;
};

#endif // SETTINGSDIALOG_H
