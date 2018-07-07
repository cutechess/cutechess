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

#ifndef TILELAYOUT_H
#define TILELAYOUT_H

#include <QLayout>
#include <QStyle>

class TileLayout : public QLayout
{
	Q_OBJECT

	public:
		explicit TileLayout(QWidget* parent = nullptr);
		virtual ~TileLayout();

		// Inherited from QLayout
		void addItem(QLayoutItem* item);
		Qt::Orientations expandingDirections() const;
		int count() const;
		QLayoutItem* itemAt(int index) const;
		QSize minimumSize() const;
		void setGeometry(const QRect& rect);
		QSize sizeHint() const;
		QLayoutItem* takeAt(int index);

	private:
		int horizontalSpacing() const;
		int verticalSpacing() const;
		void doLayout(const QRect& rect) const;
		int smartSpacing(QStyle::PixelMetric pm) const;

		QList<QLayoutItem*> m_items;
};

#endif // TILELAYOUT_H
