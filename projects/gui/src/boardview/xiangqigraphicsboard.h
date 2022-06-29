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

#ifndef XIANGQIGRAPHICSBOARD_H
#define XIANGQIGRAPHICSBOARD_H

#include <QGraphicsItem>
#include <QColor>
#include <QVector>
#include <board/square.h>
#include <board/piece.h>
#include "graphicsboard.h"
class GraphicsPiece;
class QPropertyAnimation;


class XiangqiGraphicsBoard : public GraphicsBoard
{
	public:
		/*!
		 * Creates a new GraphicsBoard object.
		 *
		 * The board will have \a files files/columns and \a ranks
		 * ranks/rows, and the squares' width and height will be
		 * \a squareSize.
		 */
		explicit XiangqiGraphicsBoard(int files,
					      int ranks,
					      qreal squareSize,
					      QGraphicsItem* parent = nullptr);
		/*! Destroys the XiangqiGraphicsBoard object. */
		virtual ~XiangqiGraphicsBoard();

		// Inherited from QGraphicsItem
		virtual QRectF boundingRect() const override;
		virtual void paint(QPainter* painter,
				   const QStyleOptionGraphicsItem* option,
				   QWidget* widget = nullptr) override;

	protected:
		virtual int squareCol(qreal x) const override;
		virtual int squareRow(qreal x) const override;
		virtual QPointF topLeft() const override;

	private:
		QColor m_bgColor;
};

#endif // XIANGQIGRAPHICSBOARD_H
