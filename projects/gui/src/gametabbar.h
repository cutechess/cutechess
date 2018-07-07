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

#ifndef GAME_TAB_BAR_H
#define GAME_TAB_BAR_H

#include <QTabBar>

/*!
 * \brief Provides a game tab bar for game windows.
 */
class GameTabBar : public QTabBar
{
	Q_OBJECT

	public:
		GameTabBar(QWidget* parent = nullptr);

	public slots:
		/*! Show the next tab in the tab bar. */
		void showNextTab();
		/*! Show the previous tab in the tab bar. */
		void showPreviousTab();

};

#endif // GAME_TAB_BAR_H

