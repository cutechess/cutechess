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

#include "tilelayout.h"
#include <qmath.h>
#include <QWidget>
#include <algorithm>

TileLayout::TileLayout(QWidget *parent)
	: QLayout(parent)
{
	setContentsMargins(0, 0, 0, 0);
}

TileLayout::~TileLayout()
{
	qDeleteAll(m_items);
}

void TileLayout::addItem(QLayoutItem* item)
{
	m_items.append(item);
}

Qt::Orientations TileLayout::expandingDirections() const
{
	return Qt::Vertical | Qt::Horizontal;
}

int TileLayout::count() const
{
	return m_items.size();
}

QLayoutItem* TileLayout::itemAt(int index) const
{
	return m_items.value(index, nullptr);
}

QSize TileLayout::minimumSize() const
{
	QSize size;
	for (const QLayoutItem* item : qAsConst(m_items))
		size = size.expandedTo(item->minimumSize());

	if (!size.isValid())
		return QLayout::minimumSize();

	return size;
}

void TileLayout::setGeometry(const QRect& rect)
{
	QLayout::setGeometry(rect);
	doLayout(rect);
}

QSize TileLayout::sizeHint() const
{
	QSize size;
	for (const QLayoutItem* item : qAsConst(m_items))
		size = size.expandedTo(item->sizeHint());

	return size;
}

QLayoutItem* TileLayout::takeAt(int index)
{
	if (index >= 0 && index < m_items.size())
		return m_items.takeAt(index);
	else
		return nullptr;
}

int TileLayout::horizontalSpacing() const
{
	return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
}

int TileLayout::verticalSpacing() const
{
	return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
}

void TileLayout::doLayout(const QRect& rect) const
{
	if (m_items.isEmpty())
		return;

	int left, top, right, bottom;
	getContentsMargins(&left, &top, &right, &bottom);
	QRect effectiveRect = rect.adjusted(left, top, -right, -bottom);
	int x = effectiveRect.x();
	int y = effectiveRect.y();

	QWidget* widget = m_items.first()->widget();
	int spaceX = horizontalSpacing();
	if (spaceX == -1)
		spaceX = widget->style()->layoutSpacing(QSizePolicy::DefaultType,
							QSizePolicy::DefaultType,
							Qt::Horizontal);
	int spaceY = verticalSpacing();
	if (spaceY == -1)
		spaceY = widget->style()->layoutSpacing(QSizePolicy::DefaultType,
							QSizePolicy::DefaultType,
							Qt::Vertical);

	qreal layoutAr = qreal(effectiveRect.width()) / effectiveRect.height();
	int count = m_items.size();
	QSize sh = widget->sizeHint();
	qreal pixels = sh.width() * sh.height() * count;

	// Approximation based on the layout's aspect ratio
	qreal totalWidth = qSqrt(pixels * layoutAr);
	int cols = totalWidth / sh.width() + 0.5;
	int rows = (count - 1) / cols + 1;

	// The approximation is far from perfect, so we perform some
	// additional checks to make sure we have as few rows and
	// columns as possible.
	int smallest = qMin(cols, rows);
	if (count <= smallest * smallest)
	{
		cols = smallest;
		rows = smallest;
	}
	if (count <= cols * (rows - 1))
		rows--;
	if (count <= (cols - 1) * rows)
		cols--;

	// Often the column and row counts are backwards, meaning that
	// the available space isn't being used efficiently. In that
	// case we swap the values of 'cols' and 'rows'.
	if (qAbs(cols - rows) >= 1)
	{
		qreal ar1 = qreal(cols * sh.width()) / (rows * sh.height());
		qreal ar2 = qreal(rows * sh.width()) / (cols * sh.height());
		if (qAbs(ar1 - layoutAr) > qAbs(ar2 - layoutAr))
			std::swap(cols, rows);
	}

	int itemWidth = (effectiveRect.width() - (cols - 1) * spaceX) / cols;
	int itemHeight = (effectiveRect.height() - (rows - 1) * spaceY) / rows;

	int col = 0;
	for (QLayoutItem* item : qAsConst(m_items))
	{
		int nextX = x + itemWidth + spaceX;
		if (++col > cols)
		{
			col = 1;
			x = effectiveRect.x();
			y += itemHeight + spaceY;
			nextX = x + itemWidth + spaceX;
		}

		item->setGeometry(QRect(QPoint(x, y), QSize(itemWidth, itemHeight)));

		x = nextX;
	}
}

int TileLayout::smartSpacing(QStyle::PixelMetric pm) const
{
	QObject* parent = this->parent();
	if (!parent)
		return -1;

	if (parent->isWidgetType())
	{
		QWidget* pw = static_cast<QWidget*>(parent);
		return pw->style()->pixelMetric(pm, nullptr, pw);
	}
	return static_cast<QLayout*>(parent)->spacing();
}
