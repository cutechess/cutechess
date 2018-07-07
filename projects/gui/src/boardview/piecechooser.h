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

#ifndef PIECECHOOSER_H
#define PIECECHOOSER_H

#include <QGraphicsObject>
#include <QList>
class GraphicsPiece;
class QAbstractAnimation;
namespace Chess { class Piece; }


/*!
 * \brief A light-weight QGraphicsObject dialog for selecting a chess piece.
 *
 * PieceChooser displays a list of graphical chess pieces in a simple
 * layout and lets the user to choose a piece type with the mouse.
 */
class PieceChooser : public QGraphicsObject
{
	Q_OBJECT

	public:
		/*! The type value returned by type(). */
		enum { Type = UserType + 2 };

		/*!
		 * Creates a new piece chooser.
		 *
		 * The user can choose one of the pieces in \a pieces, or
		 * select a null piece by clicking outside of the dialog.
		 * \a squareSize is the width and height of a square on the
		 * chessboard. The pieces shouldn't be bigger than that or
		 * else they'll overlap.
		 *
		 * The created dialog is hidden, and can be displayed by
		 * calling reveal().
		 */
		PieceChooser(const QList<GraphicsPiece*>& pieces,
			     qreal squareSize,
			     QGraphicsItem* parent = nullptr);

		// Inherited from QGraphicsObject
		virtual int type() const;
		virtual QRectF boundingRect() const;
		virtual void paint(QPainter* painter,
				   const QStyleOptionGraphicsItem* option,
				   QWidget* widget = nullptr);

		/*! Chooses a null piece and destroys the object. */
		void cancelChoice();

	public slots:
		/*! Reveals the dialog with an animation. */
		void reveal();
		/*! Fades the dialog out and destroys the object. */
		void destroy();

	signals:
		/*!
		 * This signal is emitted when the user has chosen a piece.
		 *
		 * If the user clicked outside of the dialog, \a piece is null.
		 *
		 * The piece chooser is destroyed automatically after the
		 * choice has been made.
		 */
		void pieceChosen(const Chess::Piece& piece);

	protected:
		// Inherited from QGraphicsObject
		virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);

	private:
		qreal m_squareSize;
		QRectF m_rect;
		QList<GraphicsPiece*> m_pieces[2];
		QAbstractAnimation* m_anim;
};

#endif // PIECECHOOSER_H
