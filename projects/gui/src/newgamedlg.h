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

#ifndef NEWGAMEDIALOG_H
#define NEWGAMEDIALOG_H

#include "ui_newgamedlg.h"
#include <QDialog>
#include <board/side.h>

class EngineConfigurationModel;
class QSortFilterProxyModel;

/*!
 * \brief The NewGameDialog class provides a dialog for creating a new game.
*/
class NewGameDialog : public QDialog, private Ui::NewGameDialog
{
	Q_OBJECT

	public:
		/*! Player's type. */
		enum PlayerType
		{
			/*! Human player. */
			Human,
			/*! Computer controlled player. */
			CPU
		};

		/*!
		 * Creates a new game dialog with \a engineConfigurations as the
		 * list of chess engines and given \a parent.
		*/
		NewGameDialog(QWidget* parent = 0);

		/*! Returns the user selected player type for \a side. */
		PlayerType playerType(Chess::Side side) const;

		/*!
		 * Returns the user selected chess engine for \a side.
		 *
		 * The return value is an index to the list of engines.
		*/
		int selectedEngineIndex(Chess::Side side) const;

		/*! Returns the user-selected chess variant. */
		QString selectedVariant() const;

	private slots:
		void configureWhiteEngine();
		void configureBlackEngine();

	private:
		EngineConfigurationModel* m_engines;
		QSortFilterProxyModel* m_proxyModel;

};

#endif // NEWGAMEDIALOG_H
