#include <QResizeEvent>

#include "chessboardview.h"

ChessboardView::ChessboardView(QWidget* parent)
	: QGraphicsView(parent)
{
}

void ChessboardView::resizeEvent(QResizeEvent* event)
{
	Q_UNUSED(event)

	QList<QGraphicsItem*> items = scene()->items();
	if (items.count() > 0)
	{
		fitInView(items.first(), Qt::KeepAspectRatio);
	}
}

