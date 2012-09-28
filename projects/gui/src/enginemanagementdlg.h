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

#ifndef ENGINEMANAGEMENTDIALOG_H
#define ENGINEMANAGEMENTDIALOG_H

#include <QDialog>
#include <engineconfiguration.h>

class EngineManager;
class QSortFilterProxyModel;
class QModelIndex;

namespace Ui {
	class EngineManagementDialog;
}

/*!
 * \brief The EngineManagementDialog class provides a dialog for chess engine
 * management.
*/
class EngineManagementDialog : public QDialog
{
	Q_OBJECT

	public:
		/*!
		 * Creates a new engine management window with \a engineConfigurations
		 * and \a parent as parent.
		*/
		EngineManagementDialog(QWidget* parent = 0);
		/*! Destroys the dialog. */
		virtual ~EngineManagementDialog();

		QList<EngineConfiguration> engines() const;
	
	private slots:
		void updateUi();
		void updateSearch(const QString& terms);
		void addEngine();
		void configureEngine();
		void configureEngine(const QModelIndex& index);
		void removeEngine();

	private:
		EngineManager* m_engineManager;
		QSortFilterProxyModel* m_filteredModel;
		Ui::EngineManagementDialog* ui;
};

#endif // ENGINEMANAGEMENTDIALOG_H
