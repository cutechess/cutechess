#include "piecechooser.h"
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include "graphicspiece.h"


PieceChooser::PieceChooser(const QList<GraphicsPiece*>& pieces,
			   qreal squareSize,
			   QGraphicsItem* parent)
	: QGraphicsObject(parent),
	  m_squareSize(squareSize),
	  m_anim(nullptr)
{
	for (auto piece : pieces)
		if (piece != nullptr)
			m_pieces[piece->pieceType().side()] << piece;

	int columns = qMax(m_pieces[0].size(), m_pieces[1].size());
	int rows = (!m_pieces[0].isEmpty() && !m_pieces[1].isEmpty()) ? 2 : 1;

	m_rect.setWidth(m_squareSize * columns);
	m_rect.setHeight(m_squareSize * rows);
	m_rect.moveCenter(QPointF(0, 0));

	setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren);
	hide();
}

int PieceChooser::type() const
{
	return Type;
}

QRectF PieceChooser::boundingRect() const
{
	return m_rect;
}

void PieceChooser::paint(QPainter* painter,
			 const QStyleOptionGraphicsItem* option,
			 QWidget* widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	painter->setBrush(QColor(Qt::white));

	QPen pen(painter->pen());
	pen.setWidth(3);
	painter->setPen(pen);

	painter->drawRoundedRect(m_rect, 10.0, 10.0);
}

void PieceChooser::cancelChoice()
{
	emit pieceChosen(Chess::Piece());
	destroy();
}

void PieceChooser::reveal()
{
	QParallelAnimationGroup* group = new QParallelAnimationGroup(this);

	qreal y = m_rect.top() + m_squareSize / 2;
	for (int i = 0; i < 2; i++)
	{
		if (m_pieces[i].isEmpty())
			continue;

		for (int j = 0; j < m_pieces[i].size(); j++)
		{
			GraphicsPiece* piece = m_pieces[i].at(j);
			piece->setParentItem(this);

			QPropertyAnimation* posAnim = new QPropertyAnimation(piece, "pos");
			qreal x = m_rect.left() + m_squareSize * (0.5 + j);
			posAnim->setStartValue(QPointF(0, 0));
			posAnim->setEndValue(QPointF(x, y));
			posAnim->setEasingCurve(QEasingCurve::InOutQuad);
			posAnim->setDuration(300);
			group->addAnimation(posAnim);

			QPropertyAnimation* opAnim = new QPropertyAnimation(piece, "opacity");
			opAnim->setStartValue(0.0);
			opAnim->setEndValue(1.0);
			opAnim->setEasingCurve(QEasingCurve::InOutQuad);
			opAnim->setDuration(300);
			group->addAnimation(opAnim);
		}
		y += m_squareSize;
	}

	QPropertyAnimation* anim = new QPropertyAnimation(this, "opacity");
	anim->setStartValue(0.0);
	anim->setEndValue(0.6);
	anim->setEasingCurve(QEasingCurve::InOutQuad);
	anim->setDuration(300);
	group->addAnimation(anim);

	show();
	m_anim = group;
	group->start();
}

void PieceChooser::destroy()
{
	if (m_anim == nullptr)
	{
		deleteLater();
		return;
	}

	connect(m_anim, SIGNAL(finished()), this, SLOT(deleteLater()));
	m_anim->setDirection(QAbstractAnimation::Backward);
	m_anim->start();
}

void PieceChooser::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	if (event->button() != Qt::LeftButton)
		return;

	event->setAccepted(true);
	QPointF pos(mapFromScene(event->scenePos()));

	if (!contains(pos))
	{
		cancelChoice();
		return;
	}

	QGraphicsItem* item = scene()->itemAt(event->scenePos(), QTransform());
	GraphicsPiece* piece = qgraphicsitem_cast<GraphicsPiece*>(item);
	if (piece != nullptr && piece->parentItem() == this)
	{
		emit pieceChosen(piece->pieceType());
		destroy();
	}
}
