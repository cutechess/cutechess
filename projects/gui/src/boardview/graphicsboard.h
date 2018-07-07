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

#ifndef GRAPHICSBOARD_H
#define GRAPHICSBOARD_H

#include <QGraphicsItem>
#include <QColor>
#include <QVector>
#include <board/square.h>
#include <board/piece.h>
class GraphicsPiece;
class QPropertyAnimation;


/*!
 * \brief A graphical chessboard.
 *
 * GraphicsBoard is a graphical representation of the squares on a
 * chessboard. It also has ownership of the chess pieces on the
 * board, ie. it is the pieces' parent item and container.
 */
class GraphicsBoard : public QGraphicsItem
{
	public:
		/*! The type value returned by type(). */
		enum { Type = UserType + 1 };

		/*!
		 * Creates a new GraphicsBoard object.
		 *
		 * The board will have \a files files/columns and \a ranks
		 * ranks/rows, and the squares' width and height will be
		 * \a squareSize.
		 */
		explicit GraphicsBoard(int files,
				       int ranks,
				       qreal squareSize,
				       QGraphicsItem* parent = nullptr);
		/*! Destroys the GraphicsBoard object. */
		virtual ~GraphicsBoard();

		// Inherited from QGraphicsItem
		virtual int type() const;
		virtual QRectF boundingRect() const;
		virtual void paint(QPainter* painter,
				   const QStyleOptionGraphicsItem* option,
				   QWidget* widget = nullptr);

		/*!
		 * Returns the chess square at \a point.
		 *
		 * \a point is in item coordinates.
		 * Returns a null square if \a point is not on the board.
		 */
		Chess::Square squareAt(const QPointF& point) const;
		/*!
		 * Returns the position of \a square.
		 *
		 * The returned position is in item coordinates.
		 * Returns a null point if \a square is not on the board.
		 */
		QPointF squarePos(const Chess::Square& square) const;
		/*!
		 * Returns the type of piece at \a square.
		 *
		 * Returns a null piece if \a square is not on the board or
		 * if there's no piece placed on it.
		 */
		Chess::Piece pieceTypeAt(const Chess::Square& square) const;
		/*!
		 * Returns the GraphicsPiece object at \a square.
		 *
		 * Returns 0 if \a square is not on the board or if there's
		 * no piece placed on it.
		 */
		GraphicsPiece* pieceAt(const Chess::Square& square) const;
		/*!
		 * Removes the GraphicsPiece object at \a square and returns it.
		 *
		 * Returns 0 if \a square is not on the board or if there's
		 * no piece placed on it.
		 */
		GraphicsPiece* takePieceAt(const Chess::Square& square);

		/*! Deletes all pieces and removes them from the scene. */
		void clearSquares();
		/*!
		 * Sets the piece at \a square to \a piece.
		 *
		 * If \a square already contains a piece, it is deleted.
		 * If \a piece is 0, the square becomes empty.
		 */
		void setSquare(const Chess::Square& square, GraphicsPiece* piece);
		/*!
		 * Moves the piece from \a source to \a target.
		 *
		 * If \a target already contains a piece, it is deleted.
		 */
		void movePiece(const Chess::Square& source,
			       const Chess::Square& target);

		/*! Clears all highlights. */
		void clearHighlights();
		/*!
		 * Highlights squares.
		 *
		 * This function clears all previous highlights and marks the
		 * squares in \a squares as possible target squares of a chess move.
		 */
		void setHighlights(const QList<Chess::Square>& squares);

		/*!
		 * Returns true if the board is flipped;
		 * otherwise returns false.
		 */
		bool isFlipped() const;

		/*! Sets board flipping to \a flipped. */
		void setFlipped(bool flipped);

	private:
		int squareIndex(const Chess::Square& square) const;

		int m_files;
		int m_ranks;
		qreal m_squareSize;
		qreal m_coordSize;
		QRectF m_rect;
		QColor m_lightColor;
		QColor m_darkColor;
		QVector<GraphicsPiece*> m_squares;
		QPropertyAnimation* m_highlightAnim;
		bool m_flipped;
};

#endif // GRAPHICSBOARD_H
