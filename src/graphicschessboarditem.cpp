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
#include <QGraphicsSvgItem>

#include "graphicschessboarditem.h"
#include "graphicschessboardsquareitem.h"

const qreal GraphicsChessboardItem::size = 400;
const qreal GraphicsChessboardItem::borderSize = 25;

GraphicsChessboardItem::GraphicsChessboardItem(QGraphicsItem* parent)
	: QGraphicsItem(parent)
{
	m_showBorder = true;
	m_borderColor = QColor("peru");
	
	initChessboard();
	initChessPieces();
}

QRectF GraphicsChessboardItem::boundingRect() const
{
	if (isBorderVisible())
	{
		return QRectF(0, 0, GraphicsChessboardItem::size +
			(2 * GraphicsChessboardItem::borderSize), GraphicsChessboardItem::size +
			(2 * GraphicsChessboardItem::borderSize));
	}

	return QRectF(0, 0, GraphicsChessboardItem::size, GraphicsChessboardItem::size);
}

void GraphicsChessboardItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	Q_UNUSED(option)
	Q_UNUSED(widget)

	painter->fillRect(boundingRect(), m_borderColor);
}

void GraphicsChessboardItem::showBorder(bool visibility)
{
	m_showBorder = visibility;
}

bool GraphicsChessboardItem::isBorderVisible() const
{
	return m_showBorder;
}

void GraphicsChessboardItem::initChessboard()
{
	bool colorToggle = false;
	int row = 0;
	GraphicsChessboardSquareItem* square;

	for (int i = 0; i < 64; i++)
	{
		colorToggle = !colorToggle;

		if (colorToggle)
		{
			square = new GraphicsChessboardSquareItem(this,
				GraphicsChessboardSquareItem::LightSquare);
		}
		else
		{
			square = new GraphicsChessboardSquareItem(this,
				GraphicsChessboardSquareItem::DarkSquare);
		}

		square->setPositionInChessboard(Chessboard::ChessSquare(i));
		
		if (isBorderVisible())
		{
			square->setPos(GraphicsChessboardSquareItem::size * (i % 8) +
				GraphicsChessboardItem::borderSize,
				GraphicsChessboardSquareItem::size * row +
				GraphicsChessboardItem::borderSize);
		}
		else
		{
			square->setPos(GraphicsChessboardSquareItem::size * (i % 8),
				GraphicsChessboardSquareItem::size * row);
		}

		m_squares[i] = square;
		
		if (i > 0 && (i + 1) % 8 == 0)
		{
			row++;
			colorToggle = !colorToggle;
		}
	}
}

void GraphicsChessboardItem::initChessPieces()
{
	// TODO: Using QSvgRenderer it would be possible to save some memory
	
	// TODO: The pieces aren't centered within the square

	// == BLACK PIECES ==
	//
	// The order of piece initialization is the same as they appear in the
	// chessboard: Rook, Knigh, Bishop, Queen, King, Bishop, Knight, Rook,
	// and 8 Pawns.
	m_pieces[0] = new QGraphicsSvgItem(":/brook.svg", m_squares[0]);
	m_pieces[1] = new QGraphicsSvgItem(":/bknight.svg", m_squares[1]);
	m_pieces[2] = new QGraphicsSvgItem(":/bbishop.svg", m_squares[2]);
	m_pieces[3] = new QGraphicsSvgItem(":/bqueen.svg", m_squares[3]);
	m_pieces[4] = new QGraphicsSvgItem(":/bking.svg", m_squares[4]);
	m_pieces[5] = new QGraphicsSvgItem(":/bbishop.svg", m_squares[5]);
	m_pieces[6] = new QGraphicsSvgItem(":/bknight.svg", m_squares[6]);
	m_pieces[7] = new QGraphicsSvgItem(":/brook.svg", m_squares[7]);

	for (int i = 0; i < 8; i++)
	{
		m_pieces[8 + i] = new QGraphicsSvgItem(":/bpawn.svg", m_squares[8 + i]);
	}

	// == WHITE PIECES ==
	//
	// The order of piece initialization is the same as they appear in the
	// chessboard: Rook, Knigh, Bishop, Queen, King, Bishop, Knight, Rook,
	// and 8 Pawns.
	m_pieces[15] = new QGraphicsSvgItem(":/wrook.svg", m_squares[56]);
	m_pieces[16] = new QGraphicsSvgItem(":/wknight.svg", m_squares[57]);
	m_pieces[17] = new QGraphicsSvgItem(":/wbishop.svg", m_squares[58]);
	m_pieces[18] = new QGraphicsSvgItem(":/wqueen.svg", m_squares[59]);
	m_pieces[19] = new QGraphicsSvgItem(":/wking.svg", m_squares[60]);
	m_pieces[20] = new QGraphicsSvgItem(":/wbishop.svg", m_squares[61]);
	m_pieces[21] = new QGraphicsSvgItem(":/wknight.svg", m_squares[62]);
	m_pieces[22] = new QGraphicsSvgItem(":/wrook.svg", m_squares[63]);

	for (int i = 0; i < 8; i++)
	{
		m_pieces[23 + i] = new QGraphicsSvgItem(":/wpawn.svg", m_squares[48 + i]);
	}
}

