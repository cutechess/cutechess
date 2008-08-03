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
	if (!items.isEmpty())
	{
		fitInView(items.first(), Qt::KeepAspectRatio);
	}
}

