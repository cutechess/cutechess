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

#ifndef ENGINECONFIGURATIONDIALOG_H
#define ENGINECONFIGURATIONDIALOG_H

#include <QDialog>
#include <QSet>
#include <engineconfiguration.h>

class EngineOption;
class EngineOptionModel;
class ChessEngine;

namespace Ui {
	class EngineConfigurationDialog;
}

/*!
 * \brief The EngineConfigurationDialog class provides a dialog for chess engine
 * configuration.
*/
class EngineConfigurationDialog : public QDialog
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
		EngineConfigurationDialog(DialogMode mode, QWidget* parent = nullptr);
		/*! Destroys the dialog. */
		virtual ~EngineConfigurationDialog();

		/*!
		 * Applies the information of \a engine to the dialog.
		*/
		void applyEngineInformation(const EngineConfiguration& engine);
		/*!
		 * Returns an engine based on the information user selected.
		*/
		EngineConfiguration engineConfiguration();

		/*!
		 * Sets the list of names reserved for other engines.
		 *
		 * If the user tries to use a reserved name they'll see a
		 * warning message.
		 */
		void setReservedNames(const QSet<QString>& names);

	signals:
		void detectionFinished();
	
	private slots:
		void browseCommand();
		void setExecutable(const QString& file);
		void browseWorkingDir();
		void detectEngineOptions();
		void restoreDefaults();
		void onDetectionFinished();
		void onEngineReady();
		void onEngineQuit();
		void onTabChanged(int index);
		void onNameOrCommandChanged();
		void onAccepted();
		void resizeColumns();

	private:
		bool m_hasError;
		EngineOptionModel* m_engineOptionModel;
		QString m_oldCommand;
		QString m_oldPath;
		QString m_oldProtocol;
		QList<EngineOption*> m_options;
		QStringList m_variants;
		ChessEngine* m_engine;
		Ui::EngineConfigurationDialog* ui;
		QSet<QString> m_reservedNames;
};

#endif // ENGINECONFIGURATIONDIALOG_H
