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
#include <QSvgRenderer>
#include <QGraphicsSvgItem>

#include "chessboarditem.h"
#include "chessboardsquareitem.h"

ChessboardItem::ChessboardItem(QGraphicsItem* parent)
	: QGraphicsItem(parent)
{
	bool colorToggle = false;
	ChessboardSquareItem* square;

	for (int i = 0; i < 8; i++)
	{
		colorToggle = !colorToggle;

		for (int j = 0; j < 8; j++)
		{
			if (colorToggle)
			{
				square = new ChessboardSquareItem(this,
					ChessboardSquareItem::LightSquare);
			}
			else
			{
				square = new ChessboardSquareItem(this,
					ChessboardSquareItem::DarkSquare);
			}
			
			square->setPos(ChessboardSquareItem::size * j, ChessboardSquareItem::size * i);
			m_squares[i][j] = square;

			colorToggle = !colorToggle;
		}
	}
}

ChessboardSquareItem* ChessboardItem::squareAt(int file, int rank)
{
	Q_ASSERT_X(file >= 0 && file <= 7, "squareAt(int, int)", "invalid file (column) number");
	Q_ASSERT_X(rank >= 0 && rank <= 7, "squareAt(int, int)", "invalid rank (row) number");

	return m_squares[rank][file];
}

ChessboardSquareItem* ChessboardItem::squareAt(const QChar& file, const QChar& rank)
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

QRectF ChessboardItem::boundingRect() const
{
	// TODO: Use static const qreal
	return QRectF(0, 0, 400, 400);
}

void ChessboardItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	painter->fillRect(boundingRect(), Qt::red);
}

