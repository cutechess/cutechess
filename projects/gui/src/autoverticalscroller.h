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

#ifndef AUTO_VERTICAL_SCROLLER_H
#define AUTO_VERTICAL_SCROLLER_H

#include <QObject>

class QAbstractItemView;

/*!
 * \brief Automatically scroll an item view when new items are added.
 *
 * AutoVerticalScroller automatically scrolls an item view vertically when
 * new items are added. Automatic scrolling only happens when the scroll bar
 * is positioned at the bottom and thus the user has always control of the
 * behavior.
 */
class AutoVerticalScroller : QObject
{
	Q_OBJECT

	public:
		AutoVerticalScroller(QAbstractItemView* view, QObject* parent = 0);

	private:
		QAbstractItemView* m_view;
		bool m_scrollToBottom;

	private slots:
		void onRowsAboutToBeInserted();
		void onRowsInserted();

};

#endif // AUTO_VERTICAL_SCROLLER_H

