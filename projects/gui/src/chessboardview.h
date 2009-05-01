/*
    This file is part of Cute Chess.

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

#ifndef CHESSBOARDVIEW_H
#define CHESSBOARDVIEW_H

#include <QAbstractItemView>
#include <QTimer>
class QSvgRenderer;

class ChessboardView : public QAbstractItemView
{
	Q_OBJECT

	public:
		ChessboardView(QWidget* parent = 0);
		QModelIndex indexAt(const QPoint& point) const;
		void scrollTo(const QModelIndex& index, ScrollHint hint = EnsureVisible);
		QRect visualRect(const QModelIndex& index) const;

		void setLightSquareColor(const QColor& color);
		void setDarkSquareColor(const QColor& color);

		QColor lightSquareColor() const;
		QColor darkSquareColor() const;

	public slots:
		void onMoveMade(const QModelIndex& source, const QModelIndex& target);
		void reset();

	protected:
		bool edit(const QModelIndex& index, EditTrigger trigger, QEvent* event);
		void paintEvent(QPaintEvent* event);
		bool isIndexHidden(const QModelIndex& index) const;
		int horizontalOffset() const;
		int verticalOffset() const;
		QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
		void setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags flags);
		QRegion visualRegionForSelection(const QItemSelection& selection) const;
		void resizeEvent(QResizeEvent* event);

	protected slots:
		void dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);

	private slots:
		void onResizeTimeout();

	private:
		void renderPiece(const QModelIndex& index, QPainter& painter, QRectF bounds);
		void renderSquare(const QModelIndex& index, QPainter& painter);
		void resizeBoard(const QSize& size);

		int m_squareSize;
		bool m_needsUpdate;
		QTimer m_resizeTimer;
		QColor m_lightSquareColor;
		QColor m_darkSquareColor;
		QColor m_moveColor;
		QRect m_squaresRect;
		QPixmap m_background;
		QSvgRenderer* m_pieceRenderer;
		QModelIndex m_sourceSquare;
		QModelIndex m_targetSquare;
};

#endif // CHESSBOARDVIEW_H
