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

GraphicsChessboardSquareItem* GraphicsChessboardItem::squareAt(int file, int rank)
{
	Q_ASSERT_X(file >= 0 && file <= 7, "squareAt(int, int)", "invalid file (column) number");
	Q_ASSERT_X(rank >= 0 && rank <= 7, "squareAt(int, int)", "invalid rank (row) number");

	return m_squares[rank][file];
}

GraphicsChessboardSquareItem* GraphicsChessboardItem::squareAt(const QChar& file, const QChar& rank)
{
	int fileNum = -1;
	
	if (file == QChar('a'))
		fileNum = 0;
	else if (file == QChar('b'))
		fileNum = 1;
	else if (file == QChar('c'))
		fileNum = 2;
	else if (file == QChar('d'))
		fileNum = 3;
	else if (file == QChar('e'))
		fileNum = 4;
	else if (file == QChar('f'))
		fileNum = 5;
	else if (file == QChar('g'))
		fileNum = 6;
	else if (file == QChar('h'))
		fileNum = 7;

	// Returns valid square or hits squareAt(int, int)'s asserts
	return squareAt(fileNum, 8 - rank.digitValue());
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
	GraphicsChessboardSquareItem* square;

	for (int i = 0; i < 8; i++)
	{
		colorToggle = !colorToggle;

		for (int j = 0; j < 8; j++)
		{
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
			
			if (isBorderVisible())
			{
				square->setPos(GraphicsChessboardSquareItem::size * j +
					GraphicsChessboardItem::borderSize,
					GraphicsChessboardSquareItem::size * i +
					GraphicsChessboardItem::borderSize);
			}
			else
			{
				square->setPos(GraphicsChessboardSquareItem::size * j,
					GraphicsChessboardSquareItem::size * i);
			}

			m_squares[i][j] = square;

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
	m_pieces[0] = new QGraphicsSvgItem(":/brook.svg", m_squares[0][0]);
	m_pieces[1] = new QGraphicsSvgItem(":/bknight.svg", m_squares[0][1]);
	m_pieces[2] = new QGraphicsSvgItem(":/bbishop.svg", m_squares[0][2]);
	m_pieces[3] = new QGraphicsSvgItem(":/bqueen.svg", m_squares[0][3]);
	m_pieces[4] = new QGraphicsSvgItem(":/bking.svg", m_squares[0][4]);
	m_pieces[5] = new QGraphicsSvgItem(":/bbishop.svg", m_squares[0][5]);
	m_pieces[6] = new QGraphicsSvgItem(":/bknight.svg", m_squares[0][6]);
	m_pieces[7] = new QGraphicsSvgItem(":/brook.svg", m_squares[0][7]);

	for (int i = 0; i < 8; i++)
	{
		m_pieces[8 + i] = new QGraphicsSvgItem(":/bpawn.svg", m_squares[1][i]);
	}

	// == WHITE PIECES ==
	//
	// The order of piece initialization is the same as they appear in the
	// chessboard: Rook, Knigh, Bishop, Queen, King, Bishop, Knight, Rook,
	// and 8 Pawns.
	m_pieces[15] = new QGraphicsSvgItem(":/wrook.svg", m_squares[7][0]);
	m_pieces[16] = new QGraphicsSvgItem(":/wknight.svg", m_squares[7][1]);
	m_pieces[17] = new QGraphicsSvgItem(":/wbishop.svg", m_squares[7][2]);
	m_pieces[18] = new QGraphicsSvgItem(":/wqueen.svg", m_squares[7][3]);
	m_pieces[19] = new QGraphicsSvgItem(":/wking.svg", m_squares[7][4]);
	m_pieces[20] = new QGraphicsSvgItem(":/wbishop.svg", m_squares[7][5]);
	m_pieces[21] = new QGraphicsSvgItem(":/wknight.svg", m_squares[7][6]);
	m_pieces[22] = new QGraphicsSvgItem(":/wrook.svg", m_squares[7][7]);

	for (int i = 0; i < 8; i++)
	{
		m_pieces[23 + i] = new QGraphicsSvgItem(":/wpawn.svg", m_squares[6][i]);
	}
}

