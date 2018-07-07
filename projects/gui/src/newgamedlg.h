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

#ifndef NEWGAMEDIALOG_H
#define NEWGAMEDIALOG_H

#include <QDialog>
#include <board/side.h>
#include <engineconfiguration.h>

class ChessGame;
class PlayerBuilder;
class EngineConfigurationModel;
class EngineConfigurationProxyModel;
class EngineManager;

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
		 * Creates a "New Game" dialog with \a engineManager as the
		 * source of engine configurations.
		 */
		NewGameDialog(EngineManager* engineManager,
			      QWidget* parent = nullptr);
		/*! Destroys the dialog. */
		virtual ~NewGameDialog();

		/*! Creates and returns the ChessGame object. */
		ChessGame* createGame() const;
		/*! Creates and returns the PlayerBuilder for \a side. */
		PlayerBuilder* createPlayerBuilder(Chess::Side side) const;

	private slots:
		void configureEngine();
		void onVariantChanged(const QString& variant);
		void onEngineChanged(int index, Chess::Side = Chess::Side::NoSide);

	private:
		void setPlayerType(Chess::Side side, PlayerType type);
		PlayerType playerType(Chess::Side side) const;

		EngineManager* m_engineManager;
		EngineConfigurationModel* m_engines;
		EngineConfigurationProxyModel* m_proxyModel;
		EngineConfiguration m_engineConfig[2];
		Ui::NewGameDialog* ui;
};

#endif // NEWGAMEDIALOG_H
