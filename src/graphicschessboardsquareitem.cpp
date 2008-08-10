/*
    This file is part of SloppyGUI.

    SloppyGUI is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SloppyGUI is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SloppyGUI.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QPainter>
#include <QGraphicsScene>
#include <QtDebug>
#include <QGraphicsItem>
#include <QGraphicsSvgItem>

#include "graphicschessboardsquareitem.h"

const qreal GraphicsChessboardSquareItem::size = 50;
QColor GraphicsChessboardSquareItem::m_lightSquareColor("navajowhite");
QColor GraphicsChessboardSquareItem::m_darkSquareColor("darkred");
QColor GraphicsChessboardSquareItem::m_selectedSquareColor("gold");
QColor GraphicsChessboardSquareItem::m_focusMarkerColor("black");

GraphicsChessboardSquareItem::GraphicsChessboardSquareItem(QGraphicsItem* parent, ChessboardSquareType type)
	: QGraphicsItem(parent), m_type(type)
{
	setFlag(ItemIsSelectable);
	setFlag(ItemIsFocusable);
	m_selectionWasHandled = false;
}

void GraphicsChessboardSquareItem::setLightSquareColor(const QColor& color)
{
	GraphicsChessboardSquareItem::m_lightSquareColor = color;
}

void GraphicsChessboardSquareItem::setDarkSquareColor(const QColor& color)
{
	GraphicsChessboardSquareItem::m_darkSquareColor = color;
}

void GraphicsChessboardSquareItem::setSelectedSquareColor(const QColor& color)
{
	GraphicsChessboardSquareItem::m_selectedSquareColor = color;
}

void GraphicsChessboardSquareItem::setFocusMarkerColor(const QColor& color)
{
	GraphicsChessboardSquareItem::m_focusMarkerColor = color;
}

QColor GraphicsChessboardSquareItem::lightSquareColor()
{
	return GraphicsChessboardSquareItem::m_lightSquareColor;
}

QColor GraphicsChessboardSquareItem::darkSquareColor()
{
	return GraphicsChessboardSquareItem::m_darkSquareColor;
}

QColor GraphicsChessboardSquareItem::selectedSquareColor()
{
	return GraphicsChessboardSquareItem::m_selectedSquareColor;
}

QColor GraphicsChessboardSquareItem::focusMarkerColor()
{
	return GraphicsChessboardSquareItem::m_focusMarkerColor;
}

GraphicsChessboardSquareItem::ChessboardSquareType GraphicsChessboardSquareItem::squareType() const
{
	return m_type;
}

bool GraphicsChessboardSquareItem::isLightSquare() const
{
	return m_type == GraphicsChessboardSquareItem::LightSquare;
}

bool GraphicsChessboardSquareItem::isDarkSquare() const
{
	return m_type == GraphicsChessboardSquareItem::DarkSquare;
}

void GraphicsChessboardSquareItem::setSquareType(ChessboardSquareType type)
{
	m_type = type;
}

QRectF GraphicsChessboardSquareItem::boundingRect() const
{
	return QRectF(0, 0, GraphicsChessboardSquareItem::size, GraphicsChessboardSquareItem::size);
}

void GraphicsChessboardSquareItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	Q_UNUSED(option)
	Q_UNUSED(widget)

	if (isSelected())
	{
		painter->fillRect(boundingRect(), QBrush(GraphicsChessboardSquareItem::selectedSquareColor()));
	}
	else
	{
		if (isLightSquare())
		{
			painter->fillRect(boundingRect(), QBrush(GraphicsChessboardSquareItem::lightSquareColor()));
		}
		else
		{
			painter->fillRect(boundingRect(), QBrush(GraphicsChessboardSquareItem::darkSquareColor()));
		}
	}
	
	if (hasFocus())
	{
		painter->setBrush(Qt::NoBrush);
		QPen pen(GraphicsChessboardSquareItem::focusMarkerColor());
		pen.setWidthF(3.0);
		painter->setPen(pen);
		
		int x = (int)(boundingRect().x() + pen.width());
		int y = (int)(boundingRect().y() + pen.width());
		int width = (int)(boundingRect().height() - (2 * pen.width()));
		int height = (int)(boundingRect().width() - (2 * pen.width()));
		painter->drawRect(x, y, width, height);
	}
}

void GraphicsChessboardSquareItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	/*QList<QGraphicsItem*> selected = scene()->selectedItems();
	if (!selected.isEmpty())
	{
		qDebug() << "Would move piece from" << selected.first()->pos() << " to " << pos();
		
		if (GraphicsChessboardSquareItem* source = dynamic_cast<GraphicsChessboardSquareItem* >(selected.first()))
		{
			if (source->isOccupied())
			{
				// Get the chess piece
				if (QGraphicsSvgItem* piece = dynamic_cast<QGraphicsSvgItem* >(source->childItems().front()))
				{
					piece->setParentItem(this);
					piece->setPos(0, 0);
				}
			}
		}

		m_selectionWasHandled = true;
	}
	else
	{
		m_selectionWasHandled = false;
	}
	*/
	QGraphicsItem::mousePressEvent(event);
}

void GraphicsChessboardSquareItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	QGraphicsItem::mouseReleaseEvent(event);

	if (m_selectionWasHandled)
	{
		setSelected(false);
		update();
	}
}

void GraphicsChessboardSquareItem::focusOutEvent(QFocusEvent* event)
{
	QGraphicsItem::focusOutEvent(event);

	// We need to redraw to get rid of the focus rectangle
	update();
}

void GraphicsChessboardSquareItem::focusInEvent(QFocusEvent* event)
{
	QGraphicsItem::focusInEvent(event);

	// We need to redraw to show the focus rectangle
	update();
}

bool GraphicsChessboardSquareItem::isOccupied() const
{
	// Check if this square has child items and they're are specific type
	/*QList<QGraphicsItem*> children = childItems();
	
	if (!children.isEmpty())
	{
		if (dynamic_cast<QGraphicsSvgItem *>(children.front()))
		{
			return true;
		}
	}*/
	return false;
}

void GraphicsChessboardSquareItem::setPositionInChessboard(Chessboard::ChessSquare pos)
{
	m_posInChessboard = pos;
}

Chessboard::ChessSquare GraphicsChessboardSquareItem::positionInChessboard() const
{
	return m_posInChessboard;
}

