#include "chessboardview.h"
#include <QPainter>
#include <QSvgRenderer>
#include <QResizeEvent>
#include <QApplication>
#include <QtDebug>
#include <board/piece.h>
#include <board/genericmove.h>
#include "squareinfo.h"


ChessboardView::ChessboardView(QWidget* parent)
	: QAbstractItemView(parent),
	  m_dragging(false),
	  m_squareSize(40),
	  m_canPaint(false),
	  m_needsUpdate(true)
{
	setEnabled(false);
	m_squaresRect = QRect(0, 0, 0, 0);

	setSelectionMode(QAbstractItemView::SingleSelection);
	setSelectionBehavior(QAbstractItemView::SelectItems);

	m_lightSquareColor = QColor("#ffce9e");
	m_darkSquareColor = QColor("#d18b47");
	m_moveColor = QColor(255, 255, 0);

	m_pieceRenderer = new QSvgRenderer(QString(":/default.svg"), this);
	m_resizeTimer.setSingleShot(true);
	connect(&m_resizeTimer, SIGNAL(timeout()), this, SLOT(onResizeTimeout()));
}

void ChessboardView::mousePressEvent(QMouseEvent *event)
{
	Q_ASSERT(event != 0);

	m_dragging = false;
	m_dragStartPos = QPoint();

	m_dragSquare = indexAt(event->pos());
	if ((m_dragSquare.flags() & Qt::ItemIsSelectable)
	&&  event->button() == Qt::LeftButton)
		m_dragStartPos = event->pos();

	QAbstractItemView::mousePressEvent(event);
}

void ChessboardView::mouseMoveEvent(QMouseEvent *event)
{
	Q_ASSERT(event != 0);

	if (m_dragging)
	{
		m_dragUpdateRegion = m_dragRect;

		QPoint pos(event->pos() + m_dragOffset);
		m_dragRect = QRect(pos, m_dragPixmap.size());
		m_dragUpdateRegion += m_dragRect;
		viewport()->update(m_dragUpdateRegion);
	}
	else if ((event->buttons() & Qt::LeftButton) && !m_dragStartPos.isNull())
	{
		int distance = (event->pos() - m_dragStartPos).manhattanLength();
		if (distance >= QApplication::startDragDistance())
		    startDrag();
	}

	QAbstractItemView::mouseMoveEvent(event);
}

void ChessboardView::mouseReleaseEvent(QMouseEvent* event)
{
	Q_ASSERT(event != 0);

	if (m_dragging)
	{
		m_dragging = false;
		emit humanMove(m_dragSquare, indexAt(event->pos()));

		m_dragUpdateRegion = m_dragRect;
		m_dragUpdateRegion += visualRect(m_dragSquare);

		QPainter painter(&m_background);
		renderSquare(m_dragSquare, painter);
		viewport()->update(m_dragUpdateRegion);
	}

	QAbstractItemView::mouseReleaseEvent(event);
}

void ChessboardView::startDrag()
{
	Q_ASSERT(m_dragSquare.isValid());

	QPainter painter;
	QRect rect = visualRect(m_dragSquare);

	m_dragOffset = rect.topLeft() - m_dragStartPos;

	m_dragPixmap = QPixmap(m_squareSize, m_squareSize);
	m_dragPixmap.fill(Qt::transparent);

	painter.begin(&m_dragPixmap);
	renderPiece(m_dragSquare, painter.viewport(), painter);
	painter.end();

	m_dragRect = rect;
	m_dragUpdateRegion = QRegion();
	m_dragging = true;

	painter.begin(&m_background);
	renderSquare(m_dragSquare, painter);
	viewport()->update(rect);
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
	if (!model() || !m_squaresRect.contains(point))
		return QModelIndex();

	int row = (point.y() - m_squaresRect.y()) / m_squareSize;
	int column = (point.x() - m_squaresRect.x()) / m_squareSize;
	return model()->index(row, column);
}

void ChessboardView::scrollTo(const QModelIndex& index, ScrollHint hint)
{
	Q_UNUSED(index);
	Q_UNUSED(hint);
}

QRect ChessboardView::visualRect(const QModelIndex& index) const
{
	if (!index.isValid())
		return QRect();

	return QRect(m_squaresRect.x() + (index.column() * m_squareSize),
		     m_squaresRect.y() + (index.row() * m_squareSize),
		     m_squareSize, m_squareSize);
}

void ChessboardView::renderPiece(const QString& symbol,
				 const QRect& bounds,
				 QPainter& painter)
{
	Q_ASSERT(!symbol.isEmpty());
	Q_ASSERT(bounds.isValid());

	QRectF squareBounds(bounds);
	QRectF pieceBounds(m_pieceRenderer->boundsOnElement(symbol));

	qreal aspect = pieceBounds.width() / pieceBounds.height();
	if (aspect > 1.0)
	{
		qreal height = bounds.height() / aspect;
		qreal adjust = (bounds.height() - height) / 2;
		squareBounds.adjust(0, adjust, 0, -adjust);
	}
	else if (aspect < 1.0)
	{
		qreal width = bounds.width() * aspect;
		qreal adjust = (bounds.width() - width) / 2;
		squareBounds.adjust(adjust, 0, -adjust, 0);
	}
	qreal a = squareBounds.width() / 10;
	squareBounds.adjust(a, a, -a, -a);

	m_pieceRenderer->render(&painter, symbol, squareBounds);
}

