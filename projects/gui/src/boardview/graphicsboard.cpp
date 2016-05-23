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

#include "graphicsboard.h"
#include <QPainter>
#include <QPropertyAnimation>
#include <board/square.h>
#include "graphicspiece.h"


class TargetHighlights : public QGraphicsObject
{
	public:
		TargetHighlights(QGraphicsItem* parentItem = nullptr)
			: QGraphicsObject(parentItem)
		{
			setFlag(ItemHasNoContents);
		}
		virtual QRectF boundingRect() const
		{
			return QRectF();
		}
		virtual void paint(QPainter* painter,
				   const QStyleOptionGraphicsItem* option,
				   QWidget* widget)
		{
			Q_UNUSED(painter);
			Q_UNUSED(option);
			Q_UNUSED(widget);
		}
};

GraphicsBoard::GraphicsBoard(int files,
			     int ranks,
			     qreal squareSize,
			     QGraphicsItem* parent)
	: QGraphicsItem(parent),
	  m_files(files),
	  m_ranks(ranks),
	  m_squareSize(squareSize),
	  m_lightColor(QColor("#ffce9e")),
	  m_darkColor(QColor("#d18b47")),
	  m_squares(files * ranks),
	  m_highlightAnim(nullptr)
{
	Q_ASSERT(files > 0);
	Q_ASSERT(ranks > 0);

	m_rect.setSize(QSizeF(squareSize * files, squareSize * ranks));
	m_rect.moveCenter(QPointF(0, 0));

	setCacheMode(DeviceCoordinateCache);
}

int GraphicsBoard::type() const
{
	return Type;
}

QRectF GraphicsBoard::boundingRect() const
{
	return m_rect;
}

void GraphicsBoard::paint(QPainter* painter,
			  const QStyleOptionGraphicsItem* option,
			  QWidget* widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	QRectF rect(m_rect.topLeft(), QSizeF(m_squareSize, m_squareSize));
	const qreal left = rect.left();

	for (int y = 0; y < m_ranks; y++)
	{
		rect.moveLeft(left);
		for (int x = 0; x < m_files; x++)
		{
			if ((x % 2) == (y % 2))
				painter->fillRect(rect, m_lightColor);
			else
				painter->fillRect(rect, m_darkColor);
			rect.moveLeft(rect.left() + m_squareSize);
		}
		rect.moveTop(rect.top() + m_squareSize);
	}
}

Chess::Square GraphicsBoard::squareAt(const QPointF& point) const
{
	if (!m_rect.contains(point))
		return Chess::Square();

	int col = (point.x() + m_rect.width() / 2) / m_squareSize;
	int row = (point.y() + m_rect.height() / 2) / m_squareSize;

	return Chess::Square(col, m_ranks - row - 1);
}

QPointF GraphicsBoard::squarePos(const Chess::Square& square) const
{
	if (!square.isValid())
		return QPointF();

	qreal left = m_rect.left() + m_squareSize / 2;
	qreal top = m_rect.top() + m_squareSize / 2;

	qreal x = left + m_squareSize * square.file();
	qreal y = top + m_squareSize * (m_ranks - square.rank() - 1);

	return QPointF(x, y);
}

Chess::Piece GraphicsBoard::pieceTypeAt(const Chess::Square& square) const
{
	GraphicsPiece* piece = pieceAt(square);
	if (piece == nullptr)
		return Chess::Piece();
	return piece->pieceType();
}

GraphicsPiece* GraphicsBoard::pieceAt(const Chess::Square& square) const
{
	if (!square.isValid())
		return nullptr;

	GraphicsPiece* piece = m_squares.at(squareIndex(square));
	Q_ASSERT(piece == nullptr || piece->container() == this);
	return piece;
}

GraphicsPiece* GraphicsBoard::takePieceAt(const Chess::Square& square)
{
	int index = squareIndex(square);
	if (index == -1)
		return nullptr;

	GraphicsPiece* piece = m_squares.at(index);
	if (piece == nullptr)
		return nullptr;

	m_squares[index] = nullptr;
	piece->setParentItem(nullptr);
	piece->setContainer(nullptr);

	return piece;
}

void GraphicsBoard::clearSquares()
{
	qDeleteAll(m_squares);
	m_squares.clear();
}

void GraphicsBoard::setSquare(const Chess::Square& square, GraphicsPiece* piece)
{
	Q_ASSERT(square.isValid());

	int index = squareIndex(square);
	delete m_squares[index];

	if (piece == nullptr)
		m_squares[index] = nullptr;
	else
	{
		m_squares[index] = piece;
		piece->setContainer(this);
		piece->setParentItem(this);
		piece->setPos(squarePos(square));
	}
}

void GraphicsBoard::movePiece(const Chess::Square& source,
			      const Chess::Square& target)
{
	GraphicsPiece* piece = pieceAt(source);
	Q_ASSERT(piece != nullptr);

	m_squares[squareIndex(source)] = nullptr;
	setSquare(target, piece);
}

int GraphicsBoard::squareIndex(const Chess::Square& square) const
{
	if (!square.isValid())
		return -1;

	return square.rank() * m_files + square.file();
}

void GraphicsBoard::clearHighlights()
{
	if (m_highlightAnim != nullptr)
	{
		m_highlightAnim->setDirection(QAbstractAnimation::Backward);
		m_highlightAnim->start(QAbstractAnimation::DeleteWhenStopped);
		m_highlightAnim = nullptr;
	}
}

void GraphicsBoard::setHighlights(const QList<Chess::Square>& squares)
{
	clearHighlights();
	if (squares.isEmpty())
		return;

	TargetHighlights* targets = new TargetHighlights(this);

	QRectF rect;
	rect.setSize(QSizeF(m_squareSize / 3, m_squareSize / 3));
	rect.moveCenter(QPointF(0, 0));
	QPen pen(Qt::white, m_squareSize / 20);
	QBrush brush(Qt::black);

	foreach (const Chess::Square& sq, squares)
	{
		QGraphicsEllipseItem* dot = new QGraphicsEllipseItem(rect, targets);

		dot->setCacheMode(DeviceCoordinateCache);
		dot->setPen(pen);
		dot->setBrush(brush);
		dot->setPos(squarePos(sq));
	}

	m_highlightAnim = new QPropertyAnimation(targets, "opacity");
	targets->setParent(m_highlightAnim);

	m_highlightAnim->setStartValue(0.0);
	m_highlightAnim->setEndValue(1.0);
	m_highlightAnim->setDuration(500);
	m_highlightAnim->setEasingCurve(QEasingCurve::InOutQuad);
	m_highlightAnim->start(QAbstractAnimation::KeepWhenStopped);
}
