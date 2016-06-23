#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H


#include <QDialog>

namespace Ui {
	class SettingsDialog;
}

class SettingsManager;

class SettingsDialog : public QDialog
{
	Q_OBJECT

	public:
		SettingsDialog(SettingsManager* manager,
			       QWidget* parent = nullptr);
		/*! Destroys the dialog. */
		virtual ~SettingsDialog();

	protected:
		// reimplemented from QDialog
		void showEvent(QShowEvent* event);

	private:
		Ui::SettingsDialog* ui;
		SettingsManager* m_manager;
};

#endif // SETTINGSDIALOG_H
