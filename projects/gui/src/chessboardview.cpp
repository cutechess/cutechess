#include "chessboardview.h"
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
	  m_needsUpdate(true)
{
	setEnabled(false);
	m_squaresRect = QRect(0, 0, 0, 0);

	setSelectionMode(QAbstractItemView::SingleSelection);
	setSelectionBehavior(QAbstractItemView::SelectItems);

	m_lightSquareColor = QColor("#ffce9e");
	m_darkSquareColor = QColor("#d18b47");
	m_sourceSquareColor = QColor(255, 255, 0);
	m_targetSquareColor = QColor(255, 255, 0);
	m_highlightColor = QColor(Qt::darkRed);

	m_highlightPen.setColor(m_highlightColor);
	m_highlightPen.setJoinStyle(Qt::MiterJoin);

	m_pieceRenderer = new QSvgRenderer(QString(":/default.svg"), this);
	m_resizeTimer.setSingleShot(true);
	connect(&m_resizeTimer, SIGNAL(timeout()), this, SLOT(onResizeTimeout()));
}

void ChessboardView::setHighlightIndex(const QModelIndex& index)
{
	if (index == m_highlightIndex)
		return;

	if (index.flags() & Qt::ItemIsSelectable)
		m_highlightIndex = index;
	else
		m_highlightIndex = QModelIndex();

	emit mouseOver(m_highlightIndex);
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

	if (!hasMouseTracking())
		setHighlightIndex(m_dragSquare);

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
	else
		setHighlightIndex(indexAt(event->pos()));

	QAbstractItemView::mouseMoveEvent(event);
}

void ChessboardView::mouseReleaseEvent(QMouseEvent* event)
{
	Q_ASSERT(event != 0);

	if (m_dragging)
	{
		m_highlightIndex = QModelIndex();
		m_dragging = false;
		emit humanMove(m_dragSquare, indexAt(event->pos()));

		m_dragUpdateRegion = m_dragRect;
		m_dragUpdateRegion += visualRect(m_dragSquare);

		renderSquare(m_dragSquare);
		viewport()->update(m_dragUpdateRegion);
	}

	QAbstractItemView::mouseReleaseEvent(event);
}

void ChessboardView::leaveEvent(QEvent* event)
{
	setHighlightIndex(QModelIndex());
	QAbstractItemView::leaveEvent(event);
}

void ChessboardView::startDrag()
{
	Q_ASSERT(m_dragSquare.isValid());

	QRect rect = visualRect(m_dragSquare);

	m_dragOffset = rect.topLeft() - m_dragStartPos;

	m_dragPixmap = QPixmap(m_squareSize, m_squareSize);
	m_dragPixmap.fill(Qt::transparent);

	QPainter painter(&m_dragPixmap);
	renderPiece(m_dragSquare, painter.viewport(), painter);

	m_dragRect = rect;
	m_dragUpdateRegion = QRegion();
	m_dragging = true;

	renderSquare(m_dragSquare);
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

	if (m_pieceCache.contains(symbol))
	{
		painter.drawPixmap(bounds, m_pieceCache[symbol]);
		return;
	}

	QRectF squareBounds(0, 0, m_squareSize, m_squareSize);
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

	QPixmap pixmap(m_squareSize, m_squareSize);
	pixmap.fill(Qt::transparent);
	QPainter svgPainter(&pixmap);

	m_pieceRenderer->render(&svgPainter, symbol, squareBounds);
	svgPainter.end();
	m_pieceCache[symbol] = pixmap;

	painter.drawPixmap(bounds, pixmap);
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

static QColor mixColors(const QColor& color1, const QColor& color2)
{
	return QColor((color1.red() + color2.red()) / 2,
		      (color1.green() + color2.green()) / 2,
		      (color1.blue() + color2.blue()) / 2);
}

void ChessboardView::renderSquare(const QModelIndex& index)
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
		squareColor = m_bgPainter.background().color();
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

	SquareInfo::TypeFlags typeFlags(squareInfo.flags());
	if (typeFlags & SquareInfo::SourceSquare)
		squareColor = mixColors(squareColor, m_sourceSquareColor);
	if (typeFlags & SquareInfo::TargetSquare)
		squareColor = mixColors(squareColor, m_targetSquareColor);

	m_bgPainter.fillRect(sqBounds, squareColor);

	if (typeFlags & SquareInfo::HighlightedSquare)
	{
		m_bgPainter.setPen(m_highlightPen);
		int adj = m_squareSize / 10;
		m_bgPainter.drawRect(sqBounds.adjusted(adj, adj, -adj, -adj));
	}

	int count = squareInfo.pieceCount();
	if (count > 0 && (!m_dragging || index != m_dragSquare))
	{
		if (count > 1)
		{
			// Display the piece count
			m_bgPainter.setPen(m_textPen);
			QRect textBounds(sqBounds);
			textBounds.setX(textBounds.x() + 2);
			m_bgPainter.drawText(textBounds, QString::number(count));
		}
		if (squareInfo.color() == SquareInfo::HoldingsColor)
		{
			int adjust = m_squareSize / 8;
			sqBounds.adjust(adjust, adjust, 0, 0);
		}
		renderPiece(squareInfo.pieceSymbol(), sqBounds, m_bgPainter);
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

	if (m_background.size() != m_squaresRect.size())
	{
		m_pieceCache.clear();

		if (m_bgPainter.isActive())
			m_bgPainter.end();
		m_background = QPixmap(m_squaresRect.size());
		m_bgPainter.begin(&m_background);
		m_bgPainter.setBackgroundMode(Qt::OpaqueMode);
		m_bgPainter.setRenderHint(QPainter::SmoothPixmapTransform);

		m_highlightPen.setWidth(m_squareSize / 15);

		QFont font = m_bgPainter.font();
		font.setPixelSize(qMax(8, m_squareSize / 3));
		m_bgPainter.setFont(font);
	}

	int columnCount = model()->columnCount();
	int rowCount = model()->rowCount();
	for (int row = 0; row < rowCount; row++)
	{
		for (int column = 0; column < columnCount; column++)
			renderSquare(model()->index(row, column));
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
	if (!m_bgPainter.isActive())
		return;

	m_needsUpdate = true;
	for (int row = topLeft.row(); row <= bottomRight.row(); row++)
	{
		for (int column = topLeft.column(); column <= bottomRight.column(); column++)
			renderSquare(model()->index(row, column));
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

void ChessboardView::setSourceSquareColor(const QColor& color)
{
	m_sourceSquareColor = color;
}

void ChessboardView::setTargetSquareColor(const QColor& color)
{
	m_targetSquareColor = color;
}

void ChessboardView::setHighlightColor(const QColor& color)
{
	m_highlightColor = color;
}

QColor ChessboardView::lightSquareColor() const
{
	return m_lightSquareColor;
}

QColor ChessboardView::darkSquareColor() const
{
	return m_darkSquareColor;
}

QColor ChessboardView::sourceSquareColor() const
{
	return m_sourceSquareColor;
}

QColor ChessboardView::targetSquareColor() const
{
	return m_targetSquareColor;
}

QColor ChessboardView::highlightColor() const
{
	return m_highlightColor;
}

void ChessboardView::reset()
{
	resizeBoard(size());
	m_needsUpdate = true;
	QAbstractItemView::reset();
}
