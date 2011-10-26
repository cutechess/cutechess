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

#include <QDialog>
#include <board/side.h>
#include <timecontrol.h>

class EngineConfigurationModel;
class EngineConfigurationProxyModel;

namespace Ui {
	class NewGameDialog;
}

/*!
 * \brief The NewGameDialog class provides a dialog for creating a new game.
*/
class NewGameDialog : public QDialog
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
		/*! Destroys the dialog. */
		virtual ~NewGameDialog();

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

		/*! Returns the chosen time control. */
		TimeControl timeControl() const;

	private slots:
		void configureWhiteEngine();
		void configureBlackEngine();
		void showTimeControlDialog();
		void onVariantChanged(const QString& variant);

	private:
		EngineConfigurationModel* m_engines;
		EngineConfigurationProxyModel* m_proxyModel;
		TimeControl m_timeControl;
		Ui::NewGameDialog* ui;
};

#endif // NEWGAMEDIALOG_H
