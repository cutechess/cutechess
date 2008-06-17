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

#ifndef CHESSBOARDITEM_H
#define CHESSBOARDITEM_H

#include <QGraphicsItem>

class QPainter;
class QSvgRenderer;
class QGraphicsSvgItem;
class ChessboardSquareItem;

class ChessboardItem : public QGraphicsItem
{
	public:
		ChessboardItem(QGraphicsItem* parent = 0);

		ChessboardSquareItem* squareAt(int file, int rank);
		ChessboardSquareItem* squareAt(const QChar& file, const QChar& rank);
		
		QRectF boundingRect() const;
		void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
	
	private:
		ChessboardSquareItem* m_squares[8][8];
};

#endif // CHESSBOARDITEM_H

