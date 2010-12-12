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
#include <QPainter>
class QSvgRenderer;
namespace Chess { class GenericMove; }

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
		void setSourceSquareColor(const QColor& color);
		void setTargetSquareColor(const QColor& color);
		void setHighlightColor(const QColor& color);

		QColor lightSquareColor() const;
		QColor darkSquareColor() const;
		QColor sourceSquareColor() const;
		QColor targetSquareColor() const;
		QColor highlightColor() const;

	signals:
		void humanMove(const QModelIndex& source, const QModelIndex& target) const;
		void mouseOver(const QModelIndex& index);

	public slots:
		void reset();

	protected:
		bool edit(const QModelIndex& index, EditTrigger trigger, QEvent* event);
		bool isIndexHidden(const QModelIndex& index) const;
		int horizontalOffset() const;
		int verticalOffset() const;
		QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
		void setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags flags);
		QRegion visualRegionForSelection(const QItemSelection& selection) const;

		void mouseMoveEvent(QMouseEvent* event);
		void mousePressEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);
		void leaveEvent(QEvent* event);
		void paintEvent(QPaintEvent* event);
		void resizeEvent(QResizeEvent* event);

	protected slots:
		void dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);

	private slots:
		void onResizeTimeout();

	private:
		void renderPiece(const QString& symbol, const QRect& bounds, QPainter& painter);
		void renderPiece(const QModelIndex& index, const QRect& bounds, QPainter& painter);
		void renderSquare(const QModelIndex& index);
		void resizeBoard(const QSize& size);
		void startDrag();
		void setHighlightIndex(const QModelIndex& index);

		bool m_dragging;
		QPoint m_dragStartPos;
		QPoint m_dragOffset;
		QModelIndex m_dragSquare;
		QModelIndex m_highlightIndex;
		QPixmap m_dragPixmap;
		QRect m_dragRect;
		QRegion m_dragUpdateRegion;

		int m_squareSize;
		bool m_needsUpdate;
		QTimer m_resizeTimer;
		QColor m_lightSquareColor;
		QColor m_darkSquareColor;
		QColor m_sourceSquareColor;
		QColor m_targetSquareColor;
		QColor m_highlightColor;
		QPen m_textPen;
		QPen m_highlightPen;
		QRect m_squaresRect;
		QPixmap m_background;
		QPainter m_bgPainter;
		QSvgRenderer* m_pieceRenderer;
};

#endif // CHESSBOARDVIEW_H
