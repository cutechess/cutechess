/*
    This file is part of SloppyGUI.

    SloppyGUI is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SloppyGUI is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SloppyGUI.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ENGINEMANAGEMENTDIALOG_H
#define ENGINEMANAGEMENTDIALOG_H

#include <QDialog>

#include "ui_enginemanagementdlg.h"
#include "engineconfigurationmodel.h"

class EngineConfigurationModel;
class QSortFilterProxyModel;

/*!
 * \brief The EngineManagementDialog class provides a dialog for chess engine
 * management.
*/
class EngineManagementDialog : public QDialog, private Ui::EngineManagementDialog
{
	Q_OBJECT

	public:
		/*!
		 * Creates a new engine management window with \a engineConfigurations
		 * and \a parent as parent.
		*/
		EngineManagementDialog(EngineConfigurationModel* engineConfigurations,
		                       QWidget* parent = 0);
	
	private slots:
		void updateUi();
		void addEngine();
		void configureEngine();
		void removeEngine();

	private:
		QSortFilterProxyModel* m_filteredModel;
		EngineConfigurationModel* m_originalModel;

};

#endif // ENGINEMANAGEMENTDIALOG_H

