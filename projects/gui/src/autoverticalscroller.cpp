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

#include <QAbstractItemView>
#include <QScrollBar>

#include "autoverticalscroller.h"

AutoVerticalScroller::AutoVerticalScroller(QAbstractItemView* view,
                                           QObject* parent)
	: QObject(parent), m_view(view), m_scrollToBottom(false)
{
	connect(m_view->model(),
		SIGNAL(rowsAboutToBeInserted(const QModelIndex&, int, int)),
		this, SLOT(onRowsAboutToBeInserted()));
	connect(m_view->model(), SIGNAL(rowsInserted(const QModelIndex&, int, int)),
		this, SLOT(onRowsInserted()));
}

void AutoVerticalScroller::onRowsAboutToBeInserted()
{
	m_scrollToBottom = m_view->verticalScrollBar()->value() ==
		m_view->verticalScrollBar()->maximum();
}

void AutoVerticalScroller::onRowsInserted()
{
	if (m_scrollToBottom)
		m_view->scrollToBottom();
}
