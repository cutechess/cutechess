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

#ifndef ENGINESELECTIONDLG_H
#define ENGINESELECTIONDLG_H

#include <QDialog>
#include <QItemSelection>
#include <QListView>

class EngineConfigurationProxyModel;

namespace Ui {
	class EngineSelectionDialog;
}

class EngineSelectionDialog : public QDialog
{
	Q_OBJECT

	public:
		explicit EngineSelectionDialog(EngineConfigurationProxyModel* model,
					       QWidget* parent = nullptr);
		virtual ~EngineSelectionDialog();

		QItemSelection selection() const;
		QModelIndexList selectedRows(int column = 0) const;
		QListView * enginesList() const;

	private:
		EngineConfigurationProxyModel* m_model;
		Ui::EngineSelectionDialog* ui;
};

#endif // ENGINESELECTIONDLG_H
