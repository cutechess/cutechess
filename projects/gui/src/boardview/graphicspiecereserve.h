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

#ifndef GRAPHICSPIECERESERVE_H
#define GRAPHICSPIECERESERVE_H

#include <QGraphicsItem>
#include <QMultiMap>
#include <board/piece.h>
class GraphicsPiece;


/*!
 * \brief A graphical reserve for captured chess pieces.
 *
 * Some chess variants like Crazyhouse have a reserve for captured
 * pieces which can be brought back to the game. This class acts as
 * a kind of separate chessboard for the piece reserve.
 */
class GraphicsPieceReserve : public QGraphicsItem
{
	public:
		/*! The type value returned by type(). */
		enum { Type = UserType + 3 };

		/*!
		 * Creates a new piece reserve.
		 *
		 * Each "square" has a width and height of \a squareSize,
		 * and squares are automatically added and removed when
		 * needed.
		 */
		explicit GraphicsPieceReserve(qreal squareSize,
					      QGraphicsItem* parent = nullptr);

		// Inherited from QGraphicsItem
		virtual int type() const;
		virtual QRectF boundingRect() const;
		virtual void paint(QPainter* painter,
				   const QStyleOptionGraphicsItem* option,
				   QWidget* widget = nullptr);

		/*! Returns the number of pieces of type \a piece. */
		int pieceCount(const Chess::Piece& piece) const;
		/*!
		 * Returns a GraphicsPiece object of type \a piece.
		 * Returns 0 if no pieces of type \a piece exist.
		 */
		GraphicsPiece* piece(const Chess::Piece& piece) const;
		/*!
		 * Removes a GraphicsPiece of type \a piece and returns it.
		 * Returns 0 if no pieces of type \a exist.
		 */
		GraphicsPiece* takePiece(const Chess::Piece& piece);
		/*!
		 * Adds \a piece to the reserve.
		 * This object becomes the parent item and container of \a piece.
		 */
		void addPiece(GraphicsPiece* piece);

	private:
		QPointF piecePos(Chess::Side side, int index) const;
		QRectF textRect(Chess::Side side, int index) const;
		QRectF textRect(const Chess::Piece& piece) const;
		void updateTiles();
		typedef QMultiMap<Chess::Piece, GraphicsPiece*> PieceMap;

		qreal m_tileWidth;
		qreal m_tileHeight;
		QRectF m_rect;
		PieceMap m_pieces;
		int m_rowCount;
		QList<Chess::Piece> m_tiles[2];
};

#endif // GRAPHICSPIECERESERVE_H
