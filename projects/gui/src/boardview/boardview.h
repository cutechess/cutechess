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

#ifndef BOARDVIEW_H
#define BOARDVIEW_H

#include <QGraphicsView>
#include <QPixmap>
class QTimer;

/*!
 * \brief A view widget for displaying a QGraphicsScene.
 *
 * BoardView is meant for visualizing the contents of a BoardScene.
 * Unlike a pure QGraphicsView, BoardView doesn't use scrollbars and
 * always keeps the view fitted to the entire scene.
 */
class BoardView : public QGraphicsView
{
	Q_OBJECT

	public:
		/*! Creates a new BoardView object that displays \a scene. */
		explicit BoardView(QGraphicsScene* scene, QWidget* parent = nullptr);

		// Inherited from QGraphicsView
		virtual QSize sizeHint() const;
		virtual int heightForWidth(int width) const;

	protected:
		// Inherited from QGraphicsView
		virtual void resizeEvent(QResizeEvent* event);
		virtual void paintEvent(QPaintEvent* event);

	private slots:
		void fitToRect();
		void onSceneRectChanged();

	private:
		bool m_initialized;
		QTimer* m_resizeTimer;
		QPixmap m_resizePixmap;
};

#endif // BOARDVIEW_H
