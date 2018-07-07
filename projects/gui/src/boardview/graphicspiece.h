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

#ifndef GRAPHICSPIECE_H
#define GRAPHICSPIECE_H

#include <QGraphicsObject>
#include <board/piece.h>
class QSvgRenderer;

/*!
 * \brief A graphical representation of a chess piece.
 *
 * A GraphicsPiece object is a chess piece that can be easily
 * dragged and animated in a QGraphicsScene. Scalable Vector
 * Graphics (SVG) are used to ensure that the pieces look good
 * at any resolution, and a shared SVG renderer is used.
 *
 * For convenience reasons the boundingRect() of a piece should
 * be equal to that of a square on the chessboard.
 */
class GraphicsPiece : public QGraphicsObject
{
	Q_OBJECT

	public:
		/*! The type value returned by type(). */
		enum { Type = UserType + 4 };

		/*!
		 * Creates a new GraphicsPiece object of type \a piece.
		 *
		 * The painted image is scaled to fit inside a square that is
		 * \a squareSize wide and high.
		 * \a elementId is the XML ID of the piece picture which is
		 * rendered by \a renderer.
		 */
		GraphicsPiece(const Chess::Piece& piece,
			      qreal squareSize,
			      const QString& elementId,
			      QSvgRenderer* renderer,
			      QGraphicsItem* parent = nullptr);

		// Inherited from QGraphicsObject
		virtual int type() const;
		virtual QRectF boundingRect() const;
		virtual void paint(QPainter* painter,
				   const QStyleOptionGraphicsItem* option,
				   QWidget* widget = nullptr);

		/*! Returns the type of the chess piece. */
		Chess::Piece pieceType() const;
		/*!
		 * Returns the container of the piece.
		 *
		 * Usually the container is the chessboard or the piece reserve.
		 * A piece can have a container even if it doesn't have a parent
		 * item.
		 */
		QGraphicsItem* container() const;
		/*! Sets the container to \a item. */
		void setContainer(QGraphicsItem* item);

	public slots:
		/*!
		 * Restores the parent item (container).
		 *
		 * If the piece doesn't have a parent item but does have a container,
		 * this function sets the parent item to the container. Usually this
		 * function is called after an animation or drag operation that had
		 * cleared the parent item to make the piece a top-level item.
		 */
		void restoreParent();

	private:
		Chess::Piece m_piece;
		QRectF m_rect;
		QString m_elementId;
		QSvgRenderer* m_renderer;
		QGraphicsItem* m_container;
};

#endif // GRAPHICSPIECE_H
