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

#include "chessboardsquareitem.h"

const qreal ChessboardSquareItem::size = 50;
QColor ChessboardSquareItem::m_lightSquareColor("navajowhite");
QColor ChessboardSquareItem::m_darkSquareColor("peru");
QColor ChessboardSquareItem::m_selectedSquareColor("gold");
QColor ChessboardSquareItem::m_focusMarkerColor("black");

ChessboardSquareItem::ChessboardSquareItem(QGraphicsItem* parent, ChessboardSquareType type)
	: QGraphicsItem(parent), m_type(type)
{
	setFlag(ItemIsSelectable);
	setFlag(ItemIsFocusable);
	m_selectionWasHandled = false;
}

void ChessboardSquareItem::setLightSquareColor(const QColor& color)
{
	ChessboardSquareItem::m_lightSquareColor = color;
}

void ChessboardSquareItem::setDarkSquareColor(const QColor& color)
{
	ChessboardSquareItem::m_darkSquareColor = color;
}

void ChessboardSquareItem::setSelectedSquareColor(const QColor& color)
{
	ChessboardSquareItem::m_selectedSquareColor = color;
}

void ChessboardSquareItem::setFocusMarkerColor(const QColor& color)
{
	ChessboardSquareItem::m_focusMarkerColor = color;
}

QColor ChessboardSquareItem::lightSquareColor()
{
	return ChessboardSquareItem::m_lightSquareColor;
}

QColor ChessboardSquareItem::darkSquareColor()
{
	return ChessboardSquareItem::m_darkSquareColor;
}

QColor ChessboardSquareItem::selectedSquareColor()
{
	return ChessboardSquareItem::m_selectedSquareColor;
}

QColor ChessboardSquareItem::focusMarkerColor()
{
	return ChessboardSquareItem::m_focusMarkerColor;
}

ChessboardSquareItem::ChessboardSquareType ChessboardSquareItem::squareType() const
{
	return m_type;
}

bool ChessboardSquareItem::isLightSquare() const
{
	return m_type == ChessboardSquareItem::LightSquare;
}

bool ChessboardSquareItem::isDarkSquare() const
{
	return m_type == ChessboardSquareItem::DarkSquare;
}

void ChessboardSquareItem::setSquareType(ChessboardSquareType type)
{
	m_type = type;
}

QRectF ChessboardSquareItem::boundingRect() const
{
	return QRectF(0, 0, ChessboardSquareItem::size, ChessboardSquareItem::size);
}

void ChessboardSquareItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	Q_UNUSED(option)
	Q_UNUSED(widget)

	if (isSelected())
	{
		painter->fillRect(boundingRect(), QBrush(ChessboardSquareItem::selectedSquareColor()));
	}
	else
	{
		if (isLightSquare())
		{
			painter->fillRect(boundingRect(), QBrush(ChessboardSquareItem::lightSquareColor()));
		}
		else
		{
			painter->fillRect(boundingRect(), QBrush(ChessboardSquareItem::darkSquareColor()));
		}
	}
	
	if (hasFocus())
	{
		painter->setBrush(Qt::NoBrush);
		QPen pen(ChessboardSquareItem::focusMarkerColor());
		pen.setWidth(3);
		painter->setPen(pen);
		painter->drawRect(boundingRect().x() + pen.width(), boundingRect().y() + pen.width(),
			boundingRect().height() - (2 * pen.width()), boundingRect().width() - (2 * pen.width()));
	}
}

void ChessboardSquareItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	/*QList<QGraphicsItem*> selected = scene()->selectedItems();
	if (!selected.isEmpty())
	{
		qDebug() << "Would move piece from" << selected.first()->pos() << " to " << pos();
		
		if (ChessboardSquareItem* source = dynamic_cast<ChessboardSquareItem* >(selected.first()))
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

void ChessboardSquareItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	QGraphicsItem::mouseReleaseEvent(event);

	if (m_selectionWasHandled)
	{
		setSelected(false);
		update();
	}
}

void ChessboardSquareItem::focusOutEvent(QFocusEvent* event)
{
	QGraphicsItem::focusOutEvent(event);

	// We need to redraw to get rid of the focus rectangle
	update();
}

void ChessboardSquareItem::focusInEvent(QFocusEvent* event)
{
	QGraphicsItem::focusInEvent(event);

	// We need to redraw to show the focus rectangle
	update();
}

bool ChessboardSquareItem::isOccupied() const
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

