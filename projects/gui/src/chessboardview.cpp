#include "chessboardview.h"
#include <QPainter>
#include <QSvgRenderer>
#include <QResizeEvent>
#include <QtDebug>


ChessboardView::ChessboardView(QWidget* parent)
	: QAbstractItemView(parent),
	  m_squareSize(40)
{
	m_squaresRect = QRect(0, 0, 0, 0);

	setSelectionMode(QAbstractItemView::SingleSelection);
	setSelectionBehavior(QAbstractItemView::SelectItems);

	m_lightSquareColor = QColor("#ffce9e");
	m_darkSquareColor = QColor("#d18b47");
	m_moveColor = QColor(255, 255, 0, 127);

	m_pieceRenderer = new QSvgRenderer(QString(":/default.svg"), this);
}

bool ChessboardView::edit(const QModelIndex& index, EditTrigger trigger, QEvent* event)
{
	Q_UNUSED(index)
	Q_UNUSED(trigger)
	Q_UNUSED(event)

	// Items cannot be edited
	return false;
}

QModelIndex ChessboardView::indexAt(const QPoint& point) const
{
	if (model())
	{
		if (point.x() >= m_squaresRect.x() &&
			point.x() <= (m_squaresRect.x() + m_squaresRect.width()) &&
			point.y() >= m_squaresRect.y() &&
			point.y() <= (m_squaresRect.y() + m_squaresRect.height()))
		{
			return model()->index((point.y() - m_squaresRect.y()) / m_squareSize,
				(point.x() - m_squaresRect.x()) / m_squareSize);
		}
	}
	return QModelIndex();
}

void ChessboardView::scrollTo(const QModelIndex& index, ScrollHint hint)
{
	Q_UNUSED(index);
	Q_UNUSED(hint);
}

QRect ChessboardView::visualRect(const QModelIndex& index) const
{
	if (index.isValid())
	{
		return QRect (m_squaresRect.x() + (index.column() * m_squareSize),
			m_squaresRect.y() + (index.row() * m_squareSize),
			m_squareSize, m_squareSize);
	}

	return QRect();
}

void ChessboardView::paintEvent(QPaintEvent* event)
{
	if (!model())
		return;

	QPainter painter(viewport());

	int columnCount = model()->columnCount();
	int rowCount = model()->rowCount();

	for (int row = 0; row < rowCount; row++)
	{
		for (int column = 0; column < columnCount; column++)
		{
			QBrush squareBrush;
			QModelIndex index = model()->index(row, column);

			if (selectionModel()->isSelected(index))
				squareBrush.setStyle(Qt::Dense3Pattern);
			else
				squareBrush.setStyle(Qt::SolidPattern);

			if ((row % 2) == (column % 2))
				squareBrush.setColor(m_lightSquareColor);
			else
				squareBrush.setColor(m_darkSquareColor);

			QRectF sqBounds(visualRect(index));

			// Render the square
			painter.fillRect(sqBounds, squareBrush);

			if (index == m_sourceSquare || index == m_targetSquare)
				painter.fillRect(sqBounds, m_moveColor);

			QVariant data = model()->data(index);
			if (data.isNull())
				continue;

			// Render the piece
			QString id = data.toString();
			QRectF pieceBounds(m_pieceRenderer->boundsOnElement(id));

			qreal aspect = pieceBounds.width() / pieceBounds.height();
			if (aspect > 1.0)
			{
				qreal height = sqBounds.height() / aspect;
				qreal adjust = (sqBounds.height() - height) / 2;
				sqBounds.adjust(0, adjust, 0, -adjust);
			}
			else if (aspect < 1.0)
			{
				qreal width = sqBounds.width() * aspect;
				qreal adjust = (sqBounds.width() - width) / 2;
				sqBounds.adjust(adjust, 0, -adjust, 0);
			}
			qreal a = sqBounds.width() / 10;
			sqBounds.adjust(a, a, -a, -a);

			m_pieceRenderer->render(&painter, id, sqBounds);
		}
	}
}

bool ChessboardView::isIndexHidden(const QModelIndex& index) const
{
	Q_UNUSED(index);
	return false;
}

int ChessboardView::horizontalOffset() const
{
	return 0;
}

int ChessboardView::verticalOffset() const
{
	return 0;
}

QModelIndex ChessboardView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
	// TODO
	return QModelIndex();
}

void ChessboardView::setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags flags)
{
	QModelIndex selected = indexAt(QPoint(rect.x(), rect.y()));

	if (selected.isValid())
	{
		selectionModel()->select(selected, flags);
	}
}

QRegion ChessboardView::visualRegionForSelection(const QItemSelection& selection) const
{
	Q_UNUSED(selection);
	return QRegion();
}

void ChessboardView::resizeBoard(const QSize& size)
{
	if (!model())
		return;

	int width = size.width();
	int height = size.height();
	int columnCount = model()->columnCount();
	int rowCount = model()->rowCount();
	m_squareSize = qMin(width / columnCount, height / rowCount);

	m_squaresRect.setWidth(columnCount * m_squareSize);
	m_squaresRect.setHeight(rowCount * m_squareSize);

	if (width > m_squaresRect.width())
		m_squaresRect.setX((width - m_squaresRect.width()) / 2);
	else
		m_squaresRect.setX(0);

	if (height > m_squaresRect.height())
		m_squaresRect.setY((height - m_squaresRect.height()) / 2);
	else
		m_squaresRect.setY(0);
}

void ChessboardView::resizeEvent(QResizeEvent* event)
{
	QAbstractItemView::resizeEvent(event);
	resizeBoard(event->size());
}

void ChessboardView::onMoveMade(const QModelIndex& source, const QModelIndex& target)
{
	QModelIndex oldSource = m_sourceSquare;
	QModelIndex oldTarget = m_targetSquare;

	m_sourceSquare = source;
	m_targetSquare = target;

	update(oldSource);
	update(oldTarget);
}

void ChessboardView::setLightSquareColor(const QColor& color)
{
	m_lightSquareColor = color;
}

void ChessboardView::setDarkSquareColor(const QColor& color)
{
	m_darkSquareColor = color;
}

QColor ChessboardView::lightSquareColor() const
{
	return m_lightSquareColor;
}

QColor ChessboardView::darkSquareColor() const
{
	return m_darkSquareColor;
}

void ChessboardView::reset()
{
	m_sourceSquare = QModelIndex();
	m_targetSquare = QModelIndex();
	resizeBoard(size());
	QAbstractItemView::reset();
}