void ChessboardView::renderPiece(const QModelIndex& index,
				 const QRect& bounds,
				 QPainter& painter)
{
	Q_ASSERT(index.isValid());
	Q_ASSERT(bounds.isValid());

	QVariant data = model()->data(index);
	if (data.canConvert<SquareInfo>())
	{
		SquareInfo info(data.value<SquareInfo>());
		renderPiece(info.pieceSymbol(), bounds, painter);
	}
}

void ChessboardView::renderSquare(const QModelIndex& index, QPainter& painter)
{
	Q_ASSERT(index.isValid());

	int row = index.row();
	int column = index.column();
	QRect sqBounds(column * m_squareSize, row * m_squareSize,
		       m_squareSize, m_squareSize);

	QVariant data = model()->data(index);
	SquareInfo squareInfo;
	if (data.canConvert<SquareInfo>())
		squareInfo = data.value<SquareInfo>();

	QColor squareColor;
	switch (squareInfo.color())
	{
	case SquareInfo::BackgroundColor:
	case SquareInfo::HoldingsColor:
		squareColor = painter.background().color();
		break;
	case SquareInfo::LightColor:
		squareColor = m_lightSquareColor;
		break;
	case SquareInfo::DarkColor:
		squareColor = m_darkSquareColor;
		break;
	default:
		return;
	}
	if (squareInfo.type() == SquareInfo::SourceSquare
	||  squareInfo.type() == SquareInfo::TargetSquare)
		squareColor.setRgb((squareColor.red() + m_moveColor.red()) / 2,
				   (squareColor.green() + m_moveColor.green()) / 2,
				   (squareColor.blue() + m_moveColor.blue()) / 2);

	painter.fillRect(sqBounds, squareColor);

	int count = squareInfo.pieceCount();
	if (count > 0 && (!m_dragging || index != m_dragSquare))
	{
		if (count > 1)
		{
			// Display the piece count
			QRect textBounds(sqBounds);
			textBounds.setX(textBounds.x() + 2);
			painter.setFont(m_font);
			painter.drawText(textBounds, QString::number(count));
		}
		if (squareInfo.color() == SquareInfo::HoldingsColor)
		{
			int adjust = m_font.pixelSize();
			sqBounds.adjust(adjust, adjust, 0, 0);
		}
		renderPiece(squareInfo.pieceSymbol(), sqBounds, painter);
	}
}

void ChessboardView::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event);
	if (!model())
		return;

	QPainter painter(viewport());

	if (!m_needsUpdate)
	{
		painter.drawPixmap(m_squaresRect, m_background);
		if (m_dragging && m_dragRect.isValid())
		{
			Q_ASSERT(!m_dragPixmap.isNull());
			painter.drawPixmap(m_dragRect, m_dragPixmap);
		}
		return;
	}
	m_needsUpdate = false;
	m_canPaint = true;

	if (m_background.size() != m_squaresRect.size())
		m_background = QPixmap(m_squaresRect.size());
	QPainter bgPainter(&m_background);

	int columnCount = model()->columnCount();
	int rowCount = model()->rowCount();

	for (int row = 0; row < rowCount; row++)
	{
		for (int column = 0; column < columnCount; column++)
		{
			QModelIndex index = model()->index(row, column);
			renderSquare(index, bgPainter);
		}
	}

	painter.drawPixmap(m_squaresRect, m_background);
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
	Q_UNUSED(cursorAction);
	Q_UNUSED(modifiers);
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

void ChessboardView::dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
	if (!m_canPaint)
		return;

	QPainter painter(&m_background);
	painter.setBackgroundMode(Qt::OpaqueMode);

	m_needsUpdate = true;
	for (int row = topLeft.row(); row <= bottomRight.row(); row++)
	{
		for (int column = topLeft.column(); column <= bottomRight.column(); column++)
		{
			QModelIndex index = model()->index(row, column);
			renderSquare(index, painter);
		}
	}
	QAbstractItemView::dataChanged(topLeft, bottomRight);
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
	m_squaresRect.moveCenter(viewport()->rect().center());

	m_font.setPixelSize(qMax(8, m_squareSize / 3));
}

void ChessboardView::onResizeTimeout()
{
	m_needsUpdate = true;
	viewport()->update();
}

void ChessboardView::resizeEvent(QResizeEvent* event)
{
	QAbstractItemView::resizeEvent(event);
	resizeBoard(event->size());
	m_resizeTimer.start(100);
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
	resizeBoard(size());
	m_needsUpdate = true;
	QAbstractItemView::reset();
}
