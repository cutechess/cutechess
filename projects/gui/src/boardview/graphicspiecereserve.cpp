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

#include "graphicspiecereserve.h"
#include <QPainter>
#include "graphicspiece.h"


GraphicsPieceReserve::GraphicsPieceReserve(qreal squareSize,
					   QGraphicsItem* parent)
	: QGraphicsItem(parent),
	  m_tileWidth(squareSize * 1.5),
	  m_tileHeight(squareSize),
	  m_rowCount(1)
{
	m_rect.setWidth(m_tileWidth * 2);
	m_rect.setHeight(m_tileHeight);
	m_rect.moveCenter(QPointF(0, 0));

	setCacheMode(DeviceCoordinateCache);
}

int GraphicsPieceReserve::type() const
{
	return Type;
}

QRectF GraphicsPieceReserve::boundingRect() const
{
	return m_rect;
}

void GraphicsPieceReserve::paint(QPainter* painter,
				 const QStyleOptionGraphicsItem* option,
				 QWidget* widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	QFont font(painter->font());
	font.setPixelSize(m_tileHeight / 2);
	painter->setFont(font);

	painter->drawRoundedRect(m_rect, 15.0, 15.0);
	painter->drawLine(0, m_rect.top(), 0, m_rect.bottom());


	for (int i = 0; i < 2; i++)
	{
		const QList<Chess::Piece>& list(m_tiles[i]);
		for (int j = 0; j < list.size(); j++)
		{
			int count = m_pieces.count(list.at(j));
			if (count < 1)
				continue;

			QRectF rect(textRect(Chess::Side::Type(i), j));
			painter->drawText(rect, QString::number(count), QTextOption(Qt::AlignCenter));
		}
	}
}

int GraphicsPieceReserve::pieceCount(const Chess::Piece& piece) const
{
	return m_pieces.count(piece);
}

GraphicsPiece* GraphicsPieceReserve::piece(const Chess::Piece& piece) const
{
	return m_pieces.value(piece);
}

GraphicsPiece* GraphicsPieceReserve::takePiece(const Chess::Piece& piece)
{
	GraphicsPiece* gpiece = m_pieces.take(piece);
	Q_ASSERT(gpiece != nullptr);

	if (!m_pieces.contains(piece))
	{
		QList<Chess::Piece>& list = m_tiles[piece.side()];
		int index = list.indexOf(piece);

		if (index == list.size() - 1)
		{
			list.removeLast();
			while (!list.isEmpty() && list.last().isEmpty())
				list.removeLast();
			updateTiles();
		}
		else if (index != -1)
			list[index] = Chess::Piece();
	}
	update(textRect(piece));
	gpiece->setParentItem(nullptr);
	gpiece->setContainer(nullptr);

	return gpiece;
}

void GraphicsPieceReserve::addPiece(GraphicsPiece* piece)
{
	Q_ASSERT(piece != nullptr);

	Chess::Piece type(piece->pieceType());
	GraphicsPiece* old = m_pieces.value(type);

	m_pieces.insert(type, piece);
	piece->setContainer(this);
	piece->setParentItem(this);

	if (old != nullptr)
		piece->setPos(old->pos());
	else
	{
		QList<Chess::Piece>& list = m_tiles[type.side()];
		int index = list.indexOf(Chess::Piece());
		if (index == -1)
		{
			index = list.size();
			list.append(type);
			updateTiles();
		}
		else
			list[index] = type;

		piece->setPos(piecePos(type.side(), index));
	}
	update(textRect(type));
}

QPointF GraphicsPieceReserve::piecePos(Chess::Side side, int index) const
{
	QPointF point;

	if (side == Chess::Side::White)
		point.setX(-m_tileHeight / 2);
	else
		point.setX(m_rect.right() - m_tileHeight / 2);

	qreal top = m_rect.top() + m_tileHeight / 2;
	point.setY(top + m_tileHeight * index);

	return point;
}

QRectF GraphicsPieceReserve::textRect(Chess::Side side, int index) const
{
	QRectF rect;

	if (side == Chess::Side::White)
		rect.setLeft(m_rect.left());
	else
		rect.setLeft(0);
	rect.setWidth(m_tileWidth / 3);

	rect.setTop(m_rect.top() + m_tileHeight * index);
	rect.setHeight(m_tileHeight);

	return rect;
}

QRectF GraphicsPieceReserve::textRect(const Chess::Piece& piece) const
{
	int index = m_tiles[piece.side()].indexOf(piece);
	if (index == -1)
		return QRectF();

	return textRect(piece.side(), index);
}

void GraphicsPieceReserve::updateTiles()
{
	int count = qMax(m_tiles[0].size(), m_tiles[1].size());
	if (count != m_rowCount && count > 0)
	{
		prepareGeometryChange();

		qreal newHeight = m_tileHeight * count;
		qreal adjust = (m_rect.height() - newHeight) / 2;

		m_rect.setHeight(newHeight);
		m_rowCount = qMax(count, 1);
		moveBy(0, adjust);
	}
}
