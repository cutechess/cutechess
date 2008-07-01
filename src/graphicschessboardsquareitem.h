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

#ifndef GRAPHICSCHESSBOARDSQUAREITEM_H
#define GRAPHICSCHESSBOARDSQUAREITEM_H

#include <QGraphicsItem>

class QPainter;

/**
 * GraphicsChessboardSquareItem represents a single square in the chessboard.
 *
 * GraphicsChessboardSquareItem inherits QGraphicsItem:
 * http://doc.trolltech.com/4.4/qgraphicsitem.html
 *
 * Each square have different states. These states include selected, focused
 * and the default state. Within a chessboard only one item can have selected
 * state and one can have a focused state. A single square can have however
 * can also have these both states.
 *
 * The painting is done depending on the square's state. If the square is
 * selected the square's color is overridden with the selected color. If the
 * square is focused a marker is painted over the square using the focus marker
 * color. If the square is in the default state painting is done using light
 * or dark color depending on the square's type.
*/
class GraphicsChessboardSquareItem : public QGraphicsItem
{
	public:
		/** Specifies the square's type: light or dark. */
		enum ChessboardSquareType
		{
			LightSquare,
			DarkSquare
		};

		/**
		 * Creates a new GraphicsChessboardSquareItem object.
		 * @param parent Square's parent object (i.e. the chessboard).
		 * @param type Square's type
		*/
		GraphicsChessboardSquareItem(QGraphicsItem* parent, ChessboardSquareType type);

		/** Size of the square. */
		static const qreal size;

		/**
		 * Sets the color of all light squares.
		 * @param color Color for all light squares.
		*/
		static void setLightSquareColor(const QColor& color);
		/**
		 * Sets the color of all dark squares.
		 * @param color Color for all dark squares.
		*/
		static void setDarkSquareColor(const QColor& color);
		/**
		 * Sets the color of all selected squares.
		 * The selected color overrides squares default color
		 * when the square is selected.
		 * @param color Color for all selected squares.
		*/
		static void setSelectedSquareColor(const QColor& color);
		/**
		 * Sets the color of the focus marker.
		 * @param color Color for the focus marker.
		*/
		static void setFocusMarkerColor(const QColor& color);
		/**
		 * Returns the color of light squares.
		 * @return Color of light squares.
		*/
		static QColor lightSquareColor();
		/**
		 * Returns the color of dark squares.
		 * @return Color of dark squares.
		*/
		static QColor darkSquareColor();
		/**
		 * Returns the color of selected squares.
		 * @return Color of selected squares.
		*/
		static QColor selectedSquareColor();
		/**
		 * Returns the color of the focus marker.
		 * @return Color of the focus marker.
		*/
		static QColor focusMarkerColor();

		/**
		 * Returns the square's type: light or dark.
		 * @return Square's type.
		*/
		ChessboardSquareType squareType() const;
		/**
		 * Returns true if the square is light.
		 * @return True if the square is light.
		*/
		bool isLightSquare() const;
		/**
		 * Returns true if the square is dark.
		 * @return True if the square is dark.
		*/
		bool isDarkSquare() const;
		/**
		 * Sets the square's type.
		 * @param type Type for this square.
		*/
		void setSquareType(ChessboardSquareType type);
		/**
		 * Returns true if this square is occupied (has a chess piece).
		 * @return True if square is occupied.
		*/
		bool isOccupied() const;

		QRectF boundingRect() const;
		void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

	protected:
		void mousePressEvent(QGraphicsSceneMouseEvent* event);
		void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
		void focusOutEvent (QFocusEvent* event);
		void focusInEvent (QFocusEvent* event);
	
	private:
		static QColor m_lightSquareColor;
		static QColor m_darkSquareColor;
		static QColor m_selectedSquareColor;
		static QColor m_focusMarkerColor;

		ChessboardSquareType m_type;
		bool m_selectionWasHandled;
};

#endif // GRAPHICSCHESSBOARDSQUAREITEM_H

