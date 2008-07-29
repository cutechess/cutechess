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
#include <QSvgRenderer>

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
	initSvgRenderers();
	initChessPieces();
}

GraphicsChessboardItem::~GraphicsChessboardItem()
{
	// Release SVG renderers
	QMap<Chessboard::ChessPiece, QSvgRenderer*> innerMap;
	foreach (innerMap, m_renderers)
	{
		QSvgRenderer* renderer = 0;
		foreach (renderer, innerMap)
		{
			delete renderer;
		}
	}
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

void GraphicsChessboardItem::initSvgRenderers()
{
	// Black pieces
	m_renderers[Chessboard::Black][Chessboard::Pawn] =
		new QSvgRenderer(QLatin1String(":/bpawn.svg"));
	m_renderers[Chessboard::Black][Chessboard::Rook] =
		new QSvgRenderer(QLatin1String(":/brook.svg"));
	m_renderers[Chessboard::Black][Chessboard::Knight] =
		new QSvgRenderer(QLatin1String(":/bknight.svg"));
	m_renderers[Chessboard::Black][Chessboard::Bishop] =
		new QSvgRenderer(QLatin1String(":/bbishop.svg"));
	m_renderers[Chessboard::Black][Chessboard::Queen] =
		new QSvgRenderer(QLatin1String(":/bqueen.svg"));
	m_renderers[Chessboard::Black][Chessboard::King] =
		new QSvgRenderer(QLatin1String(":/bking.svg"));

	// White pieces
	m_renderers[Chessboard::White][Chessboard::Pawn] =
		new QSvgRenderer(QLatin1String(":/wpawn.svg"));
	m_renderers[Chessboard::White][Chessboard::Rook] =
		new QSvgRenderer(QLatin1String(":/wrook.svg"));
	m_renderers[Chessboard::White][Chessboard::Knight] =
		new QSvgRenderer(QLatin1String(":/wknight.svg"));
	m_renderers[Chessboard::White][Chessboard::Bishop] =
		new QSvgRenderer(QLatin1String(":/wbishop.svg"));
	m_renderers[Chessboard::White][Chessboard::Queen] =
		new QSvgRenderer(QLatin1String(":/wqueen.svg"));
	m_renderers[Chessboard::White][Chessboard::King] =
		new QSvgRenderer(QLatin1String(":/wking.svg"));
}

void GraphicsChessboardItem::initChessPieces()
{
	// TODO: The pieces aren't centered within the square

	// == BLACK PIECES ==
	//
	// The order of piece initialization is the same as they appear in the
	// chessboard: Rook, Knigh, Bishop, Queen, King, Bishop, Knight, Rook,
	// and 8 Pawns.
	m_pieces[0] = new QGraphicsSvgItem(m_squares[0]);
	m_pieces[0]->setSharedRenderer(m_renderers[Chessboard::Black][Chessboard::Rook]);

	m_pieces[1] = new QGraphicsSvgItem(m_squares[1]);
	m_pieces[1]->setSharedRenderer(m_renderers[Chessboard::Black][Chessboard::Knight]);

	m_pieces[2] = new QGraphicsSvgItem(m_squares[2]);
	m_pieces[2]->setSharedRenderer(m_renderers[Chessboard::Black][Chessboard::Bishop]);

	m_pieces[3] = new QGraphicsSvgItem(m_squares[3]);
	m_pieces[3]->setSharedRenderer(m_renderers[Chessboard::Black][Chessboard::Queen]);

	m_pieces[4] = new QGraphicsSvgItem(m_squares[4]);
	m_pieces[4]->setSharedRenderer(m_renderers[Chessboard::Black][Chessboard::King]);

	m_pieces[5] = new QGraphicsSvgItem(m_squares[5]);
	m_pieces[5]->setSharedRenderer(m_renderers[Chessboard::Black][Chessboard::Bishop]);

	m_pieces[6] = new QGraphicsSvgItem(m_squares[6]);
	m_pieces[6]->setSharedRenderer(m_renderers[Chessboard::Black][Chessboard::Knight]);

	m_pieces[7] = new QGraphicsSvgItem(m_squares[7]);
	m_pieces[7]->setSharedRenderer(m_renderers[Chessboard::Black][Chessboard::Rook]);

	for (int i = 0; i < 8; i++)
	{
		m_pieces[8 + i] = new QGraphicsSvgItem(m_squares[8 + i]);
		m_pieces[8 + i]->setSharedRenderer(m_renderers[Chessboard::Black][Chessboard::Pawn]);
	}

	// == WHITE PIECES ==
	//
	// The order of piece initialization is the same as they appear in the
	// chessboard: Rook, Knigh, Bishop, Queen, King, Bishop, Knight, Rook,
	// and 8 Pawns.
	m_pieces[15] = new QGraphicsSvgItem(m_squares[56]);
	m_pieces[15]->setSharedRenderer(m_renderers[Chessboard::White][Chessboard::Rook]);

	m_pieces[16] = new QGraphicsSvgItem(m_squares[57]);
	m_pieces[16]->setSharedRenderer(m_renderers[Chessboard::White][Chessboard::Knight]);

	m_pieces[17] = new QGraphicsSvgItem(m_squares[58]);
	m_pieces[17]->setSharedRenderer(m_renderers[Chessboard::White][Chessboard::Bishop]);

	m_pieces[18] = new QGraphicsSvgItem(m_squares[59]);
	m_pieces[18]->setSharedRenderer(m_renderers[Chessboard::White][Chessboard::Queen]);

	m_pieces[19] = new QGraphicsSvgItem(m_squares[60]);
	m_pieces[19]->setSharedRenderer(m_renderers[Chessboard::White][Chessboard::King]);

	m_pieces[20] = new QGraphicsSvgItem(m_squares[61]);
	m_pieces[20]->setSharedRenderer(m_renderers[Chessboard::White][Chessboard::Bishop]);

	m_pieces[21] = new QGraphicsSvgItem(m_squares[62]);
	m_pieces[21]->setSharedRenderer(m_renderers[Chessboard::White][Chessboard::Knight]);

	m_pieces[22] = new QGraphicsSvgItem(m_squares[63]);
	m_pieces[22]->setSharedRenderer(m_renderers[Chessboard::White][Chessboard::Rook]);

	for (int i = 0; i < 8; i++)
	{
		m_pieces[23 + i] = new QGraphicsSvgItem(m_squares[48 + i]);
		m_pieces[23 + i]->setSharedRenderer(m_renderers[Chessboard::White][Chessboard::Pawn]);
	}
}

void GraphicsChessboardItem::makeMove(const ChessMove& move)
{
	// Move all child items of source square to children of
	// target square.

	// TODO: Check if move is valid.
	// TODO: Implement better debugging (asserts etc.)
	// TODO: Implement support for promotions.

	QList<QGraphicsItem*> children = m_squares[move.sourceSquare()]->childItems();
	if (!children.isEmpty())
	{
		for (int i = 0; i < children.count(); i++) 
		{
			children[i]->setParentItem(m_squares[move.targetSquare()]);
		}
	}
}

