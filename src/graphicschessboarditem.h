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

#ifndef GRAPHICSCHESSBOARDITEM_H
#define GRAPHICSCHESSBOARDITEM_H

#include <QObject>
#include <QGraphicsItem>
#include <QMap>

#include "chessboard/chessboard.h"

class QPainter;
class GraphicsChessPiece;
class QSvgRenderer;
class ChessMove;
class GraphicsChessboardSquareItem;

class GraphicsChessboardItem : public QObject, public QGraphicsItem
{
	Q_OBJECT

	public:
		GraphicsChessboardItem(const QString& svgResource = ":/default.svg",
			QGraphicsItem* parent = 0);
		~GraphicsChessboardItem();

		/** Size of the board. */
		static const qreal size;

		/** Size of the border. */
		static const qreal borderSize;

		QRectF boundingRect() const;
		void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

		bool isBorderVisible() const;
		void showBorder(bool visible);

	public slots:
		void makeMove(const ChessMove& move);
	
	private:
		void initChessboard();
		void initChessPieces();

		GraphicsChessboardSquareItem* m_squares[64];
		QSvgRenderer* m_renderer;
		GraphicsChessPiece* m_pieces[32];
		bool m_showBorder;
		QColor m_borderColor;
};

#endif // GRAPHICSCHESSBOARDITEM_H

