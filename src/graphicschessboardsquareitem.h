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

#include "chessboard/chessboard.h"

class QPainter;

/*!
 * \brief The GraphicsChessboardSquareItem class represents a single square in
 * the visual chessboard.
 *
 * Each square can have different states. These states include selected, focused
 * and the default state. Within a visuals chessboard only one item can have
 * selected state and one can have a focused state. A single square can
 * however have both states.
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
		/*! Specifies the square's type. */
		enum ChessboardSquareType
		{
			/*! A light square. */
			LightSquare,
			/*! A dark square. */
			DarkSquare
		};

		/*!
		 * Creates a new GraphicsChessboardSquareItem of \a type as a child of
		 * \a parent.
		*/
		GraphicsChessboardSquareItem(QGraphicsItem* parent, ChessboardSquareType type);

		/*! Size of the square. */
		static const qreal size;

		/*!
		 * Sets the color of all light squares to \a color.
		*/
		static void setLightSquareColor(const QColor& color);
		/*!
		 * Sets the color of all dark squares to \a color.
		*/
		static void setDarkSquareColor(const QColor& color);
		/*!
		 * Sets the color of all selected squares to \a color.
		 *
		 * The selected color overrides squares default color
		 * when the square is selected.
		*/
		static void setSelectedSquareColor(const QColor& color);
		/*!
		 * Sets the color of the focus marker to \a color.
		*/
		static void setFocusMarkerColor(const QColor& color);
		/*!
		 * Returns the color of light squares.
		*/
		static QColor lightSquareColor();
		/*!
		 * Returns the color of dark squares.
		*/
		static QColor darkSquareColor();
		/*!
		 * Returns the color of selected squares.
		*/
		static QColor selectedSquareColor();
		/*!
		 * Returns the color of the focus marker.
		*/
		static QColor focusMarkerColor();

		/*!
		 * Returns the square's type.
		*/
		ChessboardSquareType squareType() const;
		/*!
		 * Returns true if the square is light.
		 *
		 * This method is provided for convenience.
		*/
		bool isLightSquare() const;
		/*!
		 * Returns true if the square is dark.
		 *
		 * This method is provided for convenience.
		*/
		bool isDarkSquare() const;
		/*!
		 * Sets the square's type to \a type.
		*/
		void setSquareType(ChessboardSquareType type);
		/*!
		 * Returns true if this square is occupied (has a chess piece).
		*/
		bool isOccupied() const;

		/*!
		 * Sets the square's position in the chessboard to \a pos.
		*/
		void setPositionInChessboard(Chessboard::ChessSquare pos);
		/*!
		 * Returns the square's position in the chessboard.
		*/
		Chessboard::ChessSquare positionInChessboard() const;

		/*!
		 * Returns the bounds of the square.
		*/
		QRectF boundingRect() const;
		/*!
		 * Paints the square using \a painter with set of options and \a widget as target.
		*/
		void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

	protected:
		/*! Fired when square receives a mouse press event */
		void mousePressEvent(QGraphicsSceneMouseEvent* event);
		/*! Fired when square receives a mouse release event. */
		void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
		/*! Fired when square loses focus. */
		void focusOutEvent (QFocusEvent* event);
		/*! Fired when square receives focus. */
		void focusInEvent (QFocusEvent* event);
	
	private:
		static QColor m_lightSquareColor;
		static QColor m_darkSquareColor;
		static QColor m_selectedSquareColor;
		static QColor m_focusMarkerColor;

		ChessboardSquareType m_type;
		Chessboard::ChessSquare m_posInChessboard;
		bool m_selectionWasHandled;
};

#endif // GRAPHICSCHESSBOARDSQUAREITEM_H

