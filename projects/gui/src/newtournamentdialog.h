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

#ifndef NEWTOURNAMENTDIALOG_H
#define NEWTOURNAMENTDIALOG_H

#include <QDialog>
#include <timecontrol.h>

class QModelIndex;
class QItemSelection;
class EngineManager;
class EngineConfigurationModel;
class EngineConfigurationProxyModel;
class GameManager;
class Tournament;

namespace Ui {
	class NewTournamentDialog;
}

class NewTournamentDialog : public QDialog
{
	Q_OBJECT

	public:
		explicit NewTournamentDialog(EngineManager* engineManager,
					     QWidget* parent = nullptr);
		virtual ~NewTournamentDialog();

		Tournament* createTournament(GameManager* gameManager) const;

	private slots:
		void addEngineOnDblClick(const QModelIndex& index);
		void addEngine();
		void removeEngine();
		void configureEngine(const QModelIndex& index);
		void onVariantChanged(const QString& variant);
		void onPlayerSelectionChanged(const QItemSelection& selected,
					      const QItemSelection& deselected);
	
	private:
		void moveEngine(int offset);
		bool canStart() const;
		void readSettings();

		EngineManager* m_srcEngineManager;
		EngineManager* m_addedEnginesManager;
		EngineConfigurationModel* m_srcEnginesModel;
		EngineConfigurationModel* m_addedEnginesModel;
		EngineConfigurationProxyModel* m_proxyModel;
		Ui::NewTournamentDialog* ui;
};

#endif // NEWTOURNAMENTDIALOG_H
