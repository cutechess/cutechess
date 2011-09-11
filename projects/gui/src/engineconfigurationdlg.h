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

#ifndef ENGINECONFIGURATIONDIALOG_H
#define ENGINECONFIGURATIONDIALOG_H

#include "ui_engineconfigdlg.h"
#include <QDialog>
#include <engineconfiguration.h>

class QTimer;
class EngineOption;
class EngineOptionModel;

/*!
 * \brief The EngineConfigurationDialog class provides a dialog for chess engine
 * configuration.
*/
class EngineConfigurationDialog : public QDialog, private Ui::EngineConfigurationDialog
{
	Q_OBJECT

	public:
		/*! The mode that is used in the dialog. */
		enum DialogMode
		{
			/*! Mode for adding new engine. */
			AddEngine,
			/*! Mode for configuring existing engine. */
			ConfigureEngine
		};

		/*!
		 * Creates a new engine configuration dialog with \a parent as
		 * parent
		*/
		EngineConfigurationDialog(DialogMode mode, QWidget* parent = 0);

		virtual ~EngineConfigurationDialog();

		/*!
		 * Applies the information of \a engine to the dialog.
		*/
		void applyEngineInformation(const EngineConfiguration& engine);
		/*!
		 * Returns an engine based on the information user selected.
		*/
		EngineConfiguration engineConfiguration();
	
	private slots:
		void browseCommand();
		void browseWorkingDir();
		void detectEngineOptions();
		void onEngineReady();

	private:
		EngineOptionModel* m_engineOptionModel;
		QList<EngineOption*> m_options;
		QTimer* m_optionDetectionTimer;

};

#endif // ENGINECONFIGURATIONDIALOG_H

